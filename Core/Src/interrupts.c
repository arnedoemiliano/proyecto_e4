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

#include "interrupts.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */
I2C_HandleTypeDef hi2c1;
/* === Private function declarations =========================================================== */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	while (contReb > 1) {
		contReb--;
	}

	/*   	Rutina boton OK     */
	if (GPIO_Pin == GPIO_PIN_5) {
		switch (modo) {
		case INICIO:
			modo = VER_TEMP;
			break;
		case VER_TEMP:
			modo = VER_PRES;
			break;
		case VER_PRES:
			modo = INICIO;
			act_flag = 1;
			break;
		case CONFIG_TEMP:
			modo = INICIO_ALARM;
			alarma_final = alarma;
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

		flag_clear = 1;
	}

	//   	Rutina boton SUBIR     		//Solo sirve para modificar la variable alarma.
	if (GPIO_Pin == GPIO_PIN_2) {
		if (modo == CONFIG_TEMP) {
			alarma = alarma + 0.1;
			if (alarma > 10.0) {
				alarma = 10.0;
			}
		}
		flag_clear = 1;
	}

	//Rutina boton BAJAR
	if (GPIO_Pin == GPIO_PIN_1) {
		if (modo == CONFIG_TEMP) {
			alarma = alarma - 0.1;
			if (alarma < 0) {
				alarma = 0;
			}
		}
		flag_clear = 1;
	}

	// Rutina boton CONFIG
	if (GPIO_Pin == GPIO_PIN_3) {
		modo = CONFIG_TEMP;
		flag_clear = 1;
	}
	contReb = 2000;
}

/* === Public function implementation ========================================================== */
/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
void MX_I2C1_Init(void) {

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}
/* === End of documentation ==================================================================== */

