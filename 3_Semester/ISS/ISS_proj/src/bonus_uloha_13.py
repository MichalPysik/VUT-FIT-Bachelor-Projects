import numpy as np
from matplotlib import pyplot as plt
from scipy.io import wavfile
from scipy.signal import spectrogram, lfilter, unit_impulse, sosfilt

#AUTOR: Michal Pysik (xpysik00)
#Tento soubor obsahuje implementaci bonusove ulohy 13, z puvodniho reseni je prevzato pouze to nejnutnejsi
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



########## -----UKOL 4----- ##########

#pole vysledku mereni frekvenci
f0Without = np.array(range(numOfFrames), dtype=float) #pole 320 namerenych frekvenci f0 signalu bez rousky
f0With = np.array(range(numOfFrames), dtype=float) #to stejne pro signal s rouskou

#Nova 2D pole pro ulozeni obou upravenych sad ramcu
framesWithoutCopy = np.array([range(frameSize)] * numOfFrames, dtype=float)
framesWithCopy = np.array([range(frameSize)] * numOfFrames, dtype=float)

#Center clipping pro ramce bez rousky
for frame in range(numOfFrames):
	max70 = 0.7 * np.abs(framesWithout[frame]).max()
	for sample in range (frameSize):
		if framesWithout[frame][sample] >= max70:
			framesWithoutCopy[frame][sample] = 1
		elif framesWithout[frame][sample] <= -max70:
			framesWithoutCopy[frame][sample] = -1
		else:
			framesWithoutCopy[frame][sample] = 0

#Center clipping pro ramce s rouskou
for frame in range(numOfFrames):
	max70 = 0.7 * np.abs(framesWith[frame]).max()
	for sample in range (frameSize):
		if framesWith[frame][sample] >= max70:
			framesWithCopy[frame][sample] = 1
		elif framesWith[frame][sample] <= -max70:
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
########## -----UKOL 13----- ########## <--------------------------------- DULEZITA CAST TOHOTO SOUBORU
#######################################################################################################

#jedina povolena zakladni frekvence
allowedF0 = f0Without[0]

#najdeme indexy v obou sadach ramcu, kde se f0 lisi at se jich muzeme zbavit
badIndexesWithout = []
badIndexesWith = []

for i in range(numOfFrames):
	if f0Without[i] != allowedF0:
		badIndexesWithout.append(i)
	if f0With[i] != allowedF0:
		badIndexesWith.append(i)

print("bez rousky - nalezeno " + str(len(badIndexesWithout)) + " spatnych ramcu, jejich indexy:")
for x in range(len(badIndexesWithout)):
	print(badIndexesWithout[x])

print("s rouskou - nalezeno " + str(len(badIndexesWith)) + " spatnych ramcu, jejich indexy:")
for y in range(len(badIndexesWith)):
	print(badIndexesWith[y])

NEWnumOfFrames = numOfFrames - len(badIndexesWithout) # 99 - 7 = 92 validnich ramcu
NEWframesWithout = np.array([range(frameSize)] * NEWnumOfFrames, dtype=float)
NEWframesWith = np.array([range(frameSize)] * NEWnumOfFrames, dtype=float)

cntWithout = 0
cntWith = 0

#funkce ktera vraci true kdyz je dany element v listu
def is_in_list(value, list):
	for i in range(len(list)):
		if list[i] == value:
			return True
	return False

#nove sady ramcu budou mit kazda pouze 92 ramcu se stejnou zakladni frekvenci
for i in range(numOfFrames):
	if is_in_list(i, badIndexesWithout) == False:
		NEWframesWithout[cntWithout] = framesWithout[i]
		cntWithout += 1
	if is_in_list(i, badIndexesWithout) == False:
		NEWframesWith[cntWith] = framesWith[i]
		cntWith += 1

numOfFrames = NEWnumOfFrames # neni nutne uchovavat starou hodnotu 99, nyni je ramcu jen 92



########## -----UKOL 5----- ##########

#promenne pro ulozeni vysledku dft (fft) a ulozeni spekter po logaritmovani
DFTspecWithout = np.array( [range(1024)] * numOfFrames, dtype = complex)
DFTspecWith = np.array( [range(1024)] * numOfFrames, dtype = complex)
spectrumWithout = np.array( [range(513)] * numOfFrames, dtype = float)
spectrumWith = np.array( [range(513)] * numOfFrames, dtype = float)

#spocitani dft spektra a logaritmovani
for i in range(numOfFrames):
	DFTspecWithout[i] = np.fft.fft(NEWframesWithout[i], 1024)
	DFTspecWith[i] = np.fft.fft(NEWframesWith[i], 1024)
	spectrumWithout[i] = 10 * np.log10( np.abs(DFTspecWithout[i][:513])**2 )
	spectrumWith[i] = 10 * np.log10( np.abs(DFTspecWith[i][:513])**2 )



########## -----UKOL 6 + zbytek 13----- ##########

#vypocet frekvencni charakteristiky z kazdeho ramce
freqResponses = np.array( [range(1024)] * numOfFrames, dtype = float )
freqResponses = np.abs(DFTspecWith) / np.abs(DFTspecWithout) #H(jw) = Y(jw)/X(jw)

#prumerovani pres ramce
AVGfreqResponse = np.array( np.zeros(1024))
for i in range (numOfFrames):
	AVGfreqResponse += freqResponses[i]
AVGfreqResponse /= numOfFrames

powerSpect = 10 * np.log10( np.abs(AVGfreqResponse[:513])**2 )

#vykresleni frekvencni charakteristiky filtru jako vykonove spektrum POUZE Z RAMCU SE STEJNOU f0
plt.figure("Frekvencni charakteristika filtru (match_only)", figsize=(8.0, 4.0))
plt.title("Frekvencni charakteristika filtru (match_only)")
plt.plot(np.arange(0, sampleRate/2, sampleRate/1026), powerSpect)
plt.xlabel('frekvence [Hz]')
plt.savefig('images/13.1.svg', format='svg')
plt.close()



########## -----UKOL 7----- ##########

#inverse fft
idftNP = np.fft.ifft(AVGfreqResponse)

#inverzni FFT na ziskani impulsni odezvy filtru nemusim provadet znovu
FIRrouska = idftNP[:512].real #pouzivam vysledek z NP, protoze muj neni kompatibilni s np.real



########## -----UKOL 8----- ##########

sampleRate, vetaBezRousky = wavfile.read("../audio/maskoff_sentence.wav")
sampleRate, vetaRealRouska = wavfile.read("../audio/maskon_sentence.wav")
sampleRate, tonBezRousky = wavfile.read("../audio/maskoff_tone.wav")

#filtrace a zapsani do souboru tonu
tonSimulatedRouska = np.round(lfilter( FIRrouska, np.array([1]), tonBezRousky )).astype(np.int16)
wavfile.write('../audio/sim_maskon_tone_only_match.wav', sampleRate, tonSimulatedRouska)

#filtrace a zapsani do souboru vety
vetaSimulatedRouska = np.round(lfilter( FIRrouska, np.array([1]), vetaBezRousky )).astype(np.int16)
wavfile.write('../audio/sim_maskon_sentence_only_match.wav', sampleRate, vetaSimulatedRouska)
