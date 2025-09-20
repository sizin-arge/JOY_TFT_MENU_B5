#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>                           // TR: uint8_t/uint16_t tipleri

/* TR: Sürüm ve basit CRC ile küçük, sağlam bir yapı tutuyoruz */
typedef struct {
    uint16_t version;                         // TR: yapının sürümü (şimdilik 0x0001)
    uint8_t  lang_idx;                        // TR: dil indeksi (i18n için)
    uint8_t  theme_idx;                       // TR: tema indeksi (ui_theme için)
    uint16_t crc;                             // TR: basit CRC16 (alt alanların üstüne)
} SettingsRecord;                             // TR: kalıcı ayar kaydı

/* TR: Dış API – basit ve tek noktadan kullanım */
void              SETTINGS_Init(void);        // TR: RAM’i doldurur, varsa NVM’den okur
const SettingsRecord* SETTINGS_Get(void);     // TR: aktif ayarı oku (salt-okunur)
void              SETTINGS_SetLang(uint8_t i);// TR: dili ata (RAM’de)
void              SETTINGS_SetTheme(uint8_t i);// TR: temayı ata (RAM’de)
int               SETTINGS_Save(void);        // TR: NVM’ye yaz (0=ok, <0=hata)

/* TR: Küçük yardımcılar – index aralığı kontrolü üst katmanda yapılabilir */
static inline uint8_t SETTINGS_GetLang(void)  { return SETTINGS_Get()->lang_idx; }   // TR
static inline uint8_t SETTINGS_GetTheme(void) { return SETTINGS_Get()->theme_idx; }  // TR

#endif /* SETTINGS_H */
