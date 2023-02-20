
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
	log_message(LOG_LEVEL_INFO,"***** Theme : Semaphore Priority Inversion ******\r\n");
	log_message(LOG_LEVEL_INFO,"************************************\r\n");
}


/* Example Description  ---------------------------------------------------------*/

/**
 * @brief The numbers in this graph line up with the theoretical example, so if you've been following
   along closely, you may already know what to expect:
    1. TaskC (the lowest-priority task in the system) acquires a binary semaphore and
        starts to do some work (blinking the blue LED).
    2. Before TaskC completes its work, TaskB does some work.
    3. The highest-priority task (TaskA) interrupts and attempts to acquire the same
       semaphore, but is forced to wait because TaskC has already acquired the semaphore.
    4. TaskA times out after 200 ms because TaskC didn't have a chance to run (the higher-priority task, TaskB, was running instead). It lights up the red LED because of the failure.

    The fact that the lower-priority task (TaskB) was running while a higher-priority task was
    ready to run (TaskA) but waiting on a shared resource is called priority inversion. This is a
    reason to avoid using semaphores to protect shared resources.

    @note  The shared resource will be the function that's used to blink the LEDs.
 * 
 */

/* Task Definition  ---------------------------------------------------------*/
#define STACK_SIZE 128

static void lookBusy( uint32_t numIterations );
static void blinkTwice( iLed* led );
void task_a(void *arguments); // highest priority
void task_b(void *arguments); // medium priority 
void task_c(void *arguments); // low priority 

static TaskHandle_t task_a_handle;
static TaskHandle_t task_b_handle;
static TaskHandle_t task_c_handle;

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
    YellowLed.Off();
    GreenLed.Off();
    RedLed.Off();
    BlueLed.Off();

    /* Startup Info */
    print_startup_msg();
    log_demo();

    /*Configure SysView*/
    SEGGER_SYSVIEW_Conf();

    /*Create Dynamic task */
    assert_param(xTaskCreate(task_a, "task A", STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, &task_a_handle) == pdPASS);
    assert_param(xTaskCreate(task_b, "task B", STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &task_b_handle) == pdPASS);
    assert_param(xTaskCreate(task_c, "task C", STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &task_c_handle) == pdPASS);

  	//create a semaphore using the FreeRTOS Heap
    semaphore_ptr = xSemaphoreCreateBinary();
    assert_param(semaphore_ptr != NULL);

    // initial give
    xSemaphoreGive(semaphore_ptr);

    // start the scheduler - shouldn't return unless there's a problem
    vTaskStartScheduler();

    // if you've wound up here, there is likely an issue with overrunning the freeRTOS heap
    while (1)
    {
    }
}

/**
 * Task A is the highest priority task in the system
 * it periodically takes the semaphore with a 200mS timeout
 * if the semaphore is taken within 200 mS, the red LED is turned off
 * and the green LED is blinked twice
 * if the semaphore is not taken within 200mS, the red LED is turned on
 * (after TaskA has been given context)
 */

#define NUMBER_OF_TIME_SLOTS (14)
uint8_t time_slot[NUMBER_OF_TIME_SLOTS] = {10,28,16,14,20,10,24,25,15,12,19,28,17,15};

void task_a(void *arguments)
{
    /*Variable initialization*/
    log_message(LOG_LEVEL_DEBUG, "on task a init\n");
    uint8_t time_index = 0;

    while (1)
    {
      SEGGER_SYSVIEW_PrintfHost("attempt to take semaphore\n");
      if(xSemaphoreTake(semaphore_ptr, 200) == pdPASS)
      {
        BlueLed.Off();
        SEGGER_SYSVIEW_PrintfHost("received semaphore\n");
        blinkTwice(&GreenLed);
        xSemaphoreGive(semaphore_ptr);
      }
      else
      {
        SEGGER_SYSVIEW_WarnfHost("Failed to receive semphr in time\n");
        BlueLed.On();
      }

      //sleep for a bit to let other tasks run
      time_index = (time_index + 1) % NUMBER_OF_TIME_SLOTS;
      uint8_t numLoops = time_slot[time_index];
      vTaskDelay(numLoops / portTICK_PERIOD_MS);

    }
}

/**
 * wait to receive semPtr and triple blink the Blue LED
 */
void task_b(void *arguments)
{
    /*Variable initialization*/
    log_message(LOG_LEVEL_DEBUG, "on task b init\n");
    int counter = 0;

    while (1)
    {
        SEGGER_SYSVIEW_PrintfHost("starting iteration %u", counter++);
        lookBusy(450000);
        vTaskDelay(80);
    }
}

/**
 * the lowest priority task in the system.  This task also attempts to
 * take the semaphore every 200mS, turning off the red LED and double
 * blinking the blue LED if it was taken within 200mS
 * takes the semaphore and turning on the red LED if it hasn't
 */
void task_c(void *arguments)
{
    /*Variable initialization*/
    log_message(LOG_LEVEL_DEBUG, "on task b init\n");

    while (1)
    {
		//'take' the semaphore with a 200mS timeout
		SEGGER_SYSVIEW_PrintfHost("attempt to take semPtr");
		if(xSemaphoreTake(semaphore_ptr, 200/portTICK_PERIOD_MS) == pdPASS)
		{
			YellowLed.Off();
			SEGGER_SYSVIEW_PrintfHost("received semPtr");
			blinkTwice(&RedLed);
			xSemaphoreGive(semaphore_ptr);
		}
		else
		{
			//this code is called when the semaphore wasn't taken in time
			SEGGER_SYSVIEW_WarnfHost("FAILED to receive semphr in time");
			YellowLed.On();
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
 * Blink the desired LED twice
 */
static void blinkTwice( iLed* led )
{
	for(uint32_t i = 0; i < 2; i++)
	{
		led->On();
		vTaskDelay(43/portTICK_PERIOD_MS);
		led->Off();
		vTaskDelay(43/portTICK_PERIOD_MS);
	}
}


/**
 * run a simple loop for numIterations
 * @param numIterations number of iterations to compute modulus
 */
static void lookBusy( uint32_t numIterations )
{
	__attribute__ ((unused)) volatile uint32_t dontCare = 0;
 	for(int i = 0; i < numIterations; i++)
	{
		dontCare = i % 4;
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
