

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

void sending_task(void *args);
void receiving_task(void *args);

/*Create LED mask*/
#define GREEN_LED_MASK (1 << 0)
#define BLUE_LED_MASK (1 << 1)
#define RED_LED_MASK (1 << 2)
#define YELLOW_LED_MASK (1 << 3)



typedef struct 
{
    uint8_t green_led_state : 1; 
    uint8_t blue_led_state : 1; 
    uint8_t red_led_state : 1; 
    uint8_t yellow_led_state : 1; 
    uint32_t ms_delay;
}led_states_t;


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
    uint32_t notification_val = 0x00;
    uint8_t led_idx = 0;
    while (1)
    {
        if(led_idx < 4)
            notification_val |= (1 << led_idx++);
        else {
            led_idx = 0; notification_val = 0;
        }
        xTaskNotify(receiving_task_handle, notification_val, eSetValueWithOverwrite);
        SEGGER_SYSVIEW_PrintfHost("Led CMD Notified - [%d]\r\n", notification_val);
        vTaskDelay(200);
    }
}

void receiving_task(void *args)
{

  while (1)
  {
        uint32_t notification_val = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        SEGGER_SYSVIEW_PrintfHost("Led CMD Received - [%d]\r\n", notification_val);

        if ((notification_val & GREEN_LED_MASK) != 0)
            GreenLed.On();
        else
            GreenLed.Off();

        if ((notification_val & RED_LED_MASK) != 0)
            RedLed.On();
        else
            RedLed.Off();

        if ((notification_val & BLUE_LED_MASK) != 0)
            BlueLed.On();
        else
            BlueLed.Off();

        if ((notification_val & YELLOW_LED_MASK) != 0)
            YellowLed.On();
        else
            YellowLed.Off();
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
