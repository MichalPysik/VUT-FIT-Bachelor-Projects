import numpy as np
from matplotlib import pyplot as plt
from scipy.io import wavfile
from scipy.signal import spectrogram, lfilter, unit_impulse, sosfilt

#AUTOR: Michal Pysik (xpysik00)
#Tento soubor obsahuje implementaci bonusove ulohy 12, z puvodniho reseni je prevzato pouze to nejnutnejsi
#obrazky generuje zase do src/images, generuje vsak pouze obrazky k dane uloze

########## -----UKOL 3----- ##########

##extrakce sekundoveho signalu z tonu bez rousky
sampleRate, data = wavfile.read("../audio/maskoff_tone.wav")
second = np.array( [range(sampleRate)] * 2, dtype=float ) #pole pro 2 1-sekundove vzorky, uvazujeme ze budou mit stejnou Fs, tzn 16000 vzorku kazdy signal
offset = 12000
end = sampleRate + offset
second[0] = data[offset : end] #vybrana sekunda z nahravky tonu bez masky
##extrakce sekundoveho signalu z tonu s rouskou
sampleRate, data = wavfile.read("../audio/maskon_tone.wav")
offset = 33630
end = sampleRate + offset
second[1] = data[offset : end]

##ustredneni a normalizace obou techto sekundovych signalu
for i in range(2):
	second[i] -= np.mean(second[i])
	second[i] /= np.abs(second[i]).max()

##vypocet velikosti jednoho ramce
numOfFrames = 99
frameSize = int(2*sampleRate/(numOfFrames+1)) #zde pro vypocet uvazujeme ze ramcu je 100, realne jich je 99 protoze posledni nechceme

#sada ramcu pro ton bez rousky
framesWithout = np.array([range(frameSize)] * numOfFrames, dtype=float)
for i in range(numOfFrames):
	start = int(i*frameSize/2)
	end = start + frameSize
	framesWithout[i] = second[0][start : end]

#sada ramcu pro ton s rouskou
framesWith = np.array([range(frameSize)] * numOfFrames, dtype=float)
for i in range(numOfFrames):
	start = int(i*frameSize/2)
	end = start + frameSize
	framesWith[i] = second[1][start : end]



########## -----UKOL 4 (MODIFIED)----- ##########

#pole vysledku mereni frekvenci
f0Without = np.array(range(numOfFrames), dtype=float) #pole 320 namerenych frekvenci f0 signalu bez rousky
f0With = np.array(range(numOfFrames), dtype=float) #to stejne pro signal s rouskou

#Nova 2D pole pro ulozeni obou upravenych sad ramcu
framesWithoutCopy = np.array([range(frameSize)] * numOfFrames, dtype=float)
framesWithCopy = np.array([range(frameSize)] * numOfFrames, dtype=float)

#Center clipping pro ramce bez rousky
for frame in range(numOfFrames):
	max96 = 0.96 * np.abs(framesWithout[frame]).max()
	for sample in range (frameSize):
		if framesWithout[frame][sample] >= max96:
			framesWithoutCopy[frame][sample] = 1
		elif framesWithout[frame][sample] <= -max96:
			framesWithoutCopy[frame][sample] = -1
		else:
			framesWithoutCopy[frame][sample] = 0

#Center clipping pro ramce s rouskou
for frame in range(numOfFrames):
	max96 = 0.96 * np.abs(framesWith[frame]).max()
	for sample in range (frameSize):
		if framesWith[frame][sample] >= max96:
			framesWithCopy[frame][sample] = 1
		elif framesWith[frame][sample] <= -max96:
			framesWithCopy[frame][sample] = -1
		else:
			framesWithCopy[frame][sample] = 0


#funkce pro autokorelaci signalu
def autoKorelace(x):
	vysledek = np.correlate(x,x, 'full')
	return vysledek[vysledek.size//2:]

#nastaveni prahu 500Hz (muj signal ma kolem 260Hz takze netreba menit)
prah = 500
prahIndex = int(sampleRate/prah)

#Autokorelace ramce signalu bez rousky
for p in range(numOfFrames):
	framesWithoutCopy[p] = autoKorelace(framesWithoutCopy[p])
	lag = np.argmax(framesWithoutCopy[p][prahIndex:]) + prahIndex
	f0Without[p] = sampleRate/lag

#Autokorelace ramce signalu s rouskou
for p in range(numOfFrames):
	framesWithCopy[p] = autoKorelace(framesWithCopy[p])
	lag = np.argmax(framesWithCopy[p][prahIndex:]) + prahIndex
	f0With[p] = sampleRate/lag


#######################################################################################################
########## -----UKOL 12----- ########## <--------------------------------- DULEZITA CAST TOHOTO SOUBORU
#######################################################################################################

#ulozeni obrazku ramce se spatne urcenym lagem
plt.figure("Autokorelace s chybne nalezenym lagem", figsize=(16.0,4.0))
plt.title("Autokorelace ramce bez rousky 7 s chybne nalezenym lagem")
plt.plot(framesWithoutCopy[7])
plt.axvline(x=prahIndex, label='Prah', color='black')
plt.scatter(np.argmax(framesWithoutCopy[7][prahIndex:]) + prahIndex,framesWithoutCopy[7][np.argmax(framesWithCopy[p][prahIndex:]) + prahIndex] + 2, color='red', label='lag')
plt.xlabel("vzorky")
plt.ylabel("y")
plt.legend()
plt.savefig('images/12.1.svg', format='svg')
plt.close()

#urceni chyb lagu, koeficient musi byt nejlepe blizko cislu 2
coeff = 1.9

for x in range(numOfFrames):
	if coeff * f0Without[x] < np.median(f0Without) or 1/coeff * f0Without[x] > np.median(f0Without):
		f0Without[x] = np.median(f0Without)
	if coeff * f0With[x] < np.median(f0With) or 1/coeff * f0With[x] > np.median(f0With):
		f0With[x] = np.median(f0With)




########## -----UKOL 5----- ##########

#promenne pro ulozeni vysledku dft (fft) a ulozeni spekter po logaritmovani
DFTspecWithout = np.array( [range(1024)] * numOfFrames, dtype = complex)
DFTspecWith = np.array( [range(1024)] * numOfFrames, dtype = complex)
spectrumWithout = np.array( [range(513)] * numOfFrames, dtype = float)
spectrumWith = np.array( [range(513)] * numOfFrames, dtype = float)

#spocitani dft spektra a logaritmovani
for i in range(numOfFrames):
	DFTspecWithout[i] = np.fft.fft(framesWithout[i], 1024)
	DFTspecWith[i] = np.fft.fft(framesWith[i], 1024)
	spectrumWithout[i] = 10 * np.log10( np.abs(DFTspecWithout[i][:513])**2 )
	spectrumWith[i] = 10 * np.log10( np.abs(DFTspecWith[i][:513])**2 )



########## -----UKOL 6----- ##########

#vypocet frekvencni charakteristiky z kazdeho ramce
freqResponses = np.array( [range(1024)] * numOfFrames, dtype = complex )
freqResponses = np.abs(DFTspecWith) / np.abs(DFTspecWithout) #H(jw) = Y(jw)/X(jw)

#prumerovani pres ramce
AVGfreqResponse = np.array( np.zeros(1024))
for i in range (numOfFrames):
	AVGfreqResponse += freqResponses[i]
AVGfreqResponse /= numOfFrames

powerSpect = 10 * np.log10( np.abs(AVGfreqResponse[:513])**2 )

#vykresleni frekvencni charakteristiky filtru s aplikovanou Window function jako vykonove spektrum
plt.figure("Frek. char. filtru s Hamming window", figsize=(8.0, 4.0))
plt.title("Frek. char. filtru s Hamming window")
plt.plot(np.arange(0, sampleRate/2, sampleRate/1026), powerSpect)
plt.xlabel('Frekvence [Hz]')
plt.ylabel("Velikost [dB]")
plt.savefig('images/11.3.svg', format='svg')
plt.close()



########## -----UKOL 7----- ##########

#inverse fft
idftNP = np.fft.ifft(AVGfreqResponse)

#inverzni FFT na ziskani impulsni odezvy filtru nemusim provadet znovu
FIRrouska = idftNP[:512].real #pouzivam vysledek z NP, protoze muj neni kompatibilni s np.real



########## -----UKOL 8----- ##########

#neni zde potreba, nic se neuklada
