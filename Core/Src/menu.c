// =============================================================================
//  MENU SİSTEMİ (STM32F3 + ST7789) — İç Mantık + Çizim (Sola Yaslı + Kayan Liste)
//  - ROOT: 5 satır (küçük-orta-büyük-orta-küçük)
//  - SETTINGS: 3 satır (küçük-büyük-küçük)
//  - Dairesel gezinme (wrap)
//  - Kayan liste: orta satır hep seçili; yukarı/aşağıda metinler kayar
//  - Sağ/sol: ROOT'ta SETTINGS seçiliyken sağ → Settings, SETTINGS'te sol → Root
// =============================================================================

#include "menu.h"

#include <string.h>
#include "tft_driver.h"   // TFT_FillRect, TFT_DrawHLine, TFT_WriteString, TFT_GetWidth/Height
//#include "ui_theme.h"     // UITheme_Arka(), UITheme_Yazi(), UITheme_YaziAlt(), UITheme_Cizgi()
//#include "i18n.h"         // I18N_Get(I18N_Key)
#include "fonts.h"        // Font_7x10, Font_11x18, Font_16x26
#include "my_joystick.h"  // JOY_ReadUpDown(), JOY_ReadLeftRight(), JOY_ResetLeftRight()

// =============================================================================
//  Derleme seçenekleri (kolay özelleştirme)
// =============================================================================
#ifndef MENU_VISIBLE_ROWS
#define MENU_VISIBLE_ROWS        5u   // ROOT görünür satır sayısı
#endif
#ifndef SETTINGS_VISIBLE_ROWS
#define SETTINGS_VISIBLE_ROWS    3u   // SETTINGS görünür satır sayısı
#endif

#ifndef MENU_DRAW_SEPARATORS
#define MENU_DRAW_SEPARATORS     0    // 1: satır ayracı çiz
#endif

#define MENU_ROW_X_PADDING       8    // Yazıya soldan boşluk (px)
#define MENU_ROW_VPAD            6    // Satır dikey padding (+/-)

// =============================================================================
//  Font alan adı uyarlama (projene göre Width/Height olabilir)
// =============================================================================
#ifndef FONT_W
#define FONT_W(f) ((f)->width)
#endif
#ifndef FONT_H
#define FONT_H(f) ((f)->height)
#endif
static inline uint16_t ROW_H(const FontDef* f) { return (uint16_t)(FONT_H(f) + MENU_ROW_VPAD); }

// =============================================================================
//  Tema kısa yolları
// =============================================================================
//static inline uint16_t UI_BG(void)     { return UITheme_Arka();    }
//static inline uint16_t UI_FG(void)     { return UITheme_Yazi();    }
//static inline uint16_t UI_SEL_BG(void) { return UITheme_YaziAlt(); }
//static inline uint16_t UI_SEL_FG(void) { return UITheme_Arka();    }
//static inline uint16_t UI_LINE(void)   { return UITheme_Cizgi();   }

static inline uint16_t UI_BG(void)     { return TFT_COLOR_BLACK; }     // arka plan
static inline uint16_t UI_FG(void)     { return TFT_COLOR_WHITE; }     // normal yazı
static inline uint16_t UI_SEL_BG(void) { return TFT_COLOR_BLACK; }       // seçili satır bg
static inline uint16_t UI_SEL_FG(void) { return TFT_COLOR_CYAN; }     // seçili yazı
static inline uint16_t UI_LINE(void)   { return TFT_COLOR_DARKGREY; }  // ayraç (opsiyonel)



// =============================================================================
//  I18N anahtar dizileri (brife göre)
// =============================================================================
//static const I18N_Key s_root_keys[5] = {
//    I18N_MENU_QUICK_START,
//    I18N_MENU_SETTINGS,
//    I18N_MENU_JOYSTICK_TEST,
//    I18N_MENU_COLORS,
//    I18N_MENU_ABOUT
//};

//static const I18N_Key s_set_keys[3] = {
//    I18N_SETTINGS_LANG,
//    I18N_SETTINGS_COLOR,
//    I18N_SETTINGS_TIME
//};

static const char* ROOT_EN[5] = {
		 "> Quick start",
		 "> Settings",
		 "> Joystick Test",
		 "> Colors",
		 "> About"
};

static const char* SET_EN[3]  = {
		"> Language",
	    "> Theme",
	    "> Time"
};

// =============================================================================
//  İç durum (state + seçim) ve yardımcılar
// =============================================================================
typedef enum { MENU_STATE_ROOT = 0, MENU_STATE_SETTINGS } MenuState;
static MenuState s_state;
static uint16_t  s_sel_root;  // 0..4
static uint16_t  s_sel_set;   // 0..2

static inline uint8_t  in_root(void)           { return (s_state == MENU_STATE_ROOT); }
static inline uint16_t active_count(void)      { return in_root() ? 5u : 3u; }
static inline uint16_t* active_sel_ptr(void)   { return in_root() ? &s_sel_root : &s_sel_set; }

// dairesel sarmalama
static inline uint16_t wrap_index(int32_t val, uint16_t count) {
    if (count == 0) return 0;
    int32_t m = val % (int32_t)count; if (m < 0) m += count; return (uint16_t)m;
}

// aktif menüye göre metin getir
//static inline const char* text_for(uint16_t idx) {
//    return in_root() ? I18N_Get(s_root_keys[idx]) : I18N_Get(s_set_keys[idx]);
//}

static inline const char* text_for(uint16_t idx) {
    if (in_root()) {
        if (idx > 4) idx = 4;
        return ROOT_EN[idx];
    } else {
        if (idx > 2) idx = 2;
        return SET_EN[idx];
    }
}

// =============================================================================
//  Layout: font paterni, dikey merkezleme, satır koordinatları
// =============================================================================
static inline const FontDef* font_for_row(uint8_t row, uint8_t rows) {
    if (rows == 5) {
        if (row == 2) return &Font_16x26;              // merkez (büyük)
        if (row == 1 || row == 3) return &Font_11x18;  // orta
        return &Font_7x10;                              // küçük
    } else { // 3 satır
        if (row == 1) return &Font_16x26;               // merkez (büyük)
        return &Font_11x18;                              // küçük
    }
}

static uint16_t page_total_h(uint8_t rows) {
    uint16_t sum = 0;
    for (uint8_t r = 0; r < rows; ++r) sum += ROW_H(font_for_row(r, rows));
    return sum;
}

static int page_y0(uint8_t rows) {
    int H = (int)TFT_GetHeight();
    int block = (int)page_total_h(rows);
    int y0 = (H - block) / 2;
    return (y0 < 0) ? 0 : y0;
}

static int row_y(uint8_t row_index, uint8_t rows) {
    int y = page_y0(rows);
    for (uint8_t r = 0; r < row_index; ++r) y += ROW_H(font_for_row(r, rows));
    return y;
}

// sola yaslı yazım için sabit x
static inline int text_left_x(void) { return MENU_ROW_X_PADDING; }

// merkez satır index’i
static inline uint8_t mid_row(uint8_t rows) { return (rows == 5) ? 2u : 1u; }

// =============================================================================
//  Çizim (tek satır / tüm sayfa)
//  - Kayan liste: Görünen satırın temsil ettiği item = wrap(sel + (row - mid))
//  - Orta satır her zaman “seçili” renkleriyle çizilir
// =============================================================================
static void draw_row(uint8_t row, uint8_t selected_visual, uint16_t current_sel) {
    const uint8_t rows = in_root() ? MENU_VISIBLE_ROWS : SETTINGS_VISIBLE_ROWS;
    const uint8_t mid  = mid_row(rows);
    const int16_t rel  = (int16_t)row - (int16_t)mid;        // -2..+2 veya -1..+1

    const uint16_t cnt = active_count();
    const uint16_t item_index = wrap_index((int32_t)current_sel + (int32_t)rel, cnt);

    const FontDef* f = font_for_row(row, rows);
    const int y      = row_y(row, rows);
    const int h      = (int)ROW_H(f);
    const char* txt  = text_for(item_index);

    // satır arka planı
    TFT_FillRect(0, y, TFT_GetWidth(), h, selected_visual ? UI_SEL_BG() : UI_BG());

    // metin konumu
    const int text_y = y + ((h - (int)FONT_H(f)) / 2);
    const int text_x = text_left_x();

    // yazı
    TFT_WriteString(text_x, text_y, txt, f,
                    selected_visual ? UI_SEL_FG() : UI_FG(),
                    selected_visual ? UI_SEL_BG() : UI_BG());

#if MENU_DRAW_SEPARATORS
    if (row + 1u < rows) TFT_DrawHLine(0, y + h, TFT_GetWidth(), UI_LINE());
#endif
}

// --- Bottom hint overlays ----------------------------------------------------
static void draw_hints(uint16_t current_sel) {
    const FontDef* f = &Font_11x18;       // küçük, köşe ipuçları için ideal
    const int fw = (int)FONT_W(f), fh = (int)FONT_H(f);
    const int margin = 3;
    const int boxW = 90;                 // köşe temizleme kutu genişliği
    const int y = (int)TFT_GetHeight() - fh - margin;

    // Köşeleri önce temizle (eski yazı kalmasın)
    TFT_FillRect(0, y, boxW, fh + margin, UI_BG());                                  // sol alt
    TFT_FillRect(TFT_GetWidth() - boxW, y, boxW, fh + margin, UI_BG());              // sağ alt

    if (in_root()) {
        // Root'ta sadece SETTINGS (index=1) seçiliyken sağ altta göster
        if (current_sel == 1u) {
            const char* s = "> Enter";
            int w = (int)strlen(s) * fw;
            int x = TFT_GetWidth() - w - margin;
            TFT_WriteString(x, y, s, f, UI_SEL_FG(), UI_BG());
        }
    } else {
        // Alt menüde her zaman sol altta "Çıkış" ipucu
        const char* s = "< Back";
        int x = margin;
        TFT_WriteString(x, y, s, f, UI_SEL_FG(), UI_BG());
    }
}

static void draw_all_rows(uint16_t current_sel) {
    const uint8_t rows = in_root() ? MENU_VISIBLE_ROWS : SETTINGS_VISIBLE_ROWS;
    const uint8_t mid  = mid_row(rows);
    for (uint8_t r = 0; r < rows; ++r) {
        const uint8_t selected_visual = (r == mid);  // sadece orta satır seçili görünür
        draw_row(r, selected_visual, current_sel);
    }
    // ↓ ipuçlarını güncelle
        draw_hints(current_sel);
}







// =============================================================================
//  Public API
// =============================================================================
void Menu_Init(void) {
    s_state    = MENU_STATE_ROOT;
    s_sel_root = 0;
    s_sel_set  = 0;
}

void Menu_DrawAll(void) {


    TFT_FillRect(0, 0, TFT_GetWidth(), TFT_GetHeight(), UI_BG());
    draw_all_rows(*active_sel_ptr());
    Menu_DrawSizinArGe();   // <-- sadece bir kez


}

void Menu_Tick(void) {
    // 1) Yukarı/Aşağı (repeat destekli): -1/0/+1
    const int8_t ud = (int8_t)JOY_ReadUpDown();
    if (ud != 0) {
        Menu_Step(ud);
    }

    // 2) Sağ/Sol (edge tabanlı): -1 sol, +1 sağ
    const int8_t lr = (int8_t)JOY_ReadLeftRight();
    if (in_root() && lr > 0) {
        // Yalnızca SETTINGS seçiliyken alt menüye gir
        if (s_sel_root == 1u /* SETTINGS index */) {
            s_state = MENU_STATE_SETTINGS;
            Menu_DrawAll();
        }
        JOY_ResetLeftRight();
    }
    else if (!in_root() && lr < 0) {
        // Alt menüden geri dön
        s_state = MENU_STATE_ROOT;
        Menu_DrawAll();
        JOY_ResetLeftRight();
    }
}

void Menu_Step(int8_t delta) {
    uint16_t* psel = active_sel_ptr();
    const uint16_t cnt = active_count();
    if (delta == 0 || cnt == 0) return;

    const uint16_t old_sel = *psel;
    const uint16_t new_sel = wrap_index((int32_t)old_sel + (int32_t)delta, cnt);
    if (new_sel == old_sel) return;

    *psel = new_sel;

    // Kayan liste modunda her satırın metni değişir ⇒ tüm satırları çizmek en sade ve güvenli yol
    draw_all_rows(new_sel);
}

uint16_t Menu_GetSelectedIndex(void) {
    return *active_sel_ptr();
}

void Menu_SetSelectedIndex(uint16_t idx) {
    uint16_t* psel = active_sel_ptr();
    const uint16_t cnt = active_count();
    const uint16_t old = *psel;
    const uint16_t neo = (cnt == 0) ? 0 : (idx % cnt);
    if (neo == old) return;

    *psel = neo;
    draw_all_rows(neo);
}

void Menu_DrawSizinArGe(void)
{
    const char* txt   = "SizinArGe";
    const FontDef* f  = &Font_16x26;        // istersen &Font_16x26
    const int topGap  = 10;                  // üstten 1 tık boşluk
    const int x = (TFT_GetWidth()  - (int)FONT_W(f) * (int)strlen(txt)) / 2; // ortala
    const int y = topGap;

    // arkaplanı değiştirmek istemiyorsan UI_BG() kullan, sabit renk istersen TFT_COLOR_BLACK
    TFT_WriteString(x, y, txt, f, TFT_COLOR_CYAN, UI_BG());
}

