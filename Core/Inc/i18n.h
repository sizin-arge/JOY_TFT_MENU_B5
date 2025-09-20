#ifndef I18N_H
#define I18N_H

#include <stdint.h>

/* ------------------------------------------------------------
 *  Dil kimlikleri
 *  (Yeni dil eklemek için: I18N_Lang’a yenisini ekle, i18n.c’de
 *   LANG_COUNT ve tablolara karşılıklarını ekle.)
 * ----------------------------------------------------------*/
typedef enum {
    I18N_TR = 0,    // Türkçe
    I18N_EN,        // English
    I18N_DE,        // Deutsch
    I18N_LANG_COUNT
} I18N_Lang;

/* ------------------------------------------------------------
 *  Metin anahtarları
 *  (Yeni metin eklemek için: aşağıya yeni anahtar ekle,
 *   i18n.c’de TR/EN/DE tablolarına karşılığını koy.)
 * ----------------------------------------------------------*/
typedef enum {
    /* --- Ana menü başlıkları --- */
    I18N_MENU_QUICK_START = 0,
    I18N_MENU_SETTINGS,
    I18N_MENU_JOYSTICK_TEST,
    I18N_MENU_COLORS,
    I18N_MENU_ABOUT,

    /* --- Ayarlar alt menüsü --- */
    I18N_SETTINGS_LANG,
    I18N_SETTINGS_COLOR,
    I18N_SETTINGS_TIME,

    /* --- Mesajlar / Etiketler --- */
    I18N_MSG_SAVED,
    I18N_MSG_APPLIED,
    I18N_MSG_BACK,
    I18N_MSG_OK,

    /* --- Ekran sabitleri / başlık --- */
    I18N_TITLE_BRAND,     // Örn: “SizinArGe”

    /* Her zaman en sonda kalsın: toplam anahtar sayısı */
    I18N_KEY_COUNT
} I18N_Key;

/* ------------------------------------------------------------
 *  Genel API
 * ----------------------------------------------------------*/
void        I18N_Init(void);                  // Varsayılan dili kur (TR)
void        I18N_SetLang(I18N_Lang lang);     // Dili ayarla
I18N_Lang   I18N_CurrentLang(void);           // Mevcut dili oku

/* Menü gibi yerlerde index ile çalışmak istersen */
void        I18N_SetLangByIndex(uint8_t idx); // 0..I18N_LangCount()-1
uint8_t     I18N_CurrentLangIndex(void);

/* Metin erişimi */
const char* I18N_Get(I18N_Key key);           // Çeviriyi getir (fallback EN)

/* Dil listesi (UI için) */
uint8_t     I18N_LangCount(void);             // Toplam dil sayısı
const char* I18N_LangNameAt(uint8_t idx);     // “Türkçe”, “English”, “Deutsch”

#endif /* I18N_H */
