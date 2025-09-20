
#ifndef MY_JOYSTICK_H
#define MY_JOYSTICK_H

#include <stdint.h>
#include "stm32f3xx_hal.h"
#include "config_pins.h"
#include "my_adc.h"

/* ---------------------------
 *  KULLANIM ÖZETİ
 *  - JOY_ReadUpDown():  Y ekseni; -1 (yukarı), 0 (yok), +1 (aşağı)
 *    * Otomatik tekrar: JOY_STEP_FIRST_MS / JOY_STEP_REPEAT_MS
 *  - JOY_ReadLeftRight(): X ekseni; edge üretir
 *    * -1: sol edge, +1: sağ edge, 0: yok (debounce: JOY_DEBOUNCE_MS)
 *  - JOY_ResetRepeat():   Y ekseni tekrar zamanlayıcısını sıfırlar
 *  - JOY_ResetLeftRight():X ekseni edge dedektörünü sıfırlar
 * --------------------------*/

/* -1: up/left, 0: none, +1: down/right */
int  JOY_ReadUpDown(void);           /* Y ekseni (tekrar destekli) */
int  JOY_ReadLeftRight(void);        /* X ekseni (edge: -1/0/+1)  */
void JOY_ResetRepeat(void);          /* Y ekseni tekrar reset     */
void JOY_ResetLeftRight(void);       /* X ekseni edge reset       */

/* (İsteğe bağlı) Joystick buton kenarı: 1=press, -1=release, 0=yok */
int  JOY_ButtonEdge(void);

/* Sağ/Sol "edge" API bayrakları ve kısa yollar */
#define JOY_HAVE_RIGHT 1
#define JOY_HAVE_LEFT  1
static inline int JOY_ReadRight(void){ return JOY_ReadLeftRight() > 0; }
static inline int JOY_ReadLeft(void) { return JOY_ReadLeftRight() < 0; }

/* --------- DEBUG GÖZLEM DEĞİŞKENLERİ (opsiyonel) --------- */
extern volatile uint16_t JOY_dbg_x_raw;
extern volatile uint16_t JOY_dbg_y_raw;
extern volatile int8_t   JOY_dbg_ud_dir;       /* Y anlık yön  (-1/0/+1)   */
extern volatile int8_t   JOY_dbg_lr_dir;       /* X anlık yön  (-1/0/+1)   */
extern volatile int8_t   JOY_dbg_lr_edge;      /* X edge       (-1/0/+1)   */
extern volatile uint32_t JOY_dbg_last_edge_ms; /* X son edge zamanı (ms)    */

#endif /* MY_JOYSTICK_H */
