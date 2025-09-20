#ifndef CONFIG_PINS_H
#define CONFIG_PINS_H

#include "stm32f3xx_hal.h"
#include "main.h"

/* =======================  ADC GENEL  ======================= */
#define MYADC_SAMPLING_TIME    ADC_SAMPLETIME_41CYCLES_5
#define MYADC_POLL_TIMEOUT_MS  10U
#define JOY_ADC_MAX            4095U

/* Harici voltaj kanalları (ihtiyaç olursa) */
#define VOLTAJ_1_CHANNEL       ADC_CHANNEL_1   /* PA0 */
#define VOLTAJ_2_CHANNEL       ADC_CHANNEL_2   /* PA1 */
#define VOLTAJ_3_CHANNEL       ADC_CHANNEL_3   /* PA2 */
#define VOLTAJ_4_CHANNEL       ADC_CHANNEL_4   /* PA3 */
#define VOLTAJ_5_CHANNEL       ADC_CHANNEL_5   /* PA4 */

/* =======================  JOYSTICK  ======================== */
/* Eksen kanalları (bağlantına göre değiştir) */
#define JOY_X_CHANNEL          ADC_CHANNEL_11  /* ör: PA6 – sağ/sol */
#define JOY_Y_CHANNEL          ADC_CHANNEL_10  /* ör: PA5 – yukarı/aşağı */

/* Eşikler – X ve Y ayrı */
#define JOY_X_LOW_THR          1500U           /* sol bölge eşiği  */
#define JOY_X_HIGH_THR         2600U           /* sağ bölge eşiği  */
#define JOY_Y_LOW_THR          1500U           /* yukarı eşiği     */
#define JOY_Y_HIGH_THR         2600U           /* aşağı eşiği      */

/* Yön tersleme (donanımına göre 0/1 yap) */
#define JOY_X_INVERT           1               /* 1: sol/sağ ters  */
#define JOY_Y_INVERT           1               /* 1: yukarı/aşağı ters */

/* Tekrar ve debounce (ms) */
#define JOY_STEP_FIRST_MS      500U
#define JOY_STEP_REPEAT_MS     300U
#define JOY_DEBOUNCE_MS         40U

/* Sağ/sol kenar okumasını kullanacağız */
#define JOY_HAVE_RIGHT         1
#define JOY_HAVE_LEFT          1

/* (İstersen) buton pini */
#define JOY_BTN_GPIO_Port      GPIOB
#define JOY_BTN_Pin            GPIO_PIN_5
#define JOY_BTN_ACTIVE_LOW     1               /* 1: basınca 0 okunur */

/* ========================  TFT/ST7789  ===================== */
#define PANEL_WIDTH            240u
#define PANEL_HEIGHT           320u

/* CubeMX pin alias’ları (main.h’daki isimler) */
#define TFT_CS_PORT            TFT_CS_GPIO_Port
#define TFT_CS_PIN             TFT_CS_Pin
#define TFT_DC_PORT            TFT_DC_GPIO_Port
#define TFT_DC_PIN             TFT_DC_Pin
#define TFT_RST_PORT           TFT_RST_GPIO_Port
#define TFT_RST_PIN            TFT_RST_Pin
/* =========================================================== */
#endif /* CONFIG_PINS_H */
