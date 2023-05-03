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
#include "stm32f1xx_hal.h"
#include "bmp.h"
#include "i2c-lcd.h"
#include "main.h"
#include "string.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */
float temp;
float press;
char str_temp[17]; //string que guarda el valor convertido a float de temp, pres y alarma.
/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */
uint8_t modo=INICIO;
float alarma_final=250.0;
float alarma=0;





extern uint8_t flag_alarma;
extern uint8_t flag_medicion;
extern uint8_t flag_clear;
extern uint32_t ICValue;
extern uint32_t ancho_pulso;
char str_actual[17] = "Sin alarma";
extern uint8_t act_flag;	//inicializada en 1
/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */
void medirTempPres(void) {
	if (flag_medicion == 1) {

		temp = getTemperature();
		press = getPressure(1);

		flag_medicion = 0;

	}

}

/*	Solamente modifica las variables que mostrará verPantalla	*/
void actualizarValores(void) {

	switch (modo) {
	case (INICIO):

		//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,GPIO_PIN_RESET);
		break;
	case VER_TEMP:
	case VER_TEMP_ALARM:
		floatToString(temp, str_temp);//Ahora la string str_temp contiene el valor float de temp.
		break;
	case VER_PRES:
	case VER_PRES_ALARM:
		floatToString(press, str_temp);
		break;
	case CONFIG_TEMP:
		floatToString(alarma, str_temp);
		break;
	case INICIO_ALARM:
		floatToString(alarma_final, str_temp);
		break;
	default:
		break;
	}

}

/*	Debería tener un forma de que no se actualice hasta que no haya cambios en lo que hay que mostrar	*/
void actualizarPantalla(void) {
	switch (modo) {
	case INICIO:
		if (act_flag == 1) {
			lcdClear();
			lcdCursor(0, 0);
			lcdSendString("Sin alarma");
			act_flag = 0;
		}
		break;
	case VER_TEMP_ALARM:
	case VER_TEMP:
		if (strcmp(str_actual, str_temp)!=0) {
			lcdClear();
			lcdCursor(0, 0);
			displayTemp(str_temp);//displayTemp toma el string que contiene el valor de temperatura, le agrega los labels y lo manda por pantalla.
			strncpy(str_actual, str_temp,17);
		}

		break;
	case VER_PRES_ALARM:
	case VER_PRES:
		if (strcmp(str_actual, str_temp)!=0) {
			lcdClear();
			lcdCursor(0, 0);
			displayPressure(str_temp);
			strncpy(str_actual, str_temp,17);
		}
		break;
	case CONFIG_TEMP:
		if (act_flag == 1) {
			lcdClear();
			lcdCursor(0, 0);
			displayAlarm(str_temp);
			act_flag = 0;
		}
		break;
	case INICIO_ALARM:
		if (act_flag == 1) {
			lcdClear();
			lcdCursor(0, 0);
			displayInicioAlarm(str_temp);
			act_flag = 0;
		}
		break;
	default:
		break;

	}

}

void comprobarAlarma(void) {

	//Si se tiene que activar la alarma, se activa una bandera para que el timer toglee el led. Asi no uso HAL_delay en el programa principal
	if (alarma_final < temp) {
		flag_alarma = 1;
	} else {
		flag_alarma = 0;
	}

}

void comprobarPulsacionLarga(void) {

	if (ancho_pulso >= 3000) {
		ancho_pulso = 0;
		ICValue = 0;
		modo = INICIO;
		act_flag = 1;
		alarma = 0;
		alarma_final = 250.0;

	}

}

void loop(void) {

	medirTempPres();
	actualizarValores();
	actualizarPantalla();
	comprobarAlarma();
	comprobarPulsacionLarga();

}

/* === End of documentation ==================================================================== */

