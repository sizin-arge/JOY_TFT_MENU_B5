/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>          // snprintf
#include "tft_driver.h"
#include "fonts.h"
#include "config_pins.h"
#include "my_adc.h"
#include "my_joystick.h"
#include "menu.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

// Bu bölüme, programınız için özel veri tiplerini (struct, enum, union) tanımlayabilirsiniz.
// Bu, kodunuzu daha okunabilir ve düzenli hale getirmek için faydalıdır.
// In this section, you can define custom data types (struct, enum, union) for your program.
// This is useful for making your code more readable and organized.

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// Burada, projenize özel sabitleri (#define) veya makroları tanımlayabilirsiniz.
// Örneğin, TFT renkleri veya pin numaraları gibi değerleri burada belirleyebilirsiniz.
// Here you can define project-specific constants (#define) or macros.
// For example, you might define TFT colors or pin numbers here.

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

// Bu bölüme, tüm program boyunca erişebileceğiniz global değişkenleri tanımlayabilirsiniz.
// Örneğin, menü durumu veya sensör verileri gibi değişkenler buraya eklenebilir.
// In this section, you can declare global variables that are accessible throughout the program.
// For example, variables for menu state or sensor data can be added here.

volatile uint16_t dbg_adc_x = 0;
volatile uint16_t dbg_adc_y = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

// Projenizde kullandığınız yeni fonksiyonların prototiplerini (tanımlarını) buraya ekleyebilirsiniz.
// Bu, derleyicinin fonksiyonları tanıyabilmesini sağlar.
// You can add the prototypes (declarations) of new functions used in your project here.
// This allows the compiler to recognize your custom functions.

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


// Bu bölüm, main fonksiyonundan önce çalışacak kodlar veya özel başlatma işlemleri için kullanılabilir.
// This section can be used for code that runs before the main function or for special initialization routines.

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  // main fonksiyonunun en başında, HAL_Init() çağrılmadan önce çalışacak kodlar için kullanılır.
  // This section is used for code that should run at the very beginning of the main function,
  // before the HAL_Init() call.

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  // HAL_Init() çağrıldıktan sonra, sistem saati yapılandırması öncesinde çalışacak kodlar için kullanılır.
  // Bu bölüm, genellikle donanım başlatma öncesi özel ayarlamalar için idealdir.
  // This section is for code that should run after HAL_Init() but before the system clock is configured.
  // It is ideal for special settings before hardware initialization.

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  // Sistem saati yapılandırıldıktan sonra, çevrebirim başlatma öncesinde çalışacak kodlar için kullanılır.
  // Bu bölüm, sisteme bağlı diğer başlatma işlemleri için uygundur.
  // This section is for code that runs after the system clock is configured but before peripheral initialization.
  // It is suitable for other system-level initialization tasks.

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
                    // 1 ms kesmeyi başlat

  MYADC_Init(&hadc1);

  TFT_Init(&hspi1);
  TFT_SetRotation(1);    /* 0: portre, 1: yatay */

  // EKRANI MENÜ ÇİZECEK → ilk tam çizim:
  Menu_Init();
  Menu_DrawAll();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  // Bu ana döngü, programınızın kalbidir. Tekrarlayan tüm işler (sensör okuma, tuş kontrolü, ekran güncelleme vb.)
  // bu döngü içinde yer almalıdır.
  // This main loop is the heart of your program. All repetitive tasks (reading sensors, checking buttons,
  // updating the screen, etc.) should be placed inside this loop.

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  Menu_Tick();          // joystick oku + kısmi redraw
	      // HAL_Delay(1);      // istersen döngüyü sakinleştir
  /* USER CODE END 3 */

   }
}
