
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include "stdlib.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
#include "log.h"
#include "rtt.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <SEGGER_SYSVIEW.h>


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
	log_message(LOG_LEVEL_INFO,"***** Theme : Semaphore Time Bound ******\r\n");
	log_message(LOG_LEVEL_INFO,"************************************\r\n");
}

/* Task Definition  ---------------------------------------------------------*/
/**
 * @brief In this example, we'll be using all three LEDs on the dev board to indicate different states:
Green LED: GreenTaskA() blinks at a steady 5 Hz with a 50% duty cycle.
Blue LED: Rapid blinks three times when TaskB() receives the semaphore
within 500 ms.
Red LED: Turned on after a timeout from xSemaphoreTake(). This is left on
until it's reset by TaskB(), as long as it receives the semaphore within 500 ms of
starting to wait for it.
 * 
 */
#define STACK_SIZE 128

void task_a(void *arguments);
void task_b(void *arguments);

/**
 * @brief GreenTaskA() has two responsibilities: 
    * Blink the green LED
    * Give the semaphore at pseudo-random intervals
 */
static TaskHandle_t task_a_handle;
static TaskHandle_t task_b_handle;

//create storage for a pointer to a semaphore
SemaphoreHandle_t semaphore_ptr;

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

    /*Create Dynamic task */
    assert_param(xTaskCreate(task_a, "task A", STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &task_a_handle) == pdPASS);
    assert_param(xTaskCreate(task_b, "task B", STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &task_b_handle) == pdPASS);

  	//create a semaphore using the FreeRTOS Heap
    semaphore_ptr = xSemaphoreCreateBinary();
    assert_param(semaphore_ptr != NULL);

    // start the scheduler - shouldn't return unless there's a problem
    vTaskStartScheduler();

    // if you've wound up here, there is likely an issue with overrunning the freeRTOS heap
    while (1)
    {
    }
}

/**
 * Task A periodically 'gives' semaphorePtr
 * NOTES:
 * - This semaphore isn't "given" to any task specifically
 * - giving the semaphore doesn't prevent taskA from continuing to run.
 *   Notice the green LED continues to blink at all times
 */

#define NUMBER_OF_TIME_SLOTS (14)
uint8_t time_slot[NUMBER_OF_TIME_SLOTS] = {2,1,6,4,7,3,4,5,6,2,3,3,4,5};

void task_a(void *arguments)
{
    /*Variable initialization*/
    log_message(LOG_LEVEL_DEBUG, "on task a init\n");
    uint8_t count = 0;
    uint8_t time_index;

    while (1)
    {
      time_index = (time_index + 1)%NUMBER_OF_TIME_SLOTS;
      uint8_t numLoops = time_slot[time_index];
      log_message(LOG_LEVEL_DEBUG, "num of loops %d\n", numLoops);

      if(++count >= numLoops)
      {
        // every 5 times through the loop, give the semaphore
        SEGGER_SYSVIEW_PrintfHost("give semaphore\n");
        xSemaphoreGive(semaphore_ptr);
        count = 0;
      }
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

/**
 * wait to receive semPtr and triple blink the Blue LED
 */
void task_b(void *arguments)
{
    /*Variable initialization*/
    log_message(LOG_LEVEL_DEBUG, "on task b init\n");

    while (1)
    {
      SEGGER_SYSVIEW_PrintfHost("attempt to take semaphore\n");
      if(xSemaphoreTake(semaphore_ptr, 500/portTICK_PERIOD_MS) == pdPASS)
      {
        SEGGER_SYSVIEW_PrintfHost("take semaphore\n");
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

        for (size_t i = 0; i < 3; i++)
        {
          HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
          vTaskDelay(50 / portTICK_PERIOD_MS);
          HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
          vTaskDelay(50 / portTICK_PERIOD_MS);
        }
      }
      else
      {
        SEGGER_SYSVIEW_WarnfHost("failed to take semaphore\n");
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
        vTaskDelay(50 / portTICK_PERIOD_MS);
      }
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
