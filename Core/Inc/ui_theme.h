#ifndef UI_THEME_H
#define UI_THEME_H

#include <stdint.h>                        // TR: uint16_t vb. tipler

/* TR: Her temada kullanacağımız tüm renk alanları (RGB565) */
typedef struct {
    uint16_t yazi;        // TR: Yazı rengi (foreground)
    uint16_t arkaplan;    // TR: Arka plan rengi
    uint16_t yazi_alt;    // TR: Yazının arka planı (satır dolgusu)
    uint16_t cizgi;       // TR: Ayraç/çizgi rengi
    uint16_t ornek1;      // TR: Örnek renk 1 (serbest kullanım)
    uint16_t ornek2;      // TR: Örnek renk 2 (serbest kullanım)
    uint16_t ornek3;      // TR: Örnek renk 3 (serbest kullanım)
} UIThemePalette;         // TR: Tema paleti yapısı

/* TR: Modül ömrü boyunca geçerli tema ile çalışır */
void UITheme_Init(void);                              // TR: Varsayılan temayı yükle
void UITheme_SetByIndex(uint16_t idx);               // TR: Index ile tema seç
uint16_t UITheme_Count(void);                        // TR: Kayıtlı tema sayısı
const char* UITheme_NameAt(uint16_t idx);            // TR: idx’teki temanın görünen adı
const UIThemePalette* UITheme_Get(void);             // TR: Aktif temanın paletini getir

/* TR: Kolay erişim için satır içi kısa yol fonksiyonları */
static inline uint16_t UITheme_Yazi(void)     { return UITheme_Get()->yazi; }       // TR
static inline uint16_t UITheme_Arka(void)     { return UITheme_Get()->arkaplan; }   // TR
static inline uint16_t UITheme_YaziAlt(void)  { return UITheme_Get()->yazi_alt; }   // TR
static inline uint16_t UITheme_Cizgi(void)    { return UITheme_Get()->cizgi; }      // TR
static inline uint16_t UITheme_Ornek1(void)   { return UITheme_Get()->ornek1; }     // TR
static inline uint16_t UITheme_Ornek2(void)   { return UITheme_Get()->ornek2; }     // TR
static inline uint16_t UITheme_Ornek3(void)   { return UITheme_Get()->ornek3; }     // TR

#endif /* UI_THEME_H */
