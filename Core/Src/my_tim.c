#include "my_tim.h"

/************ Dahili durum ************/
static TIM_HandleTypeDef *s_htim = NULL;          // TR: Aktif timer handle
static volatile uint32_t  s_ms   = 0;             // TR: 1 ms sayacı
static MYTIM_Hook         s_hooks[MYTIM_MAX_HOOKS] = {0}; // TR: Kanca tablosu
/****************************************/

static inline TIM_HandleTypeDef* prv_get_handle(void)
{
    /* TR: Eğer Init çağrılmadıysa, config’teki varsayılan handle’ı bağla */
    if (!s_htim) {
#ifdef TICK_TIM_HANDLE
        s_htim = TICK_TIM_HANDLE;
#endif
    }
    return s_htim;
}

/************ API gerçeklemeleri ************/

void MYTIM_Init(TIM_HandleTypeDef *htim)
{
    /* TR: Dışarıdan handle verildiyse onu kullan, verilmediyse config’tekini kullanacağız */
    s_htim = htim ? htim : NULL;
    s_ms   = 0;
    for (int i = 0; i < MYTIM_MAX_HOOKS; i++) s_hooks[i] = 0;
}

HAL_StatusTypeDef MYTIM_Start(void)
{
    TIM_HandleTypeDef *h = prv_get_handle();
    if (!h) return HAL_ERROR;                      // TR: Handle yoksa hata
    return HAL_TIM_Base_Start_IT(h);               // TR: Kesme ile başlat
}

HAL_StatusTypeDef MYTIM_Stop(void)
{
    TIM_HandleTypeDef *h = prv_get_handle();
    if (!h) return HAL_ERROR;
    return HAL_TIM_Base_Stop_IT(h);
}

uint32_t MYTIM_Millis(void)
{
    return s_ms;                                   // TR: 1 ms sayaç
}

int MYTIM_AddHook(MYTIM_Hook fn)
{
    if (!fn) return -1;
    for (int i = 0; i < MYTIM_MAX_HOOKS; i++) {
        if (s_hooks[i] == NULL) {
            s_hooks[i] = fn;                       // TR: Boş yere ekle
            return i;                              // TR: Eklenen indeks
        }
    }
    return -1;                                     // TR: Tabloda yer yok
}

void MYTIM_RemoveHook(int index)
{
    if (index >= 0 && index < MYTIM_MAX_HOOKS) {
        s_hooks[index] = NULL;                     // TR: İlgili kancayı sil
    }
}

void MYTIM_ClearHooks(void)
{
    for (int i = 0; i < MYTIM_MAX_HOOKS; i++) s_hooks[i] = NULL;
}

/* TR: Bu fonksiyon HAL’ın periyodik kesmesinden çağrılmalıdır.
 *     Eşleşen timer ise 1 ms sayaç arttırılır ve tüm kancalar tetiklenir.
 */
void MYTIM_ISR(TIM_HandleTypeDef *htim)
{
    TIM_HandleTypeDef *h = prv_get_handle();
    if (!h) return;

    if (htim && (htim->Instance == h->Instance)) {
        s_ms++;                                    // TR: 1 ms arttır
        for (int i = 0; i < MYTIM_MAX_HOOKS; i++) {
            if (s_hooks[i]) {
                s_hooks[i]();                      // TR: 1 ms kancalarını çalıştır
            }
        }
    }
}

/* TR: Zayıf (weak) callback — projende **kendi** HAL_TIM_PeriodElapsedCallback yoksa
 *     bu çalışır ve MYTIM_ISR’ı çağırır. Eğer senin dosyanda tanımlıysa,
 *     derleyici onu kullanır; bu durumda kendi callback’inin içinden MYTIM_ISR(htim) çağırmalısın.
 */
__attribute__((weak)) void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    MYTIM_ISR(htim);
}
