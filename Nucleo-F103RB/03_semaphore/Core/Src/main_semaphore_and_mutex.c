

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include "gpio.h"
#include "ledImplementation.h"

/* Private includes ----------------------------------------------------------*/
#include "log.h"
#include "rtt.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <SEGGER_SYSVIEW.h>
#include <timers.h>

/**
 * @brief In this version of the code, we're using the FreeRTOS API to create tasks and semaphores. 
 * The insert and remove_item functions use xSemaphoreTake and xSemaphoreGive to acquire and release the buffer_mutex semaphore. 
 * The producer and consumer functions use xSemaphoreTake and xSemaphoreGive to acquire and release the items_sem and spaces_sem semaphores, respectively.
*  The buffer_mutex semaphore is created using xSemaphoreCreateMutex, which creates a binary semaphore that can be used for mutual exclusion. 
*  The items_sem and spaces_sem semaphores are created using xSemaphoreCreateCounting, 
*  which creates counting semaphores that can be used for synchronization and limiting access to a shared resource.
*  Overall, this code demonstrates how the FreeRTOS API can be used to implement mutexes and semaphores to synchronize access to a shared resource in a multi-threaded application.
 * 
 */

#define BUFFER_SIZE 10
#define STACK_SIZE 128

int buffer[BUFFER_SIZE];
int buffer_index;

SemaphoreHandle_t buffer_mutex;
SemaphoreHandle_t items_sem, spaces_sem;

TaskHandle_t producer_handle;
TaskHandle_t consumer_handle;

void one_shot_callback(TimerHandle_t xTimer);

void insert(int item) {
    if(xSemaphoreTake(buffer_mutex, portMAX_DELAY) == pdPASS)
    {
        buffer[buffer_index] = item;
        buffer_index++;
        xSemaphoreGive(buffer_mutex);
    }

    GreenLed.On();
    vTaskDelay(100/portTICK_PERIOD_MS);
    GreenLed.Off();
    vTaskDelay(100/portTICK_PERIOD_MS);
}

int remove_item() {
    int item;
    if(xSemaphoreTake(buffer_mutex, portMAX_DELAY) == pdPASS)
    {
        buffer_index--;
        item = buffer[buffer_index];
        xSemaphoreGive(buffer_mutex);
    }

    YellowLed.On();
    vTaskDelay(100/portTICK_PERIOD_MS);
    YellowLed.Off();
    vTaskDelay(100/portTICK_PERIOD_MS);

    return item;
}

void producer(void* arg) {
    int item = 0;
    while (1) {
        if(xSemaphoreTake(spaces_sem, portMAX_DELAY) == pdPASS)
        {
			insert(item);
			xSemaphoreGive(items_sem);
			SEGGER_SYSVIEW_PrintfHost("Produced item: %d\n", item);
			item++;
        }
    }
}

void consumer(void* arg) {
    int item;
    while (1) {
        if(xSemaphoreTake(items_sem, portMAX_DELAY) == pdPASS)
        {
			item = remove_item();
			SEGGER_SYSVIEW_PrintfHost("Consumed item: %d\n", item);
			xSemaphoreGive(spaces_sem);
        }
    }
}

void one_shot_callback(TimerHandle_t xTimer)
{
    vTaskResume(consumer_handle);
    vTaskSuspend(producer_handle);
}

void SystemClock_Config(void);


int main() {
    /* MCU Configuration--------------------------------------------------------*/
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();

    /* Startup Info */
    log_demo();
    YellowLed.Off();
    GreenLed.Off();
    RedLed.Off();
    BlueLed.Off();

    /*Configure SysView*/
    SEGGER_SYSVIEW_Conf();

    xTaskCreate(producer, "Producer", configMINIMAL_STACK_SIZE, NULL, 1, &producer_handle);
    xTaskCreate(consumer, "Consumer", configMINIMAL_STACK_SIZE, NULL, 1, &consumer_handle);


    buffer_mutex = xSemaphoreCreateMutex();
    items_sem = xSemaphoreCreateCounting(BUFFER_SIZE, 0);
    spaces_sem = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);

    vTaskSuspend(consumer_handle);

    TimerHandle_t myTimer = xTimerCreate("oneShotTimer",
                                         4000 / portTICK_PERIOD_MS, pdFALSE,
                                         NULL,
                                         one_shot_callback);

    assert_param(myTimer != NULL);
    xTimerStart(myTimer,0);
    
    vTaskStartScheduler();

    while (1)
    {
        /* code */
    }

    return 0;
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
