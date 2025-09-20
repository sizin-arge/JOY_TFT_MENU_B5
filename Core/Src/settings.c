#include "settings.h"                         // TR: arayüz
#include <string.h>                           // TR: memset/memcpy
#include "i18n.h"                             // TR: dil modülü (uygularız)
#include "ui_theme.h"                         // TR: tema modülü (uygularız)

/* -----------------------------------------------------------
   TR: KALICILIK ARKA UCU (backend) SEÇİMİ
   -----------------------------------------------------------
   - İlk aşamada RAM fallback (geçici) çalışır, hemen derlenir.
   - Gerçek kalıcılık için config_pins.h içine şunları eklemen yeterli:

     // ---- config_pins.h (örnek) ----
     #define SETTINGS_USE_FLASH        1      // TR: 1=Flash’a yaz, 0=RAM’de tut
     #define SETTINGS_FLASH_PAGE_ADDR  0x0801F800u // TR: AYIRDIĞIN BOŞ SAYFA ADRESİ
     // --------------------------------

   NOT: Adresi mutlaka projenin flash haritasına göre seç. Ayrı ve boş bir sayfa ayır.
   STM32F3’te sayfa silme/yarı-söz (halfword) yazma kullanıyoruz.
   ----------------------------------------------------------- */

#ifndef SETTINGS_USE_FLASH
#define SETTINGS_USE_FLASH        0           // TR: varsayılan RAM (geçici)
#endif

#ifndef SETTINGS_FLASH_PAGE_ADDR
#define SETTINGS_FLASH_PAGE_ADDR  0u          // TR: adres tanımsız -> RAM’e düş
#endif

/* TR: dahili aktif kopya (RAM) */
static SettingsRecord s_cur;                  // TR: çalışan ayarlar (RAM)

/* TR: sürüm sabiti – ileride yapı genişlerse arttır */
#define SETTINGS_VERSION          0x0001u     // TR: yapı sürümü

/* -------------------- küçük CRC16 -------------------- */
static uint16_t crc16_ccitt(const uint8_t* data, uint32_t len)  // TR: basit CRC16-CCITT
{
    uint16_t crc = 0xFFFF;
    for (uint32_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t b = 0; b < 8; b++) {
            if (crc & 0x8000) crc = (crc << 1) ^ 0x1021;
            else              crc = (crc << 1);
        }
    }
    return crc;
}

/* TR: RAM’e fabrika varsayılanını yaz */
static void load_factory_defaults(void)       // TR: ilk değerler
{
    memset(&s_cur, 0, sizeof(s_cur));         // TR: sıfırla
    s_cur.version   = SETTINGS_VERSION;       // TR: sürüm
    s_cur.lang_idx  = 0;                      // TR: 0=ilk dil (örn: Türkçe)
    s_cur.theme_idx = 0;                      // TR: 0=ilk tema
    s_cur.crc       = 0;                      // TR: CRC hesapla (aşağıda)
    s_cur.crc = crc16_ccitt((uint8_t*)&s_cur, sizeof(s_cur)-2); // TR: crc alanı hariç
}

/* -------------------- FLASH arka ucu (opsiyonel) -------------------- */
#if SETTINGS_USE_FLASH && (SETTINGS_FLASH_PAGE_ADDR != 0)
#include "stm32f3xx_hal.h"                    // TR: HAL Flash

/* TR: kayıt tek sayfaya sığacak kadar küçük. Tek kopya yazıyoruz. */
static int flash_read(SettingsRecord* out)    // TR: Flash’tan oku -> out
{
    const SettingsRecord* rom = (const SettingsRecord*)SETTINGS_FLASH_PAGE_ADDR; // TR: bellek eşlemeli okuma
    *out = *rom;                              // TR: ham kopya
    /* TR: temel doğrula */
    if (out->version != SETTINGS_VERSION) return -1;           // TR: sürüm farklı
    uint16_t expect = crc16_ccitt((uint8_t*)out, sizeof(*out)-2);// TR: crc hesap
    if (expect != out->crc) return -2;                         // TR: crc uyumsuz
    return 0;                                                  // TR: ok
}

static int flash_erase_page(uint32_t addr)    // TR: tek sayfa sil
{
    HAL_FLASH_Unlock();                       // TR: Flash kilidini aç
    FLASH_EraseInitTypeDef ei = {0};          // TR: silme yapılandır
    uint32_t page_error = 0;                  // TR: hata kodu
    ei.TypeErase   = FLASH_TYPEERASE_PAGES;   // TR: sayfa sil
    ei.PageAddress = addr;                    // TR: hedef sayfa adresi
    ei.NbPages     = 1;                       // TR: 1 sayfa
    if (HAL_FLASHEx_Erase(&ei, &page_error) != HAL_OK) {
        HAL_FLASH_Lock();                     // TR: kilitle
        return -1;                            // TR: silme başarısız
    }
    HAL_FLASH_Lock();                         // TR: kilitle
    return 0;                                 // TR: ok
}

static int flash_write_record(const SettingsRecord* in) // TR: kaydı Flash’a yaz
{
    /* TR: CRC güncelle */
    SettingsRecord tmp = *in;                 // TR: kopya al
    tmp.crc = crc16_ccitt((uint8_t*)&tmp, sizeof(tmp)-2); // TR: crc hesap

    /* TR: sayfayı sil -> sonra yarı-söz (16-bit) yazarak doldur */
    if (flash_erase_page(SETTINGS_FLASH_PAGE_ADDR) != 0) return -1;

    HAL_FLASH_Unlock();                       // TR: kilidi aç
    const uint16_t* p = (const uint16_t*)&tmp;// TR: 16-bit yazım
    uint32_t addr = SETTINGS_FLASH_PAGE_ADDR; // TR: başlangıç adresi
    for (uint32_t i = 0; i < sizeof(SettingsRecord)/2; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, p[i]) != HAL_OK) {
            HAL_FLASH_Lock();                 // TR: kilitle
            return -2;                        // TR: yazma hatası
        }
        addr += 2;                            // TR: sonraki yarı-söz
    }
    HAL_FLASH_Lock();                         // TR: kilitle
    return 0;                                 // TR: ok
}
#endif /* SETTINGS_USE_FLASH && addr */

/* -------------------- Genel API -------------------- */
void SETTINGS_Init(void)                      // TR: RAM’i doldur; varsa kalıcıdan yükle
{
    load_factory_defaults();                  // TR: fabrika değerlerini RAM’e koy

#if SETTINGS_USE_FLASH && (SETTINGS_FLASH_PAGE_ADDR != 0)
    SettingsRecord rom;
    if (flash_read(&rom) == 0) {              // TR: Flash OK ise
        s_cur = rom;                          // TR: RAM’e kopyala
    }                                         // TR: aksi halde fabrika değerleri kalır
#endif

    /* TR: Uygulamaya uygulat (tema+dil) – burada bağlamak işleri kolaylaştırır */
    UITheme_SetByIndex(s_cur.theme_idx);      // TR: tema uygula
    I18N_SetLangByIndex(s_cur.lang_idx);      // TR: dil uygula
}

const SettingsRecord* SETTINGS_Get(void)      // TR: salt-okunur RAM kopyası
{
    return &s_cur;                            // TR: aktif ayarlar
}

void SETTINGS_SetLang(uint8_t i)              // TR: dili değiştir (RAM)
{
    s_cur.lang_idx = i;                       // TR: RAM’de ata
    I18N_SetLangByIndex(i);                   // TR: anında uygula (ekran metinleri)
}

void SETTINGS_SetTheme(uint8_t i)             // TR: temayı değiştir (RAM)
{
    s_cur.theme_idx = i;                      // TR: RAM’de ata
    UITheme_SetByIndex(i);                    // TR: anında uygula (renkler)
}

int SETTINGS_Save(void)                       // TR: RAM→kalıcı yaz (0=ok)
{
    /* TR: CRC’yi güncelle ve uygun arka uca gönder */
    SettingsRecord tmp = s_cur;               // TR: geçici
    tmp.version = SETTINGS_VERSION;           // TR: sürümü garanti et
    tmp.crc = crc16_ccitt((uint8_t*)&tmp, sizeof(tmp)-2); // TR: crc hesap

#if SETTINGS_USE_FLASH && (SETTINGS_FLASH_PAGE_ADDR != 0)
    return flash_write_record(&tmp);          // TR: Flash’a yaz
#else
    (void)tmp;                                // TR: kullanılmadı uyarısını sustur
    return 0;                                 // TR: RAM modunda “başarılı” kabul
#endif
}
