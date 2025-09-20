
#include "my_joystick.h"

/* ----------------- DEBUG (izleme için) ----------------- */
volatile uint16_t JOY_dbg_x_raw = 0;
volatile uint16_t JOY_dbg_y_raw = 0;
volatile int8_t   JOY_dbg_ud_dir = 0;
volatile int8_t   JOY_dbg_lr_dir = 0;
volatile int8_t   JOY_dbg_lr_edge = 0;
volatile uint32_t JOY_dbg_last_edge_ms = 0;

/* ----------------- Y (Up/Down) tekrar mantığı ----------------- */
static int8_t   s_ud_prev_dir = 0;       /* önceki yön (-1/0/+1)  */
static uint8_t  s_ud_repeating = 0;      /* 0: ilk bekleme, 1: tekrar */
static uint32_t s_ud_last_ms = 0;        /* son step zamanı (ms)  */

/* ----------------- X (Left/Right) edge mantığı ----------------- */
static int8_t   s_lr_prev_dir = 0;       /* önceki anlık yön      */

/* ---------------------------------------------------------------
 *  Yardımcı: Y ekseninden yön üret (-1/0/+1)
 *  - JOY_Y_INVERT = 1 ise yön terslenir
 * --------------------------------------------------------------*/
static int8_t prv_y_direction(uint16_t v)
{
#if JOY_Y_INVERT
    /* Ters: büyük değer "yukarı" gibi */
    if (v >= JOY_Y_HIGH_THR) return -1;  /* up   */
    if (v <= JOY_Y_LOW_THR)  return +1;  /* down */
#else
    if (v <= JOY_Y_LOW_THR)  return -1;  /* up   */
    if (v >= JOY_Y_HIGH_THR) return +1;  /* down */
#endif
    return 0;
}

/* ---------------------------------------------------------------
 *  Yardımcı: X ekseninden anlık yön üret (-1/0/+1)
 *  - JOY_X_INVERT = 1 ise yön terslenir
 * --------------------------------------------------------------*/
static int8_t prv_x_direction(uint16_t v)
{
    int8_t dir = 0;
    if (v <= JOY_X_LOW_THR)  dir = -1;   /* sol bölge */
    else if (v >= JOY_X_HIGH_THR) dir = +1; /* sağ bölge */

#if JOY_X_INVERT
    dir = -dir;
#endif
    return dir;
}

/* ---------------------------------------------------------------
 *  JOY_ReadUpDown()
 *  - Y ekseni okur; -1/0/+1 döner.
 *  - Basılı tutmada otomatik tekrar üretir:
 *    * İlk tekrar gecikmesi: JOY_STEP_FIRST_MS
 *    * Sürekli tekrar:       JOY_STEP_REPEAT_MS
 * --------------------------------------------------------------*/
int JOY_ReadUpDown(void)
{
    uint16_t vy = MYADC_ReadChannel(JOY_Y_CHANNEL);
    JOY_dbg_y_raw = vy;

    int8_t dir = prv_y_direction(vy);
    JOY_dbg_ud_dir = dir;

    uint32_t now = HAL_GetTick();

    if (dir == 0) {
        /* Nötr bölgede: tekrar motorunu sıfırla */
        s_ud_prev_dir = 0;
        s_ud_repeating = 0;
        return 0;
    }

    if (dir != s_ud_prev_dir) {
        /* Yön değişti: hemen bir adım üret, sayaç kur */
        s_ud_prev_dir = dir;
        s_ud_repeating = 0;
        s_ud_last_ms = now;
        return dir;
    }

    /* Aynı yönde basılı: tekrar zamanlaması */
    if (!s_ud_repeating) {
        if ((now - s_ud_last_ms) >= JOY_STEP_FIRST_MS) {
            s_ud_repeating = 1;
            s_ud_last_ms = now;
            return dir;
        }
        return 0;
    } else {
        if ((now - s_ud_last_ms) >= JOY_STEP_REPEAT_MS) {
            s_ud_last_ms = now;
            return dir;
        }
        return 0;
    }
}

/* ---------------------------------------------------------------
 *  JOY_ReadLeftRight()
 *  - X ekseninde kenar (edge) üretir:
 *    * sol edge = -1, sağ edge = +1, yok = 0
 *  - Debounce: JOY_DEBOUNCE_MS
 *  - NOT: Merkeze dönmeyi beklemeden sağdan sola veya soldan sağa
 *         hızlı geçişte de edge üretir (s_prev_dir -> dir değişimi).
 * --------------------------------------------------------------*/
int JOY_ReadLeftRight(void)
{
    uint16_t vx = MYADC_ReadChannel(JOY_X_CHANNEL);
    JOY_dbg_x_raw = vx;

    int8_t dir = prv_x_direction(vx);
    JOY_dbg_lr_dir = dir;

    uint32_t now = HAL_GetTick();
    int8_t edge = 0;

    if (dir != 0 && dir != s_lr_prev_dir) {
        /* Yeni tarafa girdik (veya direkt zıt tarafa atladık) */
        if ((now - JOY_dbg_last_edge_ms) >= JOY_DEBOUNCE_MS) {
            edge = (dir > 0) ? +1 : -1;
            JOY_dbg_last_edge_ms = now;
            s_lr_prev_dir = dir;
        }
    } else if (dir == 0) {
        /* Merkeze dönüldü: bir sonraki giriş için hazırla */
        s_lr_prev_dir = 0;
    }

    JOY_dbg_lr_edge = edge;
    return edge;
}

/* --------------------------------------------------------------- */
void JOY_ResetRepeat(void)
{
    s_ud_prev_dir = 0;
    s_ud_repeating = 0;
    s_ud_last_ms = HAL_GetTick();
}

void JOY_ResetLeftRight(void)
{
    s_lr_prev_dir = 0;
    JOY_dbg_last_edge_ms = HAL_GetTick();
    JOY_dbg_lr_edge = 0;
}

/* ---------------------------------------------------------------
 *  JOY_ButtonEdge()
 *  - 1: press edge, -1: release edge, 0: yok
 * --------------------------------------------------------------*/
int JOY_ButtonEdge(void)
{
#ifndef JOY_BTN_GPIO_Port
    return 0; /* Buton tanımlı değilse */
#else
    static uint8_t prev = 0xFF; /* bilinmiyor */
    static uint32_t last_ms = 0;

    uint8_t raw = (uint8_t)HAL_GPIO_ReadPin(JOY_BTN_GPIO_Port, JOY_BTN_Pin);
#if JOY_BTN_ACTIVE_LOW
    uint8_t now_state = (raw == GPIO_PIN_RESET) ? 1u : 0u; /* 1=basılı */
#else
    uint8_t now_state = (raw == GPIO_PIN_SET) ? 1u : 0u;   /* 1=basılı */
#endif

    uint32_t now = HAL_GetTick();
    if (prev == 0xFF) { prev = now_state; last_ms = now; return 0; } /* ilk örnek */

    if (now_state != prev && (now - last_ms) >= JOY_DEBOUNCE_MS) {
        int edge = now_state ? +1 : -1;
        prev = now_state;
        last_ms = now;
        return edge;
    }
    if (now_state == prev) {
        last_ms = now; /* stabil: zamanı güncelle */
    }
    return 0;
#endif
}
