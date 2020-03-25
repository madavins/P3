import matplotlib.pyplot as plt
import numpy as np
import scipy.io.wavfile as waves



archivo = 'C:/Users/aleix/OneDrive/Escritorio/PAV/P3/bondia.wav'
fmuestreo, audio = waves.read(archivo)
# tiempos en eje x
dt = 1/fmuestreo
#tab = np.arange(0,len(audio)-1,dt)
tab = []
for x in range(0, len(audio)):
    tab.append(x*dt)


r = np.correlate(audio, audio, mode='full')[len(audio)-1:]
print(audio.shape, r.shape)

plt.figure(figsize=(14, 5))
plt.plot(r[:10000])
plt.xlabel('Lag (samples)')
plt.xlim(0, 10000)

plt.show()