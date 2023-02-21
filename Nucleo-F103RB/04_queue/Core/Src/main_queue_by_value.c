

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
 * @brief Passing one byte by value
In this example, a single uint8_t is set up to pass individual enumerations, (LED_CMDS),
defining the state of one LED at a time or all of the LEDs (on/off). Here's a summary of
what is covered in this example:
* ledCmdQueue: A queue of one-byte values (uint8_t) representing an enumeration defining LED states.
* recvTask: This task receives a byte from the queue, executes the desired action, and immediately attempts to receive the next byte from the queue.
* sendingTask: This task sends enumerated values to the queue using a simple
loop, with a 200 ms delay between each send (so the LEDs turning on/off are
visible).
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
    assert_param(xTaskCreate(sending_task, "SendingTask", STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &sending_task_handle ) == pdPASS);
    assert_param(xTaskCreate(receiving_task, "ReceivingTask", STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &receiving_task_handle ) == pdPASS);

    /* create queue */
    led_cmd_queue = xQueueCreate(10, sizeof(int));
    assert_param(led_cmd_queue != NULL);

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
  uint8_t led_cmd = ALL_OFF;

  while (1)
  {
    if(xQueueSend(led_cmd_queue, &led_cmd, portMAX_DELAY) == pdPASS)
    {
      SEGGER_SYSVIEW_PrintfHost("Led CMD dispatched = [%d]\r\n", led_cmd);
      led_cmd = (led_cmd + 1) % ALL_ON;

    }

    vTaskDelay(500/portTICK_PERIOD_MS);
  }

}

void receiving_task(void *args)
{
  uint8_t next_cmd = 0;

  while (1)
  {
    if(xQueueReceive(led_cmd_queue, &next_cmd, portMAX_DELAY) == pdTRUE)
    {
      SEGGER_SYSVIEW_PrintfHost("Led CMD Received = [%d]\r\n", next_cmd);

      switch (next_cmd)
      {
      case ALL_OFF:   { YellowLed.Off(); GreenLed.Off(); RedLed.Off(); BlueLed.Off(); }break;
      case GREEN_ON:  { GreenLed.On();  } break;
      case GREEN_OFF: { GreenLed.Off(); } break;
      case RED_ON:    { RedLed.On();    } break;
      case RED_OFF:   { RedLed.Off();   } break;
      case BLUE_ON:   { BlueLed.On();   } break;
      case BLUE_OFF:  { BlueLed.Off();  } break;
      case YELLOW_ON: { YellowLed.On(); } break;
      case YELLOW_OFF:{ YellowLed.Off();} break;
      case ALL_ON:    { YellowLed.On(); GreenLed.On(); RedLed.On(); BlueLed.On(); }break;
  
      default:
        break;
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
