/* Core/Src/i18n.c */
#include "i18n.h"
#include <stddef.h>

/* --- Dahili durum --- */
static I18N_Lang s_lang = I18N_TR;

/* --- Dil adları (UI listesi için) --- */
static const char* const kLangNames[I18N_LANG_COUNT] = {
    "Türkçe", "English", "Deutsch"
};

/* --- Türkçe çeviriler --- */
static const char* const TR[I18N_KEY_COUNT] = {
    /* Ana menü */
    [I18N_MENU_QUICK_START]   = "> HIZLI BAŞLAT",
    [I18N_MENU_SETTINGS]      = "> AYARLAR",
    [I18N_MENU_JOYSTICK_TEST] = "> JOYSTICK TEST",
    [I18N_MENU_COLORS]        = "> RENKLER",
    [I18N_MENU_ABOUT]         = "> HAKKINDA",

    /* Ayarlar alt menüsü */
    [I18N_SETTINGS_LANG]      = "> Dil",
    [I18N_SETTINGS_COLOR]     = "> Tema",
    [I18N_SETTINGS_TIME]      = "> Saat",

    /* Mesajlar / Etiketler */
    [I18N_MSG_SAVED]          = "Kaydedildi",
    [I18N_MSG_APPLIED]        = "Uygulandı",
    [I18N_MSG_BACK]           = "Geri",
    [I18N_MSG_OK]             = "Tamam",

    /* Başlık / marka */
    [I18N_TITLE_BRAND]        = "SizinArGe",
};

/* --- İngilizce çeviriler (fallback) --- */
static const char* const EN[I18N_KEY_COUNT] = {
    [I18N_MENU_QUICK_START]   = "> QUICK START",
    [I18N_MENU_SETTINGS]      = "> SETTINGS",
    [I18N_MENU_JOYSTICK_TEST] = "> JOYSTICK TEST",
    [I18N_MENU_COLORS]        = "> COLORS",
    [I18N_MENU_ABOUT]         = "> ABOUT",

    [I18N_SETTINGS_LANG]      = " Language",
    [I18N_SETTINGS_COLOR]     = " Theme",
    [I18N_SETTINGS_TIME]      = " Time",

    [I18N_MSG_SAVED]          = "Saved",
    [I18N_MSG_APPLIED]        = "Applied",
    [I18N_MSG_BACK]           = "Back",
    [I18N_MSG_OK]             = "OK",

    [I18N_TITLE_BRAND]        = "YourR&D",
};

/* --- Almanca çeviriler --- */
static const char* const DE[I18N_KEY_COUNT] = {
    [I18N_MENU_QUICK_START]   = "> SCHNELLSTART",
    [I18N_MENU_SETTINGS]      = "> EINSTELLUNGEN",
    [I18N_MENU_JOYSTICK_TEST] = "> JOYSTICK TEST",
    [I18N_MENU_COLORS]        = "> FARBEN",
    [I18N_MENU_ABOUT]         = "> INFO",

    [I18N_SETTINGS_LANG]      = " Sprache",
    [I18N_SETTINGS_COLOR]     = " Thema",
    [I18N_SETTINGS_TIME]      = " Zeit",

    [I18N_MSG_SAVED]          = "Gespeichert",
    [I18N_MSG_APPLIED]        = "Angewendet",
    [I18N_MSG_BACK]           = "Zurück",
    [I18N_MSG_OK]             = "OK",

    [I18N_TITLE_BRAND]        = "IhrF&E",
};

/* Aktif tabloyu döndür (null olamaz) */
static const char* const* prv_active_table(void) {
    switch (s_lang) {
        case I18N_TR: return TR;
        case I18N_DE: return DE;
        case I18N_EN:
        default:      return EN;
    }
}

/* ---- API ---- */
void I18N_Init(void)                { s_lang = I18N_TR; }
void I18N_SetLang(I18N_Lang lang)   { if (lang < I18N_LANG_COUNT) s_lang = lang; }
I18N_Lang I18N_CurrentLang(void)    { return s_lang; }

void I18N_SetLangByIndex(uint8_t idx) {
    if (idx < I18N_LANG_COUNT) s_lang = (I18N_Lang)idx;
}
uint8_t I18N_CurrentLangIndex(void) { return (uint8_t)s_lang; }

const char* I18N_Get(I18N_Key key) {
    if (key >= I18N_KEY_COUNT) return "";
    const char* const* T = prv_active_table();
    const char* s = T[key];
    if (!s || !*s) s = EN[key];      /* eksikse EN’e düş */
    return s ? s : "";
}

uint8_t I18N_LangCount(void)           { return (uint8_t)I18N_LANG_COUNT; }
const char* I18N_LangNameAt(uint8_t i) { return (i < I18N_LANG_COUNT) ? kLangNames[i] : ""; }
