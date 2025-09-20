#ifndef FONTS_H
#define FONTS_H
// Bu satırlar, başlık dosyasının birden fazla dahil edilmesini önler. // These lines prevent the header file from being included more than once.

#include <stdint.h>
// Bu, sabit boyutlu tamsayı türlerini (örneğin uint8_t, uint16_t) kullanmak için gereklidir. // This is necessary to use fixed-size integer types (e.g., uint8_t, uint16_t).

// 1-bpp, satır başına 1 adet uint16_t (sol->sağ bitler, soldan başlayarak)
// 1-bpp (1 bit per pixel), 1 uint16_t per row (left->right bits, starting from left)
typedef struct {
    uint8_t   width;
    // Fontun piksel cinsinden genişliği. // The width of the font in pixels.
    uint8_t   height;
    // Fontun piksel cinsinden yüksekliği. // The height of the font in pixels.
    const uint16_t *data;   // !!! uint16_t oldu
    // Font karakterlerinin bit haritasını içeren sabit veriyi işaret eden pointer. // A pointer to the constant data containing the bitmap of the font characters.
    // Veri artık 16 bitlik tamsayılar olarak depolanıyor. // The data is now stored as 16-bit integers.
} FontDef;
// FontDef adında bir yapı (struct) tanımlar. // Defines a structure named FontDef.

extern const FontDef Font_7x10;
// Başka bir dosyada tanımlanmış olan 7x10 piksel boyutundaki fontu bildirir. // Declares the 7x10 pixel font, which is defined in another file.
extern const FontDef Font_11x18;
// Başka bir dosyada tanımlanmış olan 11x18 piksel boyutundaki fontu bildirir. // Declares the 11x18 pixel font, which is defined in another file.
extern const FontDef Font_16x26;
// Başka bir dosyada tanımlanmış olan 16x26 piksel boyutundaki fontu bildirir. // Declares the 16x26 pixel font, which is defined in another file.

#endif // FONTS_H
// "#ifndef" bloğunu sonlandırır. // Ends the "#ifndef" block.
