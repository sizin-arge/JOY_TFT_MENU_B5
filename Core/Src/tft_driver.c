/**
 * @file tft_driver.c
 * @brief ST7789 TFT ekran sürücüsü fonksiyonları.
 *
 * Bu dosya, TFT ekranın donanım seviyesindeki temel işlemlerini yönetir:
 * - SPI üzerinden komut ve veri gönderme
 * - Ekranı başlatma ve konfigüre etme
 * - Adresleme penceresi belirleme
 * - Ekranı doldurma, metin ve karakter çizme
 */
#include "tft_driver.h" // Sürücüye özgü sabitler ve fonksiyon prototipleri
#include "main.h"      // HAL kütüphanesi ve GPIO/SPI pin tanımları için gerekli

// Panel çalışma boyutları
// Bu değişkenler, ekranın dönüş yönüne göre güncellenir.
static uint16_t _width = TFT_WIDTH;
static uint16_t _height = TFT_HEIGHT;

// Fonksiyonlar, ekranın mevcut genişliğini ve yüksekliğini döndürür.
uint16_t TFT_GetWidth(void)  { return _width;  }
uint16_t TFT_GetHeight(void) { return _height; }

// STM32CubeIDE projesinden gelen global SPI değişkeni
extern SPI_HandleTypeDef hspi1;

// ---- GPIO Kontrol Fonksiyonları ----
// Bu fonksiyonlar, TFT kontrol pinlerini (CS, DC, RST) yönetir.
// CS (Chip Select): Ekranla iletişim başlatmak ve bitirmek için kullanılır.
// DC (Data/Command): Gönderilen bilginin komut mu (RESET) yoksa veri mi (SET) olduğunu belirtir.
// RST (Reset): Ekranı donanımsal olarak sıfırlar.
void TFT_CS_Set(void)    { HAL_GPIO_WritePin(TFT_CS_GPIO_Port,  TFT_CS_Pin,  GPIO_PIN_SET);   }
void TFT_CS_Reset(void)  { HAL_GPIO_WritePin(TFT_CS_GPIO_Port,  TFT_CS_Pin,  GPIO_PIN_RESET); }
void TFT_DC_Set(void)    { HAL_GPIO_WritePin(TFT_DC_GPIO_Port,  TFT_DC_Pin,  GPIO_PIN_SET);   }
void TFT_DC_Reset(void)  { HAL_GPIO_WritePin(TFT_DC_GPIO_Port,  TFT_DC_Pin,  GPIO_PIN_RESET); }
void TFT_RST_Set(void)   { HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_SET);   }
void TFT_RST_Reset(void) { HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_RESET); }

// ---- Komut/Veri Gönderme Fonksiyonları ----
/**
 * @brief Ekran sürücüsüne bir komut gönderir.
 * @param cmd Gönderilecek 8-bit komut kodu.
 */
void TFT_WriteCommand(uint8_t cmd)
{
    TFT_CS_Reset(); // İletişimi başlat
    TFT_DC_Reset(); // Gönderilecek şeyin bir komut olduğunu belirt
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY); // 1 bayt komut gönder
    TFT_CS_Set(); // İletişimi sonlandır
}

/**
 * @brief Ekran sürücüsüne bir bayt veri gönderir.
 * @param data Gönderilecek 8-bit veri.
 */
void TFT_WriteData(uint8_t data)
{
    TFT_CS_Reset(); // İletişimi başlat
    TFT_DC_Set(); // Gönderilecek şeyin veri olduğunu belirt
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY); // 1 bayt veri gönder
    TFT_CS_Set(); // İletişimi sonlandır
}

/**
 * @brief 16-bit bir renk verisi gönderir (RGB565).
 * @param color Gönderilecek 16-bit renk değeri.
 */
void TFT_WriteColor(uint16_t color)
{
    // 16-bit rengi yüksek ve düşük olmak üzere 2 bayta ayır.
    uint8_t color_bytes[2] = { (uint8_t)(color >> 8), (uint8_t)(color & 0xFF) };
    TFT_CS_Reset(); // İletişimi başlat
    TFT_DC_Set(); // Gönderilecek şeyin veri olduğunu belirt
    HAL_SPI_Transmit(&hspi1, color_bytes, 2, HAL_MAX_DELAY); // 2 bayt renk verisi gönder
    TFT_CS_Set(); // İletişimi sonlandır
}

// ---- Adresleme Fonksiyonları ----
/**
 * @brief Ekranda çizim yapılacak alanı (pencereyi) ayarlar.
 *
 * Bu fonksiyon, komutlar aracılığıyla TFT'nin dahili RAM'inde hangi piksel alanına
 * yazılacağını belirler. Bu, tek bir pikselden tüm ekrana kadar herhangi bir
 * dikdörtgen alan olabilir.
 *
 * @param x1 Başlangıç X koordinatı.
 * @param y1 Başlangıç Y koordinatı.
 * @param x2 Bitiş X koordinatı.
 * @param y2 Bitiş Y koordinatı.
 */
void TFT_SetAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // Gerekirse panel ofseti ekleyebilirsin
    const uint16_t x_offset = 0;
    const uint16_t y_offset = 0;

    TFT_WriteCommand(TFT_CMD_CASET); // Sütun adresini ayarla (Column Address Set)
    // Başlangıç ve bitiş sütun adreslerini gönder.
    TFT_WriteData((x1 + x_offset) >> 8);
    TFT_WriteData((x1 + x_offset) & 0xFF);
    TFT_WriteData((x2 + x_offset) >> 8);
    TFT_WriteData((x2 + x_offset) & 0xFF);

    TFT_WriteCommand(TFT_CMD_RASET); // Satır adresini ayarla (Row Address Set)
    // Başlangıç ve bitiş satır adreslerini gönder.
    TFT_WriteData((y1 + y_offset) >> 8);
    TFT_WriteData((y1 + y_offset) & 0xFF);
    TFT_WriteData((y2 + y_offset) >> 8);
    TFT_WriteData((y2 + y_offset) & 0xFF);

    TFT_WriteCommand(TFT_CMD_RAMWR); // Veriyi belleğe yazmaya başla (Memory Write)
}

// ---- Başlatma Fonksiyonları ----
/**
 * @brief TFT ekranı başlatır ve temel ayarlarını yapar.
 * @param hspi Kullanılacak SPI_HandleTypeDef, bu projede global olarak hspi1 kullanılıyor.
 */
void TFT_Init(SPI_HandleTypeDef* hspi)
{
    (void)hspi; // hspi1 global kullanıldığı için bu parametre kullanılmıyor.

    // Donanımsal resetleme sırası
    TFT_RST_Reset(); HAL_Delay(50); // RST pinini çek, bir süre bekle
    TFT_RST_Set();   HAL_Delay(200); // RST pinini bırak, başlatma için bekle

    TFT_WriteCommand(TFT_CMD_SWRESET); // Yazılımsal reset komutu
    HAL_Delay(250);

    TFT_WriteCommand(TFT_CMD_SLPOUT); // Uyku modundan çıkış komutu
    HAL_Delay(150);

    TFT_WriteCommand(TFT_CMD_COLMOD); // Renk modu komutu
    TFT_WriteData(TFT_COLOR_MODE_65K_RGB565); // 16-bit RGB565 renk modunu ayarla

    TFT_WriteCommand(TFT_CMD_MADCTL); // Bellek erişim kontrolü komutu
    TFT_WriteData(0x00); // Varsayılan portre yönü için ayar (MADCTL = 0x00)

    TFT_WriteCommand(TFT_CMD_DISPON); // Ekranı aç komutu
    HAL_Delay(100);

    TFT_SetRotation(1);  // Ekranın varsayılan olarak yatay (landscape) yönde başlamasını sağlar.
}

/**
 * @brief Ekranı tek bir renkle doldurur.
 *
 * Bu fonksiyon, tüm ekranı kapsayan bir adres penceresi belirler ve her piksel için
 * verilen rengi gönderir.
 *
 * @param color Doldurulacak renk (16-bit RGB565).
 */
void TFT_FillScreen(uint16_t color)
{
    // Tüm ekranı kaplayacak adres penceresini ayarla.
    TFT_SetAddressWindow(0, 0, _width - 1, _height - 1);

    uint32_t pixels = (uint32_t)_width * _height;
    // Renk değerini yüksek ve düşük bayt olarak ayır.
    uint8_t hi = color >> 8, lo = color & 0xFF;

    TFT_CS_Reset(); // İletişimi başlat
    TFT_DC_Set(); // Veri gönderiliyor
    // Döngü içinde tüm piksellerin rengini gönder.
    for (uint32_t i = 0; i < pixels; i++) {
        HAL_SPI_Transmit(&hspi1, &hi, 1, HAL_MAX_DELAY);
        HAL_SPI_Transmit(&hspi1, &lo, 1, HAL_MAX_DELAY);
    }
    TFT_CS_Set(); // İletişimi sonlandır
}

/**
 * @brief Ekranın yönünü ayarlar ve ilgili genişlik/yükseklik değişkenlerini günceller.
 * @param rotation Yeni dönüş yönü (0, 1, 2 veya 3).
 */
void TFT_SetRotation(uint8_t rotation)
{
    TFT_WriteCommand(TFT_CMD_MADCTL); // Bellek erişim kontrolü komutu
    switch (rotation) {
        default:
        case 0: TFT_WriteData(0x00); _width = TFT_WIDTH;  _height = TFT_HEIGHT;  break;
        case 1: TFT_WriteData(0x60); _width = TFT_HEIGHT; _height = TFT_WIDTH;   break;
        case 2: TFT_WriteData(0xC0); _width = TFT_WIDTH;  _height = TFT_HEIGHT;  break;
        case 3: TFT_WriteData(0xA0); _width = TFT_HEIGHT; _height = TFT_WIDTH;   break;
    }
}

// ---- Metin Çizim Fonksiyonları ----
// Uyum: 16x26 font MSB-left, 7x10 ve 11x18 LSB-right (senin referans projene göre).
// Font tabloların satır başına 1 adet uint16_t word içeriyor.
/**
 * @brief Ekrana tek bir karakter çizer.
 *
 * Bu fonksiyon, font tablosundaki ilgili karakter verisini okur ve her bir bitine göre
 * ekrana renkli veya arka plan renginde bir piksel çizer.
 *
 * @param x Karakterin başlangıç X koordinatı.
 * @param y Karakterin başlangıç Y koordinatı.
 * @param c Çizilecek karakter.
 * @param font Kullanılacak font yapısı.
 * @param color Karakterin rengi.
 * @param bgColor Karakterin arka plan rengi.
 */
void TFT_DrawChar(uint16_t x, uint16_t y, char c, const FontDef* font,
                  uint16_t color, uint16_t bgColor)
{
    if (!font) return;
    uint8_t uc = (uint8_t)c;
    // Çizilemeyecek karakterler için '?' kullan.
    if (uc < 32 || uc > 126) uc = '?';

    // Font tablosundaki karakterin başlangıç ofsetini hesapla.
    // Her karakter, font yüksekliği kadar satırdan oluşur.
    uint32_t glyph_offset = (uint32_t)(uc - 32u) * (uint32_t)font->height;

    // Karakterin çizileceği dikdörtgen alanı belirle.
    TFT_SetAddressWindow(x, y, x + font->width - 1, y + font->height - 1);

    TFT_CS_Reset(); // İletişimi başlat
    TFT_DC_Set();   // Veri gönderiliyor

    for (uint16_t row = 0; row < font->height; row++) {
        uint16_t b = font->data[glyph_offset + row];  // O satırın 16-bit maskesini al

        for (uint16_t col = 0; col < font->width; col++) {
            // ESKİ REFERANS MANTIĞI:
            // Bu mantık, font verisinin her satırının en sol bitinin
            // karakterin en sol pikseline karşılık geldiğini varsayar (MSB-left).
            uint8_t on = ((uint16_t)(b << col) & 0x8000u) ? 1u : 0u;

            uint8_t px[2];
            if (on) { px[0] = (uint8_t)(color >> 8);   px[1] = (uint8_t)(color & 0xFF); }
            else    { px[0] = (uint8_t)(bgColor >> 8); px[1] = (uint8_t)(bgColor & 0xFF); }

            HAL_SPI_Transmit(&hspi1, px, 2, HAL_MAX_DELAY);
        }
    }

    TFT_CS_Set(); // İletişimi sonlandır
}

/**
 * @brief Ekrana bir karakter dizisi (string) çizer.
 *
 * Bu fonksiyon, karakter dizisini döngüye alarak her karakteri tek tek çizer.
 * Metnin satır sonuna geldiğinde yeni satıra geçiş yapar.
 *
 * @param x Dizinin başlangıç X koordinatı.
 * @param y Dizinin başlangıç Y koordinatı.
 * @param str Çizilecek karakter dizisi.
 * @param font Kullanılacak font yapısı.
 * @param color Metin rengi.
 * @param bgColor Metin arka plan rengi.
 */
void TFT_WriteString(uint16_t x, uint16_t y, const char* str, const FontDef* font,
                     uint16_t color, uint16_t bgColor)
{
    if (!str || !font) return;

    while (*str) {
        // Yeni satır karakterini kontrol et.
        if (*str == '\n') {
            x = 0; // X koordinatını sıfırla
            y += font->height; // Y koordinatını bir font yüksekliği kadar artır
            str++; // Sonraki karaktere geç
            continue;
        }

        // Metin ekranın genişliğini aşıyorsa yeni satıra geç.
        if (x + font->width > _width) {
            x = 0;
            y += font->height;
        }
        // Metin ekranın yüksekliğini aşıyorsa çizimi durdur.
        if (y + font->height > _height) {
            break;
        }

        // Karakteri çiz
        TFT_DrawChar(x, y, *str, font, color, bgColor);
        x += font->width; // Sonraki karakter için X koordinatını ilerlet
        str++; // Sonraki karaktere geç
    }
}

void TFT_DrawPixel(int x,int y,uint16_t color)
{
    if (x < 0 || y < 0 || x >= (int)_width || y >= (int)_height) return;
    TFT_SetAddressWindow((uint16_t)x,(uint16_t)y,(uint16_t)x,(uint16_t)y);
    TFT_WriteColor(color);
}

void TFT_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	if (x >= _width || y >= _height) return;
	    if (x + w > _width)  w = _width  - x;
	    if (y + h > _height) h = _height - y;

	    TFT_SetAddressWindow(x, y, x + w - 1, y + h - 1);

	    uint8_t hi = (uint8_t)(color >> 8), lo = (uint8_t)(color & 0xFF);
	    uint32_t count = (uint32_t)w * h;

	    TFT_CS_Reset();
	    TFT_DC_Set();
	    for (uint32_t i = 0; i < count; i++) {
	        HAL_SPI_Transmit(&hspi1, &hi, 1, HAL_MAX_DELAY);
	        HAL_SPI_Transmit(&hspi1, &lo, 1, HAL_MAX_DELAY);
	    }
	    TFT_CS_Set();
}

void TFT_DrawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color)
{
    TFT_FillRect(x,y,w,1,color);
}

void TFT_DrawVLine(int x,int y,int h,uint16_t color)
{
    TFT_FillRect(x,y,1,h,color);
}

/* ------------------------------------------------------------
 * METİN SARLAYICILAR (main/menu tarafından çağrılabilir)
 * ----------------------------------------------------------*/
void TFT_DrawTextEx(int x,int y,const char* s,const FontDef* font,
                    uint16_t fg,uint16_t bg)
{
    TFT_WriteString((uint16_t)x,(uint16_t)y,s,font,fg,bg);
}

void TFT_DrawText(int x,int y,const char* s,uint16_t fg,uint16_t bg)
{
    /* Varsayılan font: Font_11x18 (istersen değiştir) */
    TFT_DrawTextEx(x,y,s,&Font_11x18,fg,bg);
}
