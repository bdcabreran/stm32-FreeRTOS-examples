
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
	log_message(LOG_LEVEL_INFO,"************************************\r\n");
}

/* Task Definition  ---------------------------------------------------------*/
void led_green_task(void *arguments);
void led_blue_task(void *arguments);
void led_red_task(void *arguments);

#define STACK_SIZE  (128) //128 * 4 = 512 bytes

//define stack and task control block (TCB) for the red task
static StackType_t led_red_stack[STACK_SIZE];
static StaticTask_t led_red_TCB;

static TaskHandle_t led_blue_task_handle;
static TaskHandle_t led_red_task_handle;

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

    /* Task Parameters */
    int red_task_id = 0x66;

    /*Create Dynamic task */
    assert_param(xTaskCreate(led_green_task, "Green Task", STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL) == pdPASS);
    
    #if 1
    /*Passing handle for task created */
    assert_param(xTaskCreate(led_blue_task, "Blue Task", STACK_SIZE, NULL, tskIDLE_PRIORITY + 1,
                             &led_blue_task_handle) == pdPASS);
    #else 
    // Example of a task creation failure (See Task Size)
    assert_param(xTaskCreate(led_blue_task, "Blue Task", STACK_SIZE*100, NULL, tskIDLE_PRIORITY + 1,
                             &led_blue_task_handle) == pdPASS);
    #endif

    // Creating a Task statically
    led_red_task_handle = xTaskCreateStatic(led_red_task, "Red Task", STACK_SIZE, &red_task_id, tskIDLE_PRIORITY + 3,
                                   led_red_stack, &led_red_TCB);



    // start the scheduler - shouldn't return unless there's a problem
    vTaskStartScheduler();

    // if you've wound up here, there is likely an issue with overrunning the freeRTOS heap
    while (1)
    {
    }
}

void led_blue_task(void *arguments)
{
	log_message(LOG_LEVEL_DEBUG,"on led blue task init\n");

    while (1)
    {
        SEGGER_SYSVIEW_PrintfHost("Led Blue Task Running\n");
        HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void led_green_task(void *arguments)
{
    /*Variable initialization*/
	log_message(LOG_LEVEL_DEBUG,"on led green task init\n");

    int counter = 0;
    while (1)
    {
        SEGGER_SYSVIEW_PrintfHost("Led Green Task Running\n");
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
        vTaskDelay(1500 / portTICK_PERIOD_MS);
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
        vTaskDelay(1500 / portTICK_PERIOD_MS);

        if (counter++ >= 2)
        {
            // A task can delete itself by passing NULL to vTaskDelete
            SEGGER_SYSVIEW_PrintfHost("Led Green Deleted\n");
            vTaskDelete(NULL);

            // Code will never reach here !
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
        }
    }
}

void led_red_task(void *arguments)
{
    /*Variable initialization*/
    int id = *(int*)arguments;
    log_message(LOG_LEVEL_DEBUG,"on red task init, id = 0x%X\n", id);

    int first_run = 1;
    while (1)
    {
        SEGGER_SYSVIEW_PrintfHost("Red Task Running\n");
        vTaskDelay(3000);

        if (first_run)
        {
            // Delete blue task using handle
            SEGGER_SYSVIEW_PrintfHost("Led Blue Deleted\n");
            vTaskDelete(led_blue_task_handle);
            vTaskSuspend(led_red_task_handle);
            first_run = 0;
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
