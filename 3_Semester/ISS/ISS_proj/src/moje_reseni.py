import numpy as np
from matplotlib import pyplot as plt
from scipy.io import wavfile
from scipy.signal import spectrogram, lfilter, unit_impulse, sosfilt

#AUTOR: Michal Pysik (xpysik00)
#Tento soubor obsahuje splneni vsech zakladnich uloh (3 az 8)
#Pozadovene grafy generuje do slozky src/images

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

#ulozeni obrazku ramce cislo 39 (indexovani 0 - 98, takze 40. ramec) obou signalu
plt.figure("ramec 39 signalu bez rousky")
plt.title("ramec 39 signalu bez rousky")
plt.plot( np.arange( 390.0, 410.0, 20/len(framesWithout[39]) ), framesWithout[39] )
plt.xlabel("time [ms]")
plt.ylabel("y")
plt.savefig('images/3.1.svg', format='svg')
plt.close()
plt.figure("ramec 39 signalu s rouskou")
plt.title("ramec 39 signalu s rouskou")
plt.plot( np.arange( 390.0, 410.0, 20/len(framesWith[39]) ), framesWith[39] )
plt.xlabel("time [ms]")
plt.ylabel("y")
plt.savefig('images/3.2.svg', format='svg')
plt.close()



########## -----UKOL 4----- ##########

#ulozeni obrazku vybraneho ramce pred zpracovanim
plt.figure("ramec 55 signalu bez rousky", figsize=(16.0,4.0))
plt.title("ramec 55 signalu bez rousky")
plt.plot(framesWithout[55])
plt.xlabel("vzorky")
plt.ylabel("y")
plt.savefig('images/4.1.svg', format='svg')
plt.close()

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

#ulozeni obrazku vybraneho ramce po central clippingu
plt.figure("stejny ramec po central clippingu", figsize=(16.0,4.0))
plt.title("Central clipping s 70%")
plt.plot(framesWithoutCopy[55])
plt.xlabel("vzorky")
plt.ylabel("y")
plt.savefig('images/4.2.svg', format='svg')
plt.close()

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

#ulozeni obrazku vybraneho ramce po autokorelaci
plt.figure("stejny ramec po autokorelaci", figsize=(16.0,4.0))
plt.title("Autokorelace")
plt.plot(framesWithoutCopy[55])
plt.axvline(x=prahIndex, label='Prah', color='black')
plt.scatter(np.argmax(framesWithoutCopy[55][prahIndex:]) + prahIndex ,framesWithoutCopy[55][np.argmax(framesWithCopy[p][prahIndex:]) + prahIndex], color='red', label='lag')
plt.xlabel("vzorky")
plt.ylabel("y")
plt.legend()
plt.savefig('images/4.3.svg', format='svg')
plt.close()

#ulozeni obrazku s porovnanim zakladnich frekvenci
plt.figure("Zakladni frekvence ramcu", figsize=(16.0,4.0))
plt.title("Zakladni frekvence ramcu")
plt.plot(f0Without, label='bez rousky')
plt.plot(f0With, label='s rouskou')
plt.legend()
plt.xlabel("ramce")
plt.ylabel("f0 [Hz]")
plt.savefig('images/4.4.svg', format='svg')
plt.close()

#kalkulace strednich hodnot a rozptylu
E_Without = np.mean(f0Without)
D_Without = np.mean(f0Without**2) - E_Without**2
E_With = np.mean(f0With)
D_With = np.mean(f0With**2) - E_With**2
print("Stredni hodnota a rozptyl bez rousky: " + str(E_Without) + " " + str(D_Without))
print("Stredni hodnota a rozptyl s rouskou: " + str(E_With) + " " + str(D_With))



########## -----UKOL 5----- ##########

#implementace DFT - casova slozitost O(n^2), FFT ma O(n*log(n))
def DFT(x, N = None):
	if N is None:
		N = len(x) #kdyz neni zadan argument N, N je delka vstup. signalu
	t = []
	L = np.minimum(len(x), N)
	for k in range(N):
		a = 0
		for n in range(L):
			a += x[n]*np.exp(-2j*np.pi*k*n*(1/N))
		t.append(a)
	return t

#porovnani me implementace DFT s np.fft.fft
testMoje = DFT(framesWithout[30], 1024)
testNP = np.fft.fft(framesWithout[30], 1024)
prumerMoje = (0 + 0j)
prumerNP = (0 + 0j)
for index in range(513): #poslednich 511 indexu jsou symetricke
	prumerMoje += testMoje[index]
	prumerNP += testNP[index]
prumerMoje /= 513
prumerNP /= 513
print("Ar. prumer hodnot z meho DFT: " + str(prumerMoje))
print("Ar. prumer hodnot z Numpy FFT: " + str(prumerNP))

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

#vykresleni spektrogramu bez rousky
plt.figure("Spectrogram bez rousky", figsize=(18,7))
plt.title("Spectrogram bez rousky")
plt.imshow(spectrumWithout.T, aspect='auto', extent=(0,1,sampleRate/2,0))
plt.gca().invert_yaxis()
plt.xlabel('time')
plt.ylabel('frequency [Hz]')
plt.colorbar()
plt.savefig('images/5.1.svg', format='svg')
plt.close()

#vykresleni spektrogramu s rouskou
plt.figure("Spectrogram s rouskou", figsize=(18,7))
plt.title("Spectrogram s rouskou")
plt.imshow(spectrumWith.T, aspect='auto', extent=(0,1,sampleRate/2,0))
plt.gca().invert_yaxis()
plt.xlabel('time')
plt.ylabel('frequency [Hz]')
plt.colorbar()
plt.savefig('images/5.2.svg', format='svg')
plt.close()



########## -----UKOL 6----- ##########

#vypocet frekvencni charakteristiky z kazdeho ramce
freqResponses = np.array( [range(1024)] * numOfFrames, dtype = float )
freqResponses = np.abs(DFTspecWith) / np.abs(DFTspecWithout) #H(jw) = Y(jw)/X(jw)

#prumerovani pres ramce
AVGfreqResponse = np.array( np.zeros(1024))
for i in range (numOfFrames):
	AVGfreqResponse += freqResponses[i]
AVGfreqResponse /= numOfFrames

powerSpect = 10 * np.log10( np.abs(AVGfreqResponse[:513])**2 )

#vykresleni frekvencni charakteristiky filtru jako vykonove spektrum
plt.figure("Frekvencni charakteristika filtru", figsize=(8.0, 4.0))
plt.title("Frekvencni charakteristika filtru")
plt.plot(np.arange(0, sampleRate/2, sampleRate/1026), powerSpect)
plt.xlabel('Frekvence [Hz]')
plt.ylabel("Velikost [dB]")
plt.savefig('images/6.1.svg', format='svg')
plt.close()



########## -----UKOL 7----- ##########

#moje implementace IDTF, opet O(n^2)
def IDFT(t, N = None):
	if N is None:
		N = len(t) #argument N je opet volitelny
	x = []
	L = np.minimum(len(t), N)
	for n in range(N):
		a = 0
		for k in range(L):
			a += t[k]*np.exp(2j*np.pi*k*n*(1/N))
		a /= N
		x.append(a)
	return x

#porovnani me IDFT s np.fft.ifft
idftMoje = IDFT(AVGfreqResponse)
idftNP = np.fft.ifft(AVGfreqResponse)
prumerMoje = (0 + 0j)
prumerNP = (0 + 0j)
for index in range(1024):
	prumerMoje += idftMoje[index]
	prumerNP += idftNP[index]
prumerMoje /= 1024
prumerNP /= 1024
print("Ar. prumer hodnot z meho IDFT: " + str(prumerMoje))
print("Ar. prumer hodnot z Numpy IFFT: " + str(prumerNP))

#inverzni FFT na ziskani impulsni odezvy filtru nemusim provadet znovu
FIRrouska = idftNP[:512].real #pouzivam vysledek z NP, protoze muj neni kompatibilni s np.real

#graf impulsni odezvy rousky
plt.figure("Impulsni odezva rousky", figsize=(8.0, 4.0))
plt.title("Impulsni odezva rousky")
plt.ylabel("h[n]")
plt.xlabel("n")
plt.plot(FIRrouska)
plt.savefig('images/7.1.svg', format='svg')
plt.close()



########## -----UKOL 8----- ##########

sampleRate, vetaBezRousky = wavfile.read("../audio/maskoff_sentence.wav")
sampleRate, vetaRealRouska = wavfile.read("../audio/maskon_sentence.wav")
sampleRate, tonBezRousky = wavfile.read("../audio/maskoff_tone.wav")

#filtrace a zapsani do souboru tonu
tonSimulatedRouska = np.round(lfilter( FIRrouska, np.array([1]), tonBezRousky )).astype(np.int16)
wavfile.write('../audio/sim_maskon_tone.wav', sampleRate, tonSimulatedRouska)

#filtrace a zapsani do souboru vety
vetaSimulatedRouska = np.round(lfilter( FIRrouska, np.array([1]), vetaBezRousky )).astype(np.int16)
wavfile.write('../audio/sim_maskon_sentence.wav', sampleRate, vetaSimulatedRouska)

#graf vety bez rousky
plt.figure("Veta bez rousky", figsize=(10.0,4.0))
plt.title("Veta bez rousky")
plt.ylabel("y")
plt.xlabel("samples")
plt.plot(vetaBezRousky)
plt.savefig('images/8.1.svg', format='svg')
plt.close()

#graf vety s rouskou
plt.figure("Veta s rouskou", figsize=(10.0,4.0))
plt.title("Veta s rouskou")
plt.ylabel("y")
plt.xlabel("samples")
plt.plot(vetaRealRouska)
plt.savefig('images/8.2.svg', format='svg')
plt.close()

#graf vety se simulovanou rouskou
plt.figure("Veta se simulovanou rouskou", figsize=(10.0,4.0))
plt.title("Veta se simulovanou rouskou")
plt.ylabel("y")
plt.xlabel("samples")
plt.plot(vetaSimulatedRouska)
plt.savefig('images/8.3.svg', format='svg')
plt.close()
