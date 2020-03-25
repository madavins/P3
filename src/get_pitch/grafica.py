import numpy as np
import matplotlib.pyplot as plt
import scipy.io.wavfile as waves
import math as math

#Especificamos el archivo de audio a través de su PATH
archivo = 'C:/Users/aleix/OneDrive/Escritorio/PAV/P3/adeu.wav'
#Leemos el archivo de audio, obteniendo la frecuencia de muestreo y el vector con sus muestras
fmuestreo, audio = waves.read(archivo)

# Creamos el vector de Tiempo para el eje x
dt = 1/fmuestreo
tab = []
for x in range(0, len(audio)):
    tab.append(x*dt)

#Usamos ventana de Hamming para enventanar el AUDIO
a0 = 0.53836
a1 = 0.46164
window = []
for n in range (0, len(audio)):
    window.append(a0 - a1 * math.cos((2 * math.pi  * n) / (len(audio) - 1)))

#Recalculamos el audio enventanado
audioEnventanado = audio*window
#Calculamos la autocorrelacion del audio
autocorrelacio = np.correlate(audioEnventanado, audioEnventanado, "same")
#Ajustamos la distancia de la autocorrelacion
distancia = np.arange(-0.5*len(autocorrelacio), 0.5*len(autocorrelacio), 1)


# Salida a gràfico
# Configuramos subplots, espaciados y límites de los ejes, títulos y estilos de línea
plt.subplot(211)
plt.title("Senyal d'Audio")
plt.plot(tab, audio, linewidth =0.7)
plt.xlim(0,tab[len(audio)-1])
plt.xlabel('Temps (s)')
plt.ylabel('WaveForm')
plt.grid(True)
plt.subplot(212)
plt.xlabel('Distancia')
plt.ylabel('Autocorrelació')
plt.plot(distancia, autocorrelacio, linewidth =0.7)
plt.xlim(-0.5*len(autocorrelacio),0.5*len(autocorrelacio))
plt.xticks(np.arange(-4000, 4001, step=1000))
plt.grid(True)
plt.show()


