
/**
  ******************************************************************************
  * @file    app_x-cube-ai.c
  * @author  X-CUBE-AI C code generator
  * @brief   AI program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

 /*
  * Description
  *   v1.0 - Minimum template to show how to use the Embedded Client API
  *          model. Only one input and one output is supported. All
  *          memory resources are allocated statically (AI_NETWORK_XX, defines
  *          are used).
  *          Re-target of the printf function is out-of-scope.
  *   v2.0 - add multiple IO and/or multiple heap support
  *
  *   For more information, see the embeded documentation:
  *
  *       [1] %X_CUBE_AI_DIR%/Documentation/index.html
  *
  *   X_CUBE_AI_DIR indicates the location where the X-CUBE-AI pack is installed
  *   typical : C:\Users\[user_name]\STM32Cube\Repository\STMicroelectronics\X-CUBE-AI\7.1.0
  */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#if defined ( __ICCARM__ )
#elif defined ( __CC_ARM ) || ( __GNUC__ )
#endif

/* System headers */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "main.h"
#include "ai_datatypes_defines.h"
#include "network.h"
#include "network_data.h"

/* USER CODE BEGIN includes */
#include <stdio.h>	       // Para sprintf
#include <string.h>        // Para memcpy
#include "usbd_cdc_if.h"   // Para imprimir por usb-host
#include "arm_math.h"      // Para media y desviación estandar
#include "MPU6050.h"
/* USER CODE END includes */

/* IO buffers ----------------------------------------------------------------*/

#if !defined(AI_NETWORK_INPUTS_IN_ACTIVATIONS)
AI_ALIGNED(4) ai_i8 data_in_1[AI_NETWORK_IN_1_SIZE_BYTES];
ai_i8* data_ins[AI_NETWORK_IN_NUM] = {
data_in_1
};
#else
ai_i8* data_ins[AI_NETWORK_IN_NUM] = {
NULL
};
#endif

#if !defined(AI_NETWORK_OUTPUTS_IN_ACTIVATIONS)
AI_ALIGNED(4) ai_i8 data_out_1[AI_NETWORK_OUT_1_SIZE_BYTES];
ai_i8* data_outs[AI_NETWORK_OUT_NUM] = {
data_out_1
};
#else
ai_i8* data_outs[AI_NETWORK_OUT_NUM] = {
NULL
};
#endif

/* Activations buffers -------------------------------------------------------*/

AI_ALIGNED(32)
static uint8_t pool0[AI_NETWORK_DATA_ACTIVATION_1_SIZE];

ai_handle data_activations0[] = {pool0};

/* AI objects ----------------------------------------------------------------*/

static ai_handle network = AI_HANDLE_NULL;

static ai_buffer* ai_input;
static ai_buffer* ai_output;

static void ai_log_err(const ai_error err, const char *fct)
{
  /* USER CODE BEGIN log */
  if (fct)
    printf("TEMPLATE - Error (%s) - type=0x%02x code=0x%02x\r\n", fct,
        err.type, err.code);
  else
    printf("TEMPLATE - Error - type=0x%02x code=0x%02x\r\n", err.type, err.code);

  do {} while (1);
  /* USER CODE END log */
}

static int ai_boostrap(ai_handle *act_addr)
{
  ai_error err;

  /* Create and initialize an instance of the model */
  err = ai_network_create_and_init(&network, act_addr, NULL);
  if (err.type != AI_ERROR_NONE) {
    ai_log_err(err, "ai_network_create_and_init");
    return -1;
  }

  ai_input = ai_network_inputs_get(network, NULL);
  ai_output = ai_network_outputs_get(network, NULL);

#if defined(AI_NETWORK_INPUTS_IN_ACTIVATIONS)
  /*  In the case where "--allocate-inputs" option is used, memory buffer can be
   *  used from the activations buffer. This is not mandatory.
   */
  for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++) {
	data_ins[idx] = ai_input[idx].data;
  }
#else
  for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++) {
	  ai_input[idx].data = data_ins[idx];
  }
#endif

#if defined(AI_NETWORK_OUTPUTS_IN_ACTIVATIONS)
  /*  In the case where "--allocate-outputs" option is used, memory buffer can be
   *  used from the activations buffer. This is no mandatory.
   */
  for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++) {
	data_outs[idx] = ai_output[idx].data;
  }
#else
  for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++) {
	ai_output[idx].data = data_outs[idx];
  }
#endif

  return 0;
}

static int ai_run(void)
{
  ai_i32 batch;

  batch = ai_network_run(network, ai_input, ai_output);
  if (batch != 1) {
    ai_log_err(ai_network_get_error(network),
        "ai_network_run");
    return -1;
  }

  return 0;
}

/* USER CODE BEGIN 2 */
int acquire_and_process_data(ai_i8* data[])
{
  /* fill the inputs of the c-model
  for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++ )
  {
      data[idx] = ....
  }

  */

//************************************************************************************************* DATOS CRUDOS MPU
	const int STRIDE = 30;
	const int sampling_delay = 100;    // Delay para que sea una frecuencia de 10 HZ (30 datos por 3 segundos)

	char buffer[70];

	// Arrays para almacenar 30 muestras de cada eje
	float x_data[STRIDE];
	float y_data[STRIDE];
	float z_data[STRIDE];
	float Ax, Ay, Az;


	int sample_index = 0; // Índice de la muestra (0 a 29)

	while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) != GPIO_PIN_RESET);

	HAL_Delay(50);  // 50 ms bouncing delay
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

	int start_time = HAL_GetTick();
	// El bucle se ejecuta hasta que tengamos las 30 muestras
	while (sample_index < STRIDE)
	{
		// 1. Lectura del sensor
		MPU6050_Read_Accel(&Ax, &Ay, &Az);

		// 2. Guardar los datos en los arrays separados usando el índice de la muestra
		x_data[sample_index] = Ax;
		y_data[sample_index] = Ay;
		z_data[sample_index] = Az;

		sample_index++; // Avanzar al siguiente índice de muestra (1 a 30)

		// 3. Control del tiempo (frecuencia de muestreo)
		HAL_Delay(sampling_delay);
	}
	int sample_time = HAL_GetTick() - start_time;  // Tiempo que muestro, deben ser 3 segundos

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // LED OFF
	sprintf(buffer,"Tiempo (ms): %d \n\rTotal de muestras: %d\n\r",sample_time, sample_index);
	CDC_Transmit_FS((uint8_t*)buffer, strlen(buffer));

//*********************************************************************************************** EXTRACCION FEATURES

	const int WINDOW_SIZE = 6;     // Bloques de 6 datos
	const int TOTAL_FEATURES = 6;  // X_mean, Y_mean, Z_mean, X_std, Y_std, Z_std
	const int NUM_WINDOWS = STRIDE/WINDOW_SIZE; // 30 / 6 = 5

	// Arrays de salida para las medias y desviaciones estándar por eje
	float x_mean[NUM_WINDOWS];
	float x_std[NUM_WINDOWS];
	float y_mean[NUM_WINDOWS];
	float y_std[NUM_WINDOWS];
	float z_mean[NUM_WINDOWS];
	float z_std[NUM_WINDOWS];

	// El array final de 30 características para el modelo (X_mean, Y_mean, Z_mean, X_std, Y_std, Z_std)
	float features_vector[TOTAL_FEATURES*NUM_WINDOWS];


 /* void arm_mean/std_f32(
		const float32_t * pSrc,  // 1. Puntero al inicio del array (la "ventana")
		uint32_t blockSize,      // 2. El número de elementos en el array (WINDOW_SIZE)
		float32_t * pResult      // 3. Puntero donde se almacenará el resultado (la Media)
	);*/

	for (int i = 0; i < NUM_WINDOWS; i++)
	{
		// 1. Calcular la Media (Mean)
		arm_mean_f32(x_data + (i*WINDOW_SIZE), WINDOW_SIZE, x_mean + i);
		arm_mean_f32(y_data + (i*WINDOW_SIZE), WINDOW_SIZE, y_mean + i);
		arm_mean_f32(z_data + (i*WINDOW_SIZE), WINDOW_SIZE, z_mean + i);

		// 2. Calcular la Desviación Estandar (STD)
		arm_std_f32(x_data + (i*WINDOW_SIZE), WINDOW_SIZE, x_std + i);
		arm_std_f32(y_data + (i*WINDOW_SIZE), WINDOW_SIZE, y_std + i);
		arm_std_f32(z_data + (i*WINDOW_SIZE), WINDOW_SIZE, z_std + i);
	}

  /* void *memcpy(
		void *destino,           // 1. Puntero a la dirección de memoria donde se van a copiar los datos
		const void *origen,      // 2. Puntero a la dirección de memoria de donde se van a leer los datos
		size_t num_bytes         // 3. El número exacto de bytes que se deben copiar
	);*/
	// 3. Comprime los valores en un solo vector
	memcpy(features_vector,      x_mean, NUM_WINDOWS * sizeof(float));
	memcpy(features_vector + 5,  y_mean, NUM_WINDOWS * sizeof(float));
	memcpy(features_vector + 10, z_mean, NUM_WINDOWS * sizeof(float));
	memcpy(features_vector + 15, x_std,  NUM_WINDOWS * sizeof(float));
	memcpy(features_vector + 20, y_std,  NUM_WINDOWS * sizeof(float));
	memcpy(features_vector + 25, z_std,  NUM_WINDOWS * sizeof(float));

	sprintf(buffer,"Features (5-XYZ Mean and 5-XYZ STD): \n\r");
	CDC_Transmit_FS((uint8_t*)buffer, strlen(buffer));
	for(int i = 0; i < TOTAL_FEATURES*NUM_WINDOWS; i++)
	{
		sprintf(buffer,"%f, ", features_vector[i]);
		CDC_Transmit_FS((uint8_t*)buffer, strlen(buffer));
		HAL_Delay(5);
	}

//---------------------------------------------------------------------------------------------

	// Puntero al buffer de entrada del modelo (donde el motor AI espera los datos)
	float *input_buffer = (float *)data[0];

	// Copia el vector de 30 características al buffer de entrada del modelo.
	memcpy(input_buffer, features_vector, (TOTAL_FEATURES * NUM_WINDOWS) * sizeof(float));

  return 0;
}

int post_process(ai_i8* data[])
{
  /* process the predictions
  for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++ )
  {
      data[idx] = ....
  }
  */

	const int NUM_CLASSES = 3;

	// El buffer de salida (data[0] en este contexto) contiene las probabilidades (Softmax)
	float *output_buffer = (float *)data[0];

	float max_prob = -1.0f;
	int predicted_class = -1;
	char buffer[70];

	// 1. ENCONTRAR LA CLASE CON MAYOR PROBABILIDAD (np.argmax)
	for (int i = 0; i < NUM_CLASSES; i++)
	{
		if (output_buffer[i] > max_prob)
		{
			max_prob = output_buffer[i];
	        predicted_class = i;
	    }
	}

	// 2. MOSTRAR EL RESULTADO DE LA PREDICCIÓN (CDC_Transmit_FS)
    sprintf(buffer, "\n\n\r--- Prediccion AI ---\n\rClase: %d  (Prob: %.2f) ---  ", predicted_class, max_prob);
	CDC_Transmit_FS((uint8_t*)buffer, strlen(buffer));
	HAL_Delay(5);
	switch (predicted_class)
	{
		case 0:
			sprintf(buffer, "M\n\n\r");
			break;
		case 1:
			sprintf(buffer, "H\n\n\r");
			break;
		case 2:
			sprintf(buffer, "R\n\n\r");
			break;
		default:
			sprintf(buffer, "Clase desconocida\n\n\r");
			break;
	}
	CDC_Transmit_FS((uint8_t*)buffer, strlen(buffer));



  return 0;
}
/* USER CODE END 2 */

/* Entry points --------------------------------------------------------------*/

void MX_X_CUBE_AI_Init(void)
{
    /* USER CODE BEGIN 5 */
  printf("\r\nTEMPLATE - initialization\r\n");

  ai_boostrap(data_activations0);
    /* USER CODE END 5 */
}

void MX_X_CUBE_AI_Process(void)
{
    /* USER CODE BEGIN 6 */
  int res = -1;

  printf("TEMPLATE - run - main loop\r\n");

  if (network) {

    do {
      /* 1 - acquire and pre-process input data */
      res = acquire_and_process_data(data_ins);
      /* 2 - process the data - call inference engine */
      if (res == 0)
        res = ai_run();
      /* 3- post-process the predictions */
      if (res == 0)
        res = post_process(data_outs);
    } while (res==0);
  }

  if (res) {
    ai_error err = {AI_ERROR_INVALID_STATE, AI_ERROR_CODE_NETWORK};
    ai_log_err(err, "Process has FAILED");
  }
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
