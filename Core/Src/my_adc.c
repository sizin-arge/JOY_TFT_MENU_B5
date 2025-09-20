#include "my_adc.h"
#include "config_pins.h"

#ifndef MYADC_ASSUME_HANDLE
#define MYADC_ASSUME_HANDLE   1     // TR: 1 ise, MYADC_Init çağrılmasa da JOY_ADC_HANDLE otomatik bağlanır
#endif

#ifndef JOY_ADC_HANDLE
extern ADC_HandleTypeDef hadc1;
#define JOY_ADC_HANDLE (&hadc1)
#endif

static ADC_HandleTypeDef *s_hadc = NULL;  // TR: Aktif ADC handle burada tutulur

// TR: İç kullanım — güvenli şekilde handle al (gerekirse varsayılanı bağla)
static inline ADC_HandleTypeDef* prv_ensure_handle(void)
{
#if MYADC_ASSUME_HANDLE
    if (!s_hadc) s_hadc = JOY_ADC_HANDLE;   // TR: config_pins.h’deki varsayılan (örn: &hadc1)
#endif
    return s_hadc;                           // TR: NULL ise init eksik demektir
}

// TR: İç kullanım — tek seferlik bloklayıcı okuma (kanalı seç, başlat, bekle, değeri al)
static uint16_t prv_read_once(uint32_t channel)
{
    ADC_HandleTypeDef *hadc = prv_ensure_handle();    // TR: aktif handle
    if (!hadc) return 0;                               // TR: handle yoksa 0 döndür

    ADC_ChannelConfTypeDef sConfig = {0};              // TR: kanal konfigürasyonu
    sConfig.Channel      = channel;                    // TR: istenen kanal
    sConfig.Rank         = ADC_REGULAR_RANK_1;         // TR: tek-sıra okuma
    sConfig.SamplingTime = MYADC_SAMPLING_TIME;        // TR: örnekleme (config_pins.h belirler)

    if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)   // TR: kanalı yapılandır
        return 0;                                          // TR: hata -> 0

    if (HAL_ADC_Start(hadc) != HAL_OK)                     // TR: ADC başlat
        return 0;                                          // TR: hata -> 0

    if (HAL_ADC_PollForConversion(hadc, MYADC_POLL_TIMEOUT_MS) != HAL_OK) {
        HAL_ADC_Stop(hadc);                                 // TR: zaman aşımı -> durdur
        return 0;                                           // TR: 0 döndür
    }

    uint32_t v = HAL_ADC_GetValue(hadc);                   // TR: 12-bit ham değer
    HAL_ADC_Stop(hadc);                                    // TR: tek okuma bitti

    return (uint16_t)(v & 0x0FFF);                         // TR: 0..4095’e maskele
}

void MYADC_Init(ADC_HandleTypeDef *hadc)                   // TR: Dışarıdan handle bağla
{
    s_hadc = hadc;
}

uint16_t MYADC_ReadChannel(uint32_t channel)               // TR: Tek okuma
{
    return prv_read_once(channel);
}

uint16_t MYADC_ReadAverage(uint32_t channel, uint8_t samples) // TR: N örnek ortalaması
{
    if (samples == 0) samples = 1;                         // TR: 0 gelirse 1’e sabitle
    uint32_t acc = 0;
    for (uint8_t i = 0; i < samples; i++) {
        acc += prv_read_once(channel);
    }
    return (uint16_t)(acc / samples);
}
