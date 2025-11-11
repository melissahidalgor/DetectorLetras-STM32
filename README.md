# 🌟 Proyecto: DNN Detector de Letras

Este proyecto implementa un sistema de Reconocimiento de Actividad (HAR - Human Activity Recognition) en un microcontrolador STM32F401 (Black Pill), utilizando una Red Neuronal Densa (DNN). El objetivo es clasificar tres movimientos específicos, trazados en el aire con la tarjeta, que representan las letras 'M', 'H' y 'R'

## 💻 Hardware y Componentes

| Componente | Conexión/ Tecnología| Función Principal |
| :--- | :---: | :--- |
| **STM32F411 BlackPill** | Microcontrolador | Utilizado por su capacidad de procesamiento |
| **MPU6050** | I2C| IMU - Unidad de Medición Inercial |

## ⚙️ Flujo de Trabajo

1. **Adquisición y Captura de Dataset**
   * **Muestreo en el MCU:** El frimware captura los datos del acelerómentro a una frecuencia de 10 Hz por un período de 3 segundos.
   * **Captura Dataset:** Los datos son enviados a un Jupyter Notebook como cadena de valores separados por coma a través de un puerto serial virutal (VCP). Se capturaron 90 movimientos para cada letra (M,H,R) con ventanas de 30 muestras en cada movimiento.
    
2. **Preprocesaiento**

    Para reducir la dimensionalidad y optimizar la inferencia en el microcontrolador, se utilizó la técnica de extracción de características sobre las ventanas de 30 muestras. Se divide la ventana de 30 muestras en 5 bloques de 6 datos cado uno y se extrae la *Media* y la *Desviación Estándar* de los ejes X, Y y Z.

    En el MCU se emplea la librería CMSIS-DSP para implementar las funciones `arm_mean_f32` y `arm_std_f3`

3. **Entrenamiento**

    Se entrena usando el framework TensorFlow/Keras por 300 épocas la red neuronal con la siguiente arquitectura:

      * **Capa de Entrada:** Capa *Flatten* que recibe el vector de 30 características
      * **Capas Ocultas:** 2 capas Ocultas de 30 y 20 neuronas con activación *ReLu* y tasa de *Dropout* del 50% 
      * **Capa de Salida:** 3 neuronas con activación *Softmax* para clasificar las 3 letras.
        
    El modelo tuvo un peso de 6.30 KB

4. **Despliegue (MCU)**
      El despliegue del modelo Keras (.h5) en la Black Pill se realizóa través del paquete de expansión de software STM32Cube.AI (`X-CUBE-AI`), que actúa como un puente entre el framework de entrenamiento TensorFlow y el firmware embebido.

    Para  ejecutar el modelo se integró la fase caputra y el preprocesamiento para la extracción de características de las nuevas inferencias. El postprocesamiento consistió en analizar el buffer de salida  para determinar la clase con mayor probabilidad e imprimir el resultado de la inferencia por el puerto serial virtual USB (VCP).

🧠
