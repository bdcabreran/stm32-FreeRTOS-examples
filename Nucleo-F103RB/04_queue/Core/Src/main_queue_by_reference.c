

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
#include "queue.h"

/**
 * @brief Passing one byte by reference
 * 
 * Since a queue will make a copy of whatever it is holding, if the data structure being queued
 * is large, it will be inefficient to pass it around by value:
 * Sending and receiving from queues forces a copy of the queue element each time.
 * The resulting queue gets very large for large data items if large structures are
 * queued.
*/

#define BUFFER_SIZE 10
#define STACK_SIZE 128

TaskHandle_t sending_task_handle;
TaskHandle_t receiving_task_handle;

static QueueHandle_t led_cmd_queue = NULL; 

void sending_task(void *args);
void receiving_task(void *args);

typedef enum
{
  ALL_OFF = 0,
  GREEN_ON,
  GREEN_OFF,
  BLUE_ON,
  BLUE_OFF,
  RED_ON,
  RED_OFF,
  YELLOW_ON,
  YELLOW_OFF,
  ALL_ON
} led_cmds_t;

#define MAX_MSG_LEN (256)

typedef struct 
{
    uint8_t green_led_state : 1; 
    uint8_t blue_led_state : 1; 
    uint8_t red_led_state : 1; 
    uint8_t yellow_led_state : 1; 
    uint32_t ms_delay;
    char message[MAX_MSG_LEN];
}led_states_t;

static led_states_t led_state_1 = {1, 0, 0, 1, 1000, "The quick brown fox jumped over the lazy dog.The LED 1 and 4 is on."};
static led_states_t led_state_2 = {0, 1, 0, 1, 1000, "Another string. The LED 2 and 3 is on"};


/**
 * @brief Rather than copy 264 bytes every time an item is added or removed from ledCmdQueue, we
can define ledCmdQueue to hold a pointer (4 bytes on Cortex-M) to LedStates_t :
 * 
 */

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

    /*Create tasks */
    assert_param(xTaskCreate(receiving_task, "ReceivingTask", STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &receiving_task_handle ) == pdPASS);
    assert_param(xTaskCreate(sending_task, "SendingTask", STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &sending_task_handle ) == pdPASS);
    // assert_param(xTaskCreate(sending_task, "SendingTask", STACK_SIZE, NULL, configMAX_PRIORITIES -1, &sending_task_handle ) == pdPASS);

//    vTaskSuspend(receiving_task_handle);
    /* create queue */
    led_cmd_queue = xQueueCreate(8, sizeof(led_states_t *));
    assert_param(led_cmd_queue != NULL);

    log_message(LOG_LEVEL_DEBUG, "RTOS Version [%s]\r\n", tskKERNEL_VERSION_NUMBER);

    /*Start Scheduler */
     vTaskStartScheduler();

    while (1)
    {
        /* code */
    }

    return 0;
}

void sending_task(void *args)
{
  UBaseType_t uxQueueLength;
  led_states_t *state1_ptr = &led_state_1;
  led_states_t *state2_ptr = &led_state_2;

  while (1)
  {

    xQueueSend(led_cmd_queue, &state1_ptr, portMAX_DELAY);
    uxQueueLength = uxQueueMessagesWaiting(led_cmd_queue);
    SEGGER_SYSVIEW_PrintfHost("Led state 1 dispatched\r\n", uxQueueLength);

    xQueueSend(led_cmd_queue, &state2_ptr, portMAX_DELAY);
    uxQueueLength = uxQueueMessagesWaiting(led_cmd_queue);
    SEGGER_SYSVIEW_PrintfHost("Led state 2 dispatched\r\n", uxQueueLength);

  }
}

void receiving_task(void *args)
{
  led_states_t *led_state_prt;
  UBaseType_t uxQueueLength;

  while (1)
  {
    if (xQueueReceive(led_cmd_queue, &led_state_prt, portMAX_DELAY) == pdTRUE)
    {
      uxQueueLength = uxQueueMessagesWaiting(led_cmd_queue);
      SEGGER_SYSVIEW_PrintfHost("Led CMD Received - led ms [%d], count [%d]\r\n",
                                led_state_prt->ms_delay, uxQueueLength);
      SEGGER_SYSVIEW_PrintfHost("Message -> [%s]\r\n", led_state_prt->message);

      if (led_state_prt->green_led_state == 1)
          GreenLed.On();
      else
          GreenLed.Off();

      if (led_state_prt->red_led_state == 1)
          RedLed.On();
      else
          RedLed.Off();

      if (led_state_prt->blue_led_state == 1)
          BlueLed.On();
      else
          BlueLed.Off();

      if (led_state_prt->yellow_led_state == 1)
          YellowLed.On();
      else
          YellowLed.Off();
    }
    vTaskDelay(led_state_prt->ms_delay/portTICK_PERIOD_MS);
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
