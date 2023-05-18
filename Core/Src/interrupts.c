/************************************************************************************************
 Copyright (c) 2023, Emiliano Arnedo <emiarnedo@gmail.com>
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all copies or substantial
 portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 SPDX-License-Identifier: MIT
 *************************************************************************************************/

/* === Headers files inclusions =============================================================== */

#include "loop.h"
#include "interrupts.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_i2c.h"
#include "stm32f1xx.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

uint16_t contReb = 2000;
uint32_t Frequency = 0;
float Duty = 0;
uint8_t boton = 0;
bool flag_timer1 = true;

/* === Private function declarations =========================================================== */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	if (flag_timer1 == true) {
		//MX_TIM1_Init();	//inicializo el contador para que solo trabaje cuando se presiona un boton.
		boton = GPIO_Pin; //guardo que boton se presiono para que el timer ejecute la rutina adecuada.
		HAL_TIM_Base_Start_IT(&htim1);	//activo el timer
	}
	//__HAL_TIM_SET_COUNTER(&htim1, 0);	//reinicia la cuenta.
	flag_timer1 = false;//desactivo la bandera para que no se esté inicializando el contador todo el tiempo

}

/* === Public function implementation ========================================================== */

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {

	while (contReb > 1) {
		contReb--;
	}

	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) { //Verifica que la interrupcion provenga del channel 1. (falling)

		if (modo < CONFIG_TEMP) {
			modo = INICIO;
			//act_flag = 1;
		} else if ((modo == CONFIG_TEMP) && (flag_prim_config == false)) {
			modo = INICIO;
			//act_flag = 1;
		} else if ((modo == CONFIG_TEMP) && (flag_prim_config == true)) {
			modo = INICIO_ALARM;
			alarma = alarma_final; //Para no perder el valor de alarma que configure.

		} else {
			modo = INICIO_ALARM;

		}

		act_flag = true;
		//Logica activo baja
		//cuando hay un flanco de bajada (pulsacion) en ese momento el channel 1 captura el valor actual del timer.
		//ICValue = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);	//falling direct

	}

	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {//viene por un rising edge (cuando suelto el boton)

		//el canal 2 está configurado para capturar el valor del contador en el momento en que se produce un evento
		//de captura en el canal 1. En otras palabras, cuando el canal 1 detecta el flanco de bajada y captura el valor del contador,
		//este valor se utiliza como una referencia para el canal 2. Luego, cuando se produce un evento de captura en el canal 2 (en este caso,
		//un flanco de subida), se captura el valor actual del contador y se resta del valor de referencia previamente capturado del
		//canal 1 para obtener la duración del pulso.

		//ancho_pulso = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_2); //rising indirect

		if (HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_2) >= 3000) {
			ancho_pulso = 0;
			ICValue = 0;
			modo = INICIO;
			act_flag = true;
			alarma = 0;
			alarma_final = 250.0;
			flag_prim_config = false;

		}

	}
	contReb = 2000;
}

//El timer 2 (sin canal asociado) cuenta de a ms (clock de 8MHz con prescaler en 8000 => 0,001 s) y tiene un periodo de 500 => interrupcion cada 500ms
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if (htim == &htim1) {
		//OK
		if (boton == GPIO_PIN_5) {
			switch (modo) {
			case INICIO:
				modo = VER_TEMP;
				break;
			case VER_TEMP:
				modo = VER_PRES;
				break;
			case VER_PRES:
				modo = INICIO;
				act_flag = true;
				break;
			case CONFIG_TEMP:
				modo = INICIO_ALARM;
				alarma_final = alarma;
				flag_prim_config = true;
				break;
			case INICIO_ALARM:
				modo = VER_TEMP_ALARM;
				break;
			case VER_TEMP_ALARM:
				modo = VER_PRES_ALARM;
				break;
			case VER_PRES_ALARM:
				modo = INICIO_ALARM;
				break;
			default:
				break;
			}

			act_flag = 1;
		}
		//SUBIR
		else if (boton == GPIO_PIN_2) {
			if (modo == CONFIG_TEMP) {
				alarma = alarma + 0.1;
				if (alarma > 10.0) {
					alarma = 10.0;
				}
			}
			act_flag = 1;
		}
		//BAJAR
		else if (boton == GPIO_PIN_1) {
			if (modo == CONFIG_TEMP) {
				alarma = alarma - 0.1;
				if (alarma < 0) {
					alarma = 0;
				}
			}
			act_flag = true;
		}
		//CONFIG
		else if (boton == GPIO_PIN_3) {
			modo = CONFIG_TEMP;
			act_flag = true;
		}

		boton = 0;
		flag_timer1 = true;	//vuelvo a habilitar el timer para cuando se vuelva a presionar un boton
		HAL_TIM_Base_Stop(&htim1);

	}

	if (htim == &htim2) {

		flag_medicion = true; //El timer me hace tomar mediciones cada cierto tiempo

		if (flag_alarma == true) {			//Toglea el led cada 0.5 segundos.
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		} else if (flag_alarma == false
				&& HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == 0) {	//Para que apague el led una sola vez y no tenga que estar entrando todo el tiempo.
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		}

	}
}

/* === End of documentation ==================================================================== */

