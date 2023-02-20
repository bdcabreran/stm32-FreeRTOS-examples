
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
#include "log.h"
#include "rtt.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <SEGGER_SYSVIEW.h>
#include <timers.h>
#include "ledImplementation.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

void print_startup_msg(void)
{
	log_message(LOG_LEVEL_INFO,"************************************\r\n");
	log_message(LOG_LEVEL_INFO,"***** %s - %s ****/\r\n", __DATE__, __TIME__);
	log_message(LOG_LEVEL_INFO,"***** Author : Bayron Cabrera ******\r\n");
	log_message(LOG_LEVEL_INFO,"***** Theme : SW Timers ******\r\n");
	log_message(LOG_LEVEL_INFO,"************************************\r\n");
}

/* Task Definition  ---------------------------------------------------------*/
#define STACK_SIZE 128

void one_shot_callback(TimerHandle_t xTimer);
void repeat_callback(TimerHandle_t xTimer);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();

    /* Startup Info */
    print_startup_msg();
    log_demo();

    /*Configure SysView*/
    SEGGER_SYSVIEW_Conf();

    /*Create One Shot Timer*/
    GreenLed.Off();
    YellowLed.Off();
    RedLed.Off();
    BlueLed.On();
    TimerHandle_t one_shot_handle = xTimerCreate("myOneShotTimer",
                                                 4000 / portTICK_PERIOD_MS,
                                                 pdFALSE,
                                                 NULL,
                                                 one_shot_callback);

    assert_param(one_shot_handle != NULL);
    xTimerStart(one_shot_handle, 0);

    TimerHandle_t repeat_handle = xTimerCreate("myRepeatTimer",
                                                 500 / portTICK_PERIOD_MS,
                                                 pdTRUE,
                                                 NULL,
                                                 repeat_callback);

    assert_param(repeat_handle != NULL);
    xTimerStart(repeat_handle, 0);
    // start the scheduler - shouldn't return unless there's a problem
    vTaskStartScheduler();

    // if you've wound up here, there is likely an issue with overrunning the freeRTOS heap
    while (1)
    {
    }
}


void one_shot_callback( TimerHandle_t xTimer )
{
	SEGGER_SYSVIEW_PrintfHost("blue LED off");
	BlueLed.Off();
}

void repeat_callback(TimerHandle_t xTimer)
{
    static uint32_t counter = 0;

    SEGGER_SYSVIEW_PrintfHost("toggle Green LED");
    // toggle the green LED
    if (counter++ % 2)
    {
        GreenLed.On();
    }
    else
    {
        GreenLed.Off();
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}



/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  log_message_tag(LOG_LEVEL_ERROR, "ASSERT", "Wrong parameters value: file %s on line %ld\r\n", file, line);
}
#endif /* USE_FULL_ASSERT */
