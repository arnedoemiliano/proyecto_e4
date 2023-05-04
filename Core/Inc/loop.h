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

#ifndef LOOP_H
#define LOOP_H

/* === Headers files inclusions ================================================================ */
#include "stm32f1xx_hal.h"
#include "stdbool.h"
/* === Cabecera C++ ============================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

#define INICIO 0
#define VER_TEMP 1
#define VER_PRES 2
#define CONFIG_TEMP 3
#define INICIO_ALARM 4
#define VER_TEMP_ALARM 5
#define VER_PRES_ALARM 6

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

extern uint8_t modo;	//variable que necesita loop.c e interrupts.c
extern float alarma_final;
extern float alarma;
extern bool flag_prim_config; //Me indica que ya hubo una primera configuracion de alarma. Puedo hacer back de CONFIG_TEMP a INICIO_ALARM
extern bool flag_alarma;
extern bool flag_medicion; //Bandera que activa el timer, para que el main se comunique por i2c con el sensor y tome temp y pres.
extern bool act_flag;
extern uint32_t ICValue;
extern uint32_t ancho_pulso;


/* === Public function declarations ============================================================ */

void loop(void);
void actualizarPantalla(void);
void actualizarValores(void);
void comprobarAlarma(void);
void medirTempPres(void);

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* LOOP_H */
