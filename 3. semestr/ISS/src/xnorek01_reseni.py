from matplotlib import pyplot as plt
import numpy as np
from scipy.io import wavfile
from scipy.signal import lfilter

# Ukol 3 
sr = 16
# Mask Off
fs, data = wavfile.read("../audio/maskoff_tone.wav")
data = data / 2**15
data = data[24000:40000]
data -= np.mean(data)

frames_off = [data[i*10*sr : i*10*sr + 20*sr] for i in range(100)]
t_off = np.arange(frames_off[69].size) / fs

# Mask On
fs, data = wavfile.read("../audio/maskon_tone.wav")
data = data / 2**15
data = data[24000:40000]
data -= np.mean(data)

frames_on = [data[i*10*sr : i*10*sr + 20*sr] for i in range(100)]
t_on = np.arange(frames_on[69].size) / fs

plt.figure()
fig, (ax1, ax2) = plt.subplots(2,1, figsize=(8,5)) 
ax1.plot(t_off, frames_off[69])
ax2.plot(t_on, frames_on[69])
ax1.set_ylabel('y')
ax1.set_xlabel('Čas [s]')
ax2.set_ylabel('y')
ax2.set_xlabel('Čas [s]')
ax1.set_title('Ramec bez roušky')
ax2.set_title('Ramec s rouškou')
plt.tight_layout()
plt.savefig('frames.pdf')

# Ukol 4
def autocorrelation(x, frames, graph, L):
    max_value = np.amax(frames[x])
    clip = (max_value / 100) * 70

    clipping = frames[x].copy()
    for i in range(len(clipping)):
        if clipping[i] >= clip:
            clipping[i] = 1
        elif abs(clipping[i]) >= clip:
            clipping[i] = -1
        else:
            clipping[i] = 0
        
    N = len(clipping) - 1
    correlate = clipping.copy()
    for k in range(N):
        for i in range(N-1-k):
            correlate[k] = correlate[k] + (clipping[i]*clipping[i+k])

    lag = np.argmax(correlate[L:])
    T = lag/fs
    F0 = 1/T
    
    if x == 69 and graph == 1:
        fig, (ax1, ax2, ax3) = plt.subplots(3,1, figsize=(10,7))
        ax1.plot(t_off, frames_off[x])
        ax2.plot(t_off, clipping)
        ax3.plot(correlate)
        stem = ax3.stem([L+lag], correlate[L+lag:L+lag+1],  linefmt='r-', markerfmt='bo', use_line_collection=True)
        line = ax3.axvline(L, color='black')
        ax3.legend((line, stem), ('Práh', 'Lag'))
        ax1.set_ylabel('y')
        ax1.set_xlabel('Čas [s]')
        ax2.set_ylabel('y')
        ax2.set_xlabel('Čas [s]')
        ax3.set_ylabel('y')
        ax3.set_xlabel('Vzorky')
        ax1.set_title('Rámec')
        ax2.set_title('Centrální klipování s 70%')
        ax3.set_title('Autokorelace')
        plt.tight_layout()
        plt.savefig('clip_correlate.pdf')

    return F0

corr_frame_off = [autocorrelation(x, frames_off, 1, int(16000/2000)) for x in range(len(frames_off))]
corr_frame_on = [autocorrelation(x, frames_on, 0, int(16000/2000)) for x in range(len(frames_on))]

var_off = np.var(corr_frame_off)
var_on = np.var(corr_frame_on)
prb = 1 / 100   
sum_off = 0
sum_on = 00
for i in range(0, 100): 
    sum_off += (corr_frame_off[i] * prb)  
    sum_on += (corr_frame_on[i] * prb)  
          
#print(sum_off, sum_on)
#print(var_off, var_on)

plt.figure(figsize=(10,2.5))
plt.plot(corr_frame_off, label = 'Bez roušky')
plt.plot(corr_frame_on, label = 'S rouškou')
plt.legend(framealpha=1, frameon=True)
plt.xlabel('Rámce')
plt.ylabel('f0')
plt.title('Základní frekvence rámců')
plt.tight_layout()
plt.savefig('ramce.pdf')

# Ukol 5 
def myDFT(frame):
    z = np.zeros(1024)
    z[:320] = np.copy(frame)
    array =[]
    for k in range(1024):
        value = 0
        for t in range(1024):
            value += z[t] * (np.e ** (-2 * np.pi * 1j * t * k/1024))
        array.append(value)
    return array
if 0:
    myDFT_off = []
    myDFT_on = []
    for i in range(len(frames_off)):
        myDFT_off.append(myDFT(frames_off[i]))
        myDFT_on.append(myDFT(frames_on[i]))

DFT_off = [np.fft.fft(frames_off[x], 1024) for x in range(100)]
spec_off = 10*np.log10(np.square(np.abs(DFT_off)))

DFT_on = [np.fft.fft(frames_on[x], 1024) for x in range(100)]
spec_on = 10*np.log10(np.square(np.abs(DFT_on)))

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 6))
graph1 = ax1.imshow(np.transpose(spec_off[:,:512]),aspect='auto', origin='lower', extent=(0, 1, 0, 8000))
graph2 = ax2.imshow(np.transpose(spec_on[:,:512]),aspect='auto', origin='lower', extent=(0, 1, 0, 8000))
cbar1 = fig.colorbar(graph1, ax=ax1)
cbar2 = fig.colorbar(graph2, ax=ax2)
cbar1.set_label('Spektralní hustota výkonu [dB]', rotation=270, labelpad=15)
cbar2.set_label('Spektralní hustota výkonu [dB]', rotation=270, labelpad=15)
ax1.set_ylabel('Frekvence [Hz]')
ax1.set_xlabel('Čas [s]')
ax2.set_ylabel('Frekvence[Hz]')
ax2.set_xlabel('Čas [s]')
ax1.set_title('Spektogram bez roušky')
ax2.set_title('Spektogram s rouškou')
plt.tight_layout()
plt.savefig('Spektogram.pdf')

# Ukol 6 
H = np.array(DFT_on) / np.array(DFT_off)
H = np.mean(np.abs(H),axis=0)
char_filter = 10*np.log10(np.square(np.abs(H)))

f = np.arange(char_filter.size) / 1000 * fs
plt.figure(figsize=(8,3)) 
plt.plot(f[:f.size//2+1], char_filter[:char_filter.size//2+1])
plt.ylabel('y')
plt.xlabel('Frekvence[Hz]')
plt.title('Frekvenční charakteristika roušky')
plt.tight_layout()
plt.savefig('freq_char.pdf')

# Ukol 7
def myIDFT(frame):
    z = np.zeros(1024)
    z[:512] = np.copy(frame)
    array =[]
    for k in range(1024):
        value = 0
        for t in range(1024):
            value += z[t] * (np.e ** (2 * np.pi * 1j * t * k/1024))
        value = value / 1024
        array.append(value)
    return array
if 0:
    myIDFT_off = myIDFT(H[:512])
    myIDFT_on = myIDFT(H[:512])

IDFT = np.fft.ifft(H[:512], 1024)
plt.figure(figsize=(10,4)) 
plt.plot(IDFT[:512].real)
plt.ylabel('y')
plt.xlabel('vzorky')
plt.title('Impulzivní odezva')
plt.tight_layout()
plt.savefig('odezva.pdf')

# Ukol 8 
fs, maskoff = wavfile.read("../audio/maskoff_sentence.wav")
fs, maskoff_tone = wavfile.read("../audio/maskoff_tone.wav")
fs, maskon = wavfile.read("../audio/maskon_sentence.wav")

sim_maskon = lfilter(IDFT[:512].real, [1], maskoff)
sim_maskon_tone = lfilter(IDFT[:512].real, [1], maskoff_tone)

plt.figure(figsize=(10,4)) 
plt.plot(maskoff, label = 'Puvodni bez roušky')
plt.plot(maskon, label = 'Původní s rouškou')
plt.plot(sim_maskon, label = 'Simulace roušky')
plt.legend(framealpha=1, frameon=True)
plt.xlabel('vzorky')
plt.ylabel('y')
plt.title('Rozdíl')
plt.tight_layout()
plt.savefig('rozdil.pdf')

sim_maskon = sim_maskon.astype(np.float32)
sim_maskon_tone = sim_maskon_tone.astype(np.float32)
wavfile.write("../audio/sim_maskon_sentence.wav", int(fs), sim_maskon)
wavfile.write("../audio/sim_maskon_tone.wav", int(fs), sim_maskon_tone)

# Ukol 11 
array_test = np.hamming(51)

plt.figure(figsize=(10,4)) 
plt.plot(array_test)
plt.xlabel('vzorky')
plt.ylabel('y')
plt.title('Hamming v časové oblasti')
plt.tight_layout()
plt.savefig('hamming.pdf')

array_DFT = np.fft.fft(array_test, 1024)
mag = np.abs(np.fft.fftshift(array_DFT))

plt.figure()
freq = np.linspace(-0.5, 0.5, len(array_DFT))
response = 20 * np.log10(mag)
response = np.clip(response, -100, 100)
plt.plot(freq, response)
plt.xlabel('vzorky')
plt.ylabel('amplituda')
plt.title('Hamming ve spektrální oblasti')
plt.tight_layout()
plt.savefig('hamming_DFT.pdf')

array = np.hamming(1024)
hamm_off = np.multiply(array, DFT_off)
hamm_on = np.multiply(array, DFT_on)

plt.figure(figsize=(10,4)) 
plt.plot(DFT_off[5][:512].real, label='Rámec bez okénkové funkce')
plt.plot(hamm_off[5][:512].real, label='Rámec s okénkovou funkcí')
plt.legend(framealpha=1, frameon=True)
plt.xlabel('vzorky')
plt.ylabel('y')
plt.tight_layout()
plt.savefig('hamming_porovnani.pdf')

H_hamm = np.array(hamm_on) / np.array(hamm_off)
H_hamm = np.mean(np.abs(H_hamm),axis=0)

char_hamm = 10*np.log10(np.square(np.abs(H_hamm)))

f = np.arange(char_hamm.size) / 1000 * fs

IDFT_hamm = np.fft.ifft(H_hamm[:512], 1024)
sim_hamm = lfilter(IDFT_hamm[:512].real, [1], maskoff)
sim_hamm_tone = lfilter(IDFT_hamm[:512].real, [1], maskoff_tone)
sim_hamm = sim_hamm.astype(np.float32)
sim_hamm_tone = sim_hamm_tone.astype(np.float32)
wavfile.write("../audio/sim_maskon_sentence_window.wav", int(fs), sim_hamm)
wavfile.write("../audio/sim_maskon_tone_window.wav", int(fs), sim_hamm_tone)

# ukol 13 
same_on = []
same_off = []
for i in range(100):
    if corr_frame_off[i] == corr_frame_on[i]:
        same_on.append(frames_on[i])
        same_off.append(frames_off[i])

DFT_same_off = [np.fft.fft(same_off[x], 1024) for x in range(len(same_off))]
DFT_same_on = [np.fft.fft(same_on[x], 1024) for x in range(len(same_on))]

H_same = np.array(DFT_same_on) / np.array(DFT_same_off)
H_same = np.mean(np.abs(H_same),axis=0)

char_same_filter = 10*np.log10(np.square(np.abs(H_same)))
plt.figure(figsize=(8,3)) 
plt.plot(char_filter[:512], label='Původní')
plt.plot(char_same_filter[:512], label='Stejná frekvence')
plt.ylabel('y')
plt.xlabel('vzorky')
plt.title('Frekvenční charakteristika roušky')
plt.legend(framealpha=1, frameon=True)
plt.tight_layout()
plt.savefig('freq_char_same.pdf')

IDFT_same = np.fft.ifft(H_same[:512], 1024)
sim_same = lfilter(IDFT_same[:512].real, [1], maskoff)
sim_same_tone = lfilter(IDFT_same[:512].real, [1], maskoff_tone)
sim_same = sim_same.astype(np.float32)
sim_same_tone = sim_same_tone.astype(np.float32)
wavfile.write("../audio/sim_maskon_sentence_only_match.wav", int(fs), sim_same)
wavfile.write("../audio/sim_maskon_tone_only_match.wav", int(fs), sim_same_tone)
