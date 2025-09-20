#ifndef __TFT_DRIVER_H__
#define __TFT_DRIVER_H__

#include "stm32f3xx_hal.h"
#include <stdint.h>
#include "fonts.h"

/* ST7789 Komutları */
#define TFT_CMD_NOP         0x00
#define TFT_CMD_SWRESET     0x01
#define TFT_CMD_SLPIN       0x10
#define TFT_CMD_SLPOUT      0x11
#define TFT_CMD_INVOFF      0x20
#define TFT_CMD_INVON       0x21
#define TFT_CMD_DISPOFF     0x28
#define TFT_CMD_DISPON      0x29
#define TFT_CMD_CASET       0x2A
#define TFT_CMD_RASET       0x2B
#define TFT_CMD_RAMWR       0x2C
#define TFT_CMD_MADCTL      0x36
#define TFT_CMD_COLMOD      0x3A

/* Renk modu */
#define TFT_COLOR_MODE_65K_RGB565  0x55

/* Panel boyutları */
#define TFT_WIDTH   240
#define TFT_HEIGHT  320

/* RGB565 renkler */
#define TFT_COLOR_BLACK       0x0000
#define TFT_COLOR_NAVY        0x000F
#define TFT_COLOR_DARKGREEN   0x03E0
#define TFT_COLOR_DARKCYAN    0x03EF
#define TFT_COLOR_MAROON      0x7800
#define TFT_COLOR_PURPLE      0x780F
#define TFT_COLOR_OLIVE       0x7BE0
#define TFT_COLOR_LIGHTGREY   0xC618
#define TFT_COLOR_DARKGREY    0x7BEF
#define TFT_COLOR_BLUE        0x001F
#define TFT_COLOR_GREEN       0x07E0
#define TFT_COLOR_CYAN        0x07FF
#define TFT_COLOR_RED         0xF800
#define TFT_COLOR_MAGENTA     0xF81F
#define TFT_COLOR_YELLOW      0xFFE0
#define TFT_COLOR_WHITE       0xFFFF
#define TFT_COLOR_ORANGE      0xFD20
#define TFT_COLOR_GREENYELLOW 0xAFE5
#define TFT_COLOR_PINK        0xF81F

/* GPIO/SPI kontrol (eğer .c içinde public kullanıyorsanız başlıkta kalsın) */
void TFT_CS_Set(void);
void TFT_CS_Reset(void);
void TFT_DC_Set(void);
void TFT_DC_Reset(void);
void TFT_RST_Set(void);
void TFT_RST_Reset(void);

/* Komut/Veri */
void TFT_WriteCommand(uint8_t cmd);
void TFT_WriteData(uint8_t data);
void TFT_WriteColor(uint16_t color);

/* Adres penceresi */
void TFT_SetAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/* Temel API */
void     TFT_Init(SPI_HandleTypeDef* hspi);
void     TFT_SetRotation(uint8_t r);
uint16_t TFT_GetWidth(void);
uint16_t TFT_GetHeight(void);

/* Düşük seviye çizimler (menü.c bunları kullanıyor) */
void TFT_DrawPixel(int x,int y,uint16_t color);
void TFT_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void TFT_FillScreen(uint16_t color);
void TFT_DrawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
void TFT_DrawVLine(int x,int y,int h,uint16_t color);

/* Metin API’si */
void TFT_DrawChar(uint16_t x, uint16_t y, char c, const FontDef* font,
                  uint16_t fg, uint16_t bg);
/* Esnek metin: istediğin font ile */
void TFT_DrawTextEx(int x,int y,const char* s,const FontDef* font, uint16_t fg,uint16_t bg);
/* Kısa yol: varsayılan font ile */
void TFT_DrawText(int x,int y,const char* s,uint16_t fg,uint16_t bg);
/* İstersen mevcut ismin kalsın diye alias da sağlıyoruz: */
void TFT_WriteString(uint16_t x, uint16_t y, const char* str, const FontDef* font,
                     uint16_t color, uint16_t bgColor);



#endif /* __TFT_DRIVER_H__ */
