PAV - P3: detección de pitch
============================

Esta práctica se distribuye a través del repositorio GitHub [Práctica 3](https://github.com/albino-pav/P3).
Siga las instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para realizar un `fork` de la
misma y distribuir copias locales (*clones*) del mismo a los distintos integrantes del grupo de prácticas.

Recuerde realizar el *pull request* al repositorio original una vez completada la práctica.

Ejercicios básicos
------------------

- Complete el código de los ficheros necesarios para realizar la detección de pitch usando el programa
  `get_pitch`.

   * Complete el cálculo de la autocorrelación e inserte a continuación el código correspondiente.

	```cpp
 	for (unsigned int l = 0; l < r.size(); ++l)
  	{
    	
    	for (unsigned int n = 0; n < (x.size() - l); n++)
    	{
      	    r[l] = r[l] + x[n] * x[n + l];
    	}

    	r[l] = (1.0F / x.size()) * r[l];

  	}
	```

   * Inserte una gŕafica donde, en un *subplot*, se vea con claridad la señal temporal de un sonido sonoro
     y su periodo de pitch; y, en otro *subplot*, se vea con claridad la autocorrelación de la señal y la
	 posición del primer máximo secundario.

	 NOTA: es más que probable que tenga que usar Python, Octave/MATLAB u otro programa semejante para
	 hacerlo. Se valorará la utilización de la librería matplotlib de Python.

	`Gràfica amb PYTHON`

	<img src="img/python.png" width="720" align="center">

	`Codi utilitzat:`

	```python
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
	```



   * Determine el mejor candidato para el periodo de pitch localizando el primer máximo secundario de la
     autocorrelación. Inserte a continuación el código correspondiente.

     ```cpp
     vector<float>::const_iterator iR = r.begin(), iRMax = iR;
     //Creem dos iteradors auxiliars, que pendran els valors immediament inferior i posterior
     //al valor de la posició actual de l'iterador
     vector<float>::const_iterator iRanterior = r.begin();
     vector<float>::const_iterator iRposterior = r.begin();

     //Iniciem iRposterior
     iRposterior = iR + 1;

     /*Bucle que serveix per establir el miním valor coherent del segon pic de l'autocorrelació
      Utilitzem 3 condicions:
    	- 1era condició: Ens assegura que arribem fins abaix del primer pic de l'autocorrelació
    	- 2na condició: Ens assegura, en cas que el primer pic baixi molt ràpid, que superem el valor
    	  mínim establert 
    	- 3era condició: Ens assegura que en cas de tenir un primer pic molt ample, avançem el suficient
     com per a no pendre com a segon pic un valor que encara sigui del primer pic
     */
     while (*iR > *iRposterior || iR < r.begin() + npitch_min || *iR > 0.0F)
     {
      ++iR;
      ++iRposterior;
     }

     iRMax = iR;

     /* Un cop hem establert el valor mínim coherent de la distancia al segon pic,
     comprovem fent ús de iRanterior i iRposterior que iRMax es trobi en un pic, comprovant
     que iR és a l'hora major que iRanterior i iRposterior
     - Establim com a condició que iR no pot superar el llindar npitch_max - 
     */
     while (iR < r.begin() + npitch_max)
     {
       if (*iR > *iRMax)
     {
       iRanterior = iR - 1;
       iRposterior = iR + 1;
       if (*iR > *iRanterior && *iR > *iRposterior)
       iRMax = iR;
     }
       ++iR;
     }

     unsigned int lag = iRMax - r.begin();
    ```

  * Implemente la regla de decisión sonoro o sordo e inserte el código correspondiente.

- Una vez completados los puntos anteriores, dispondrá de una primera versión del detector de pitch. El 
  resto del trabajo consiste, básicamente, en obtener las mejores prestaciones posibles con él.

  * Utilice el programa `wavesurfer` para analizar las condiciones apropiadas para determinar si un
    segmento es sonoro o sordo. 
	
	  - Inserte una gráfica con la detección de pitch incorporada a `wavesurfer` y, junto a ella, los 
	    principales candidatos para determinar la sonoridad de la voz: el nivel de potencia de la señal
		(r[0]), la autocorrelación normalizada de uno (r1norm = r[1] / r[0]) y el valor de la
		autocorrelación en su máximo secundario (rmaxnorm = r[lag] / r[0]).

		Puede considerar, también, la conveniencia de usar la tasa de cruces por cero.

	    Recuerde configurar los paneles de datos para que el desplazamiento de ventana sea el adecuado, que
		en esta práctica es de 15 ms.


	<img src="img/wave1.png" width="720" align="center">

	Observant els diferents paràmetres, podem extreure les conclusions següents:
	 * Tal i com vam observar a la pràctica anterior, el paràmetre de la potència [dB] és un paràmetre que
 	  resulta útil si es combina amb d'altres paràmetres, ja que sinó, com podem veure, en segons quines
          parts de l'audio, aquesta no pateix canvis suficientment signficiatius com per utilitzar-la com a 
	  criteri únic. Per tant, l'hem d'utilitzar establint un llindar no gaire restrictiu, com podria ser
	  el de considerar Unvoiced als segments amb potència inferior als -70dB.
	 * Pel que fa als paràmetres referents a l'autocorrelació, observem que presenten contorns similars
	  a la gràfica del pitch, pero amb algunes irregularitats (sobretot Rmax normalitzada), per tant, 
	  tal i com també hem comentat amb la potència, el més addient seria establir criteris que tinguin
	  en compte els tres paràmetres a l'hora. Observem com el contorn de la gràfica de R1Norm s'assimila 
          bastant a la del pitch, aleshores, per corregir els seus errors, per exemple, podriem considerar Unvoiced 
          qualsevol trama que tingui o bé RmaxNorm o R1Norm per sota d'un cert llindar, ja que així evitem les 
          irregularitats d'ambudes gràfiques. A simple vista és difícil establir els llindars, però podriem dir
	  que tots dos han de ser superiors aproximadament a 0.6 per a dir que estem en estat VOICED.
 


    - Use el detector de pitch implementado en el programa `wavesurfer` en una señal de prueba y compare
	    su resultado con el obtenido por la mejor versión de su propio sistema.  Inserte una gráfica
		ilustrativa del resultado de ambos detectores.

	<img src="img/wave2.png" width="720" align="center">
  
  * Optimice los parámetros de su sistema de detección de pitch e inserte una tabla con las tasas de error
    y el *score* TOTAL proporcionados por `pitch_evaluate` en la evaluación de la base de datos 
	`pitch_db/train`..

   * Inserte una gráfica en la que se vea con claridad el resultado de su detector de pitch junto al del
     detector de Wavesurfer. Aunque puede usarse Wavesurfer para obtener la representación, se valorará
	 el uso de alternativas de mayor calidad (particularmente Python).
   

Ejercicios de ampliación
------------------------

- Usando la librería `docopt_cpp`, modifique el fichero `get_pitch.cpp` para incorporar los parámetros del
  detector a los argumentos de la línea de comandos.
  
  Esta técnica le resultará especialmente útil para optimizar los parámetros del detector. Recuerde que
  una parte importante de la evaluación recaerá en el resultado obtenido en la detección de pitch en la
  base de datos.

  * Inserte un *pantallazo* en el que se vea el mensaje de ayuda del programa y un ejemplo de utilización
    con los argumentos añadidos.

- Implemente las técnicas que considere oportunas para optimizar las prestaciones del sistema de detección
  de pitch.

  Entre las posibles mejoras, puede escoger una o más de las siguientes:

  * Técnicas de preprocesado: filtrado paso bajo, *center clipping*, etc.
  * Técnicas de postprocesado: filtro de mediana, *dynamic time warping*, etc.
  * Métodos alternativos a la autocorrelación: procesado cepstral, *average magnitude difference function*
    (AMDF), etc.
  * Optimización **demostrable** de los parámetros que gobiernan el detector, en concreto, de los que
    gobiernan la decisión sonoro/sordo.
  * Cualquier otra técnica que se le pueda ocurrir o encuentre en la literatura.

  Encontrará más información acerca de estas técnicas en las [Transparencias del Curso](https://atenea.upc.edu/pluginfile.php/2908770/mod_resource/content/3/2b_PS Techniques.pdf)
  y en [Spoken Language Processing](https://discovery.upc.edu/iii/encore/record/C__Rb1233593?lang=cat).
  También encontrará más información en los anexos del enunciado de esta práctica.

  Incluya, a continuación, una explicación de las técnicas incorporadas al detector. Se valorará la
  inclusión de gráficas, tablas, código o cualquier otra cosa que ayude a comprender el trabajo realizado.

  También se valorará la realización de un estudio de los parámetros involucrados. Por ejemplo, si se opta
  por implementar el filtro de mediana, se valorará el análisis de los resultados obtenidos en función de
  la longitud del filtro.
   

Evaluación *ciega* del detector
-------------------------------

Antes de realizar el *pull request* debe asegurarse de que su repositorio contiene los ficheros necesarios
para compilar los programas correctamente ejecutando `make release`.

Con los ejecutables construidos de esta manera, los profesores de la asignatura procederán a evaluar el
detector con la parte de test de la base de datos (desconocida para los alumnos). Una parte importante de
la nota de la práctica recaerá en el resultado de esta evaluación.
