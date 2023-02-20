
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

/**
 * @brief While binary semaphores can only have values between 0 and 1, counting semaphores can
have a wider range of values. Some use cases for counting semaphores include
simultaneous connections in a communication stack or static buffers from a memory pool.
 * 
 */

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
	log_message(LOG_LEVEL_INFO,"***** Theme : Semaphore Counting ******\r\n");
	log_message(LOG_LEVEL_INFO,"************************************\r\n");
}

/* Task Definition  ---------------------------------------------------------*/
#define STACK_SIZE 128
#define MAX_TCP_CONNECTIONS 3


void task_tcp(void *arguments);

static TaskHandle_t task_tcp_handle;

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
    assert_param(xTaskCreate(task_tcp, "task TCP", STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &task_tcp_handle) == pdPASS);

  	//create a semaphore using the FreeRTOS Heap

    /**
     * @brief For example, let's say we have a TCP/IP stack that supports multiple simultaneous TCP
        sessions, but the MCU only has enough RAM to support three simultaneous TCP sessions.
        This would be a perfect use case for a counting semaphore.
     *
     */
    semaphore_ptr = xSemaphoreCreateCounting(MAX_TCP_CONNECTIONS, 3);

    //! The code that requests to open a TCP session would take semPtr, reducing its count by 1:
    //! Whenever a TCP session is closed, the code closing the session gives semPtr, increasing its count by 1
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
void task_tcp(void *arguments)
{
    /*Variable initialization*/
    log_message(LOG_LEVEL_DEBUG, "on task a init\n");

    while (1)
    {
        SEGGER_SYSVIEW_PrintfHost("attempt to take semaphore\n");
        if (xSemaphoreTake(semaphore_ptr, 500) == pdTRUE) {
            UBaseType_t count = uxSemaphoreGetCount(semaphore_ptr);
            SEGGER_SYSVIEW_PrintfHost("take semaphore [%d]\n", count);
            // Create a new TCP connection and process data
            // ...
            
            vTaskDelay(5000/portTICK_PERIOD_MS);

            #if 0
            // Release the semaphore when the TCP connection is closed
            xSemaphoreGive(semaphore_ptr);
            #endif
        }
        else 
        {
            SEGGER_SYSVIEW_WarnfHost("couldn't take semaphore\n");
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
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
      if(xSemaphoreTake(semaphore_ptr, portMAX_DELAY) == pdPASS)
      {
        SEGGER_SYSVIEW_PrintfHost("take semaphore\n");

        for (size_t i = 0; i < 3; i++)
        {
          HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
          vTaskDelay(50 / portTICK_PERIOD_MS);
          HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
          vTaskDelay(50 / portTICK_PERIOD_MS);
        }
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
