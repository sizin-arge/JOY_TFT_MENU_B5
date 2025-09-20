#ifndef MY_ADC_H
#define MY_ADC_H

#include "stm32f3xx_hal.h"      // HAL tipleri // TR
#include "config_pins.h"        // ADC handle ve kanal alias'ları // TR

// TR: Modülü bir ADC handle ile başlat (örn: &hadc1). Başlatılmazsa varsayılanı (config_pins.h'deki JOY_ADC_HANDLE) otomatik kullanır.
void     MYADC_Init(ADC_HandleTypeDef *hadc);   // TR: ADC handle ata

// TR: Tek kanalı bloklayıcı şekilde oku; 12-bit ham değer (0..4095) döner.
uint16_t MYADC_ReadChannel(uint32_t channel);   // TR: Kanal oku (tek sefer)

// TR: Aynı kanaldan art arda N örnek alır, tam sayı ortalamasını döner.
uint16_t MYADC_ReadAverage(uint32_t channel, uint8_t samples);  // TR: Ortalama okuma

// TR: Kısa yol fonksiyonları (config_pins.h’teki alias’lara dayanır)
static inline uint16_t MYADC_ReadVoltaj1(void){ return MYADC_ReadChannel(VOLTAJ_1_CHANNEL); } // TR
static inline uint16_t MYADC_ReadVoltaj2(void){ return MYADC_ReadChannel(VOLTAJ_2_CHANNEL); } // TR
static inline uint16_t MYADC_ReadVoltaj3(void){ return MYADC_ReadChannel(VOLTAJ_3_CHANNEL); } // TR
static inline uint16_t MYADC_ReadVoltaj4(void){ return MYADC_ReadChannel(VOLTAJ_4_CHANNEL); } // TR
static inline uint16_t MYADC_ReadVoltaj5(void){ return MYADC_ReadChannel(VOLTAJ_5_CHANNEL); } // TR

#endif /* MY_ADC_H */
