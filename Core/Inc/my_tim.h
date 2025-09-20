#ifndef MY_TIM_H
#define MY_TIM_H

#include "stm32f3xx_hal.h"   // HAL tipleri
#include "config_pins.h"     // TICK_TIM_HANDLE alias'ı

/************ Genel yapılandırma ************/

#ifndef MYTIM_MAX_HOOKS
#define MYTIM_MAX_HOOKS   4      // TR: 1 ms’de çağrılacak kanca (callback) adedi
#endif

/********************************************/

/* TR: 1 ms kanca imzası (parametresiz) */
typedef void (*MYTIM_Hook)(void);

/************ API ************/

/* TR: Timer handle ata (NULL ise config’teki TICK_TIM_HANDLE kullanılır) */
void     MYTIM_Init(TIM_HandleTypeDef *htim);

/* TR: 1 ms kesmeli sayacı başlat/durdur */
HAL_StatusTypeDef MYTIM_Start(void);
HAL_StatusTypeDef MYTIM_Stop(void);

/* TR: Geçen milisaniye (uint32, ~49 gün sonra taşar) */
uint32_t MYTIM_Millis(void);

/* TR: 1 ms’de bir çağrılacak fonksiyonu ekle — başarı: indeks (0..), dolu: -1 */
int      MYTIM_AddHook(MYTIM_Hook fn);

/* TR: Daha önce eklenen kancayı indeks ile sil */
void     MYTIM_RemoveHook(int index);

/* TR: Tüm kancaları temizle */
void     MYTIM_ClearHooks(void);

/* TR: HAL callback’ten çağrılacak ISR (eşleşen timer için 1 ms arttırır + kancaları çalıştırır) */
void     MYTIM_ISR(TIM_HandleTypeDef *htim);

/********************************************/

#endif /* MY_TIM_H */
