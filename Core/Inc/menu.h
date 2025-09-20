// =============================================================================
//  MENU SİSTEMİ (STM32F3 + ST7789)
//  Public API — 5 satır ROOT / 3 satır SETTINGS, dairesel gezinme,
//  sağ/sol geçiş ve kısmi redraw.
// =============================================================================
#ifndef MENU_H
#define MENU_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void     Menu_Init(void);              // İç durumu hazırlar (ilk seçimler)
void     Menu_DrawAll(void);           // Tüm menüyü tam çizer (ilk/tema/dil)
void     Menu_Tick(void);              // Joystick oku + gerekli redraw
void     Menu_Step(int8_t delta);      // Harici test (+1 aşağı, -1 yukarı)
void     Menu_DrawSizinArGe(void);

uint16_t Menu_GetSelectedIndex(void);  // Aktif listedeki seçili index
void     Menu_SetSelectedIndex(uint16_t idx);

#ifdef __cplusplus
}
#endif

#endif // MENU_H
