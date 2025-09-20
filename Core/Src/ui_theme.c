#include "ui_theme.h"                     // TR: Tema arayüzü
#include "tft_driver.h"                   // TR: Renk sabitleri (RGB565)

/* TR: Tema adları (menüde görünecek metinler) */
static const char* s_theme_names[] = {
    "Tema 1",                             // TR: 0. tema adı
    "Tema 2",                             // TR: 1. tema adı
    "Tema 3",                             // TR: 2. tema adı
    "Tema 4",                             // TR: 3. tema adı
    "Tema 5",                             // TR: 4. tema adı
};                                        // TR: Yeni tema eklersen buraya da adını ekle

/* TR: Tema paletleri – RGB565 renk sabitleri tft_driver.h’den gelir */
static const UIThemePalette s_themes[] = {
    /* TR: Tema 1 – Açık zemin */
    {
        .yazi      = TFT_COLOR_RED,     // TR: Yazı
        .arkaplan  = TFT_COLOR_CYAN,     // TR: Arka plan
        .yazi_alt  = TFT_COLOR_RED,     // TR: Satır dolgusu
        .cizgi     = TFT_COLOR_BLACK,     // TR: Ayraç/çizgi
        .ornek1    = TFT_COLOR_RED,       // TR: Örnek1
        .ornek2    = TFT_COLOR_BLUE,      // TR: Örnek2
        .ornek3    = TFT_COLOR_GREEN,     // TR: Örnek3
    },
    /* TR: Tema 2 – Koyu zemin */
    {
        .yazi      = TFT_COLOR_WHITE,     // TR: Yazı
        .arkaplan  = TFT_COLOR_BLACK,     // TR: Arka plan
        .yazi_alt  = TFT_COLOR_BLACK,     // TR: Satır dolgusu
        .cizgi     = TFT_COLOR_WHITE,     // TR: Ayraç/çizgi
        .ornek1    = TFT_COLOR_CYAN,      // TR: Örnek1
        .ornek2    = TFT_COLOR_YELLOW,    // TR: Örnek2
        .ornek3    = TFT_COLOR_MAGENTA,   // TR: Örnek3
    },
    /* TR: Tema 3 – Sıcak ton */
    {
        .yazi      = TFT_COLOR_BLACK,     // TR: Yazı
        .arkaplan  = TFT_COLOR_YELLOW,    // TR: Arka plan
        .yazi_alt  = TFT_COLOR_RED,    // TR: Satır dolgusu
        .cizgi     = TFT_COLOR_BLACK,     // TR: Ayraç/çizgi
        .ornek1    = TFT_COLOR_ORANGE,    // TR: Örnek1 (tanımlıysa)
        .ornek2    = TFT_COLOR_RED,       // TR: Örnek2
        .ornek3    = TFT_COLOR_BLUE,     // TR: Örnek3 (tanımlıysa)
    },
    /* TR: Tema 4 – Soğuk ton */
    {
        .yazi      = TFT_COLOR_WHITE,     // TR: Yazı
        .arkaplan  = TFT_COLOR_NAVY,      // TR: Arka plan
        .yazi_alt  = TFT_COLOR_NAVY,      // TR: Satır dolgusu
        .cizgi     = TFT_COLOR_WHITE,     // TR: Ayraç/çizgi
        .ornek1    = TFT_COLOR_CYAN,      // TR: Örnek1
        .ornek2    = TFT_COLOR_BLUE,   // TR: Örnek2 (tanımlıysa)
        .ornek3    = TFT_COLOR_LIGHTGREY, // TR: Örnek3
    },
    /* TR: Tema 5 – Kontrast */
    {
        .yazi      = TFT_COLOR_BLACK,     // TR: Yazı
        .arkaplan  = TFT_COLOR_CYAN,      // TR: Arka plan
        .yazi_alt  = TFT_COLOR_BLACK,     // TR: Satır dolgusu
        .cizgi     = TFT_COLOR_BLACK,     // TR: Ayraç/çizgi
        .ornek1    = TFT_COLOR_GREEN,      // TR: Örnek1 (tanımlıysa)
        .ornek2    = TFT_COLOR_MAGENTA,   // TR: Örnek2
        .ornek3    = TFT_COLOR_WHITE,     // TR: Örnek3
    },
};                                        // TR: Yeni tema eklerken bu diziye bir eleman ekle

/* TR: Geçerli tema index’i (0..UITheme_Count-1) */
static uint16_t s_current = 0;            // TR: Varsayılan 0. tema

void UITheme_Init(void)
{
    s_current = 0;                         // TR: Çıkışta tema 0 ile başla (gerekirse değiştir)
}

void UITheme_SetByIndex(uint16_t idx)
{
    if (idx >= (sizeof(s_themes)/sizeof(s_themes[0]))) { // TR: Aralık kontrolü
        return;                                          // TR: Geçersiz index ise yapma
    }
    s_current = idx;                                     // TR: Yeni temayı ata
    /* TR: Kalıcı saklama eklenecekse (EEPROM/Flash) burada kaydedilebilir */
}

uint16_t UITheme_Count(void)
{
    return (uint16_t)(sizeof(s_themes)/sizeof(s_themes[0])); // TR: Toplam tema sayısı
}

const char* UITheme_NameAt(uint16_t idx)
{
    if (idx >= UITheme_Count()) return "";               // TR: Aralık dışı → boş string
    return s_theme_names[idx];                           // TR: Adı döndür
}

const UIThemePalette* UITheme_Get(void)
{
    return &s_themes[s_current];                         // TR: Aktif tema paleti
}
