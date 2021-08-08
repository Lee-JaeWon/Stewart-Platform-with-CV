/**
******************************************************************************
* @file           : main.c
* @brief          : Main program body
******************************************************************************
** This notice applies to any and all portions of this file
* that are not between comment pairs USER CODE BEGIN and
* USER CODE END. Other portions of this file, whether 
* inserted by the user or by software development tools
* are owned by their respective copyright owners.
*
* COPYRIGHT(c) 2021 STMicroelectronics
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim9;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart6_rx;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
//Timer/Counter
int cnt;
int Motor_cnt;
//Motor_CCR
uint16_t Motor1_CCR;
uint16_t Motor2_CCR;
uint16_t Motor3_CCR;
//direction_pin
uint16_t drv_8825;
//Tim3 setup
uint16_t Tim3_prescaler;
uint16_t Tim3_ARR;
uint16_t Tim4_prescaler;
uint16_t Tim4_ARR;
uint16_t Tim5_prescaler;
uint16_t Tim5_ARR;
//UART
uint8_t Data;
uint8_t rx_buff_temp[1];
uint8_t RXbuffer[64];
int RX_BUFF_SIZE = 1;
//UART_Data_process
int start_i = 0;
int End_i = 0;
int cut_i = 0;
int cut2_i = 0;
char num1[20];
char num2[20];
char num3[20];
int i = 0, j = 0;
//Leg Length(Result)
int L1_Size;
int L2_Size;
int L3_Size;
//Calculation
int Max_L_Size = 1000;
int default_L_Size = 84; // 100mm
const int first_joint_Size = 80;
const int second_joint_Size = 74;
GPIO_PinState pin_state1;
GPIO_PinState pin_state2;
GPIO_PinState pin_state3;
int Motor_speed = 105;
int a = 600;
int b = 3;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM9_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_USART6_UART_Init(void);                                    
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);


/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
//Stepper Motor Clock Setup
void Tim3_Clock_setup(int pulse){
  
  Tim3_ARR = 1000;
  
  if(pulse == 0){
    TIM3->PSC = 0;
    TIM3->ARR = 0;
  }
  else{
    Tim3_prescaler = (84000000/pulse)/Tim3_ARR; //84000000 / 84
    Tim3_prescaler = Tim3_prescaler - 1;
    Tim3_ARR = Tim3_ARR - 1;
    TIM3->PSC = Tim3_prescaler;
    TIM3->ARR = Tim3_ARR;
  }
}
void Tim4_Clock_setup(int pulse){
  
  Tim4_ARR = 1000;
  
  if(pulse == 0){
    TIM4->PSC = 0;
    TIM4->ARR = 0;
  }
  else{
    Tim4_prescaler = (84000000/pulse)/Tim4_ARR; //84000000 / 84
    Tim4_prescaler = Tim4_prescaler - 1;
    Tim4_ARR = Tim4_ARR - 1;
    TIM4->PSC = Tim4_prescaler;
    TIM4->ARR = Tim4_ARR;
  }
}
void Tim5_Clock_setup(int pulse){
  
  Tim5_ARR = 1000;
  
  if(pulse == 0){
    TIM5->PSC = 0;
    TIM5->ARR = 0;
  }
  else{
    Tim5_prescaler = (84000000/pulse)/Tim5_ARR; //84000000 / 84
    Tim5_prescaler = Tim5_prescaler - 1;
    Tim5_ARR = Tim5_ARR - 1;
    TIM5->PSC = Tim5_prescaler;
    TIM5->ARR = Tim5_ARR;
  }
}
void Tim_Clock_All_setup(int pulse){
  Tim3_Clock_setup(pulse);
  Tim4_Clock_setup(pulse);
  Tim5_Clock_setup(pulse);
}
//Timer interrupt
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim == &htim9)
    cnt++;
}
//External interrupt
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  
  //GPIO_PIN_13 -> Blue switch
  if(GPIO_Pin == GPIO_PIN_13){
    //DRV_8825 direction pin set
    drv_8825 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);
    if(drv_8825 == 0){ //set
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
    }
    else if(drv_8825 == 1){ //reset
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
    }
    //
  }
}
//UART interrupt
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  
  //UART6(PC to STM32)
  if(huart->Instance == USART6){
    
    RXbuffer[i] = rx_buff_temp[0];
    
    if(RXbuffer[i] == 's') //시작 알림
      start_i = i;
    else if(RXbuffer[i] == ',') //구분
      cut_i = i;
    else if(RXbuffer[i] == '/')
      cut2_i = i;
    else if(RXbuffer[i] == 'e'){ //끝 알림
      End_i = i;
      RXbuffer[i + 1] = 0;
      
      for(i = start_i+1; i < cut_i; i++)
        num1[j++] = RXbuffer[i];
      num1[j] = '\0';
      j = 0;
      
      for(i = cut_i+1; i < cut2_i; i++)
        num2[j++] = RXbuffer[i];
      num2[j] = '\0';
      j = 0;
      
      for(i = cut2_i+1; i < End_i; i++)
        num3[j++] = RXbuffer[i];
      num3[j] = '\0';
      j = 0;
      
      L1_Size = atoi(num1);
      L2_Size = atoi(num2);
      L3_Size = atoi(num3);
      
      i = 0;
      
    }
    i++;
    
  }
  
  HAL_UART_Receive_IT(&huart6, rx_buff_temp, RX_BUFF_SIZE); // 1바이트 수신
  
}

/* USER CODE END 0 */

/**
* @brief  The application entry point.
*
* @retval None
*/
int main(void)
{
  /* USER CODE BEGIN 1 */
  
  /* USER CODE END 1 */
  
  /* MCU Configuration----------------------------------------------------------*/
  
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  
  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */
  
  /* Configure the system clock */
  SystemClock_Config();
  
  /* USER CODE BEGIN SysInit */
  
  /* USER CODE END SysInit */
  
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM9_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim9);
  
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim5, TIM_CHANNEL_1);
  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
  
  HAL_UART_Receive_IT(&huart6, rx_buff_temp, RX_BUFF_SIZE);
  
  /* USER CODE END 2 */
  
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {     
    
    if(cnt == 500){     
      pin_state1 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);
      pin_state2 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10);
      pin_state3 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11);
      
      if((abs(L1_Size - L2_Size)) >= 0 && (abs(L1_Size - L2_Size)) <= 2){
        if(L1_Size > L3_Size && L2_Size > L3_Size){
          Tim5_Clock_setup(0);
          if(pin_state1 == 0 && pin_state2 == 0){
            Tim3_Clock_setup(Motor_speed);
            Tim4_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            Tim3_Clock_setup(Motor_speed);
            Tim4_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }else if(pin_state1 == 0 && pin_state2 == 1){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            Tim3_Clock_setup(Motor_speed);
            Tim4_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            Tim3_Clock_setup(Motor_speed);
            Tim4_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }else if(pin_state1 == 1 && pin_state2 == 0){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            Tim3_Clock_setup(Motor_speed);
            Tim4_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            Tim3_Clock_setup(Motor_speed);
            Tim4_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }else if(pin_state1 == 1 && pin_state2 == 1){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            Tim3_Clock_setup(Motor_speed);
            Tim4_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            Tim3_Clock_setup(Motor_speed);
            Tim4_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }
        }
        else if(L3_Size > L1_Size && L3_Size > L2_Size){
          Tim3_Clock_setup(0);
          Tim4_Clock_setup(0);
          if(pin_state3 == 0){
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }else if(pin_state3 == 1){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }
        }
      }
      else if((abs(L2_Size - L3_Size)) >= 0 && (abs(L2_Size - L3_Size)) <= 2){
        if(L2_Size > L1_Size && L3_Size > L1_Size){
          Tim3_Clock_setup(0); // 3번 안움직임
          if(pin_state2 == 0 && pin_state3 == 0){
            Tim4_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim4_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }else if(pin_state2 == 0 && pin_state3 == 1){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim4_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim4_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }else if(pin_state2 == 1 && pin_state3 == 0){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            Tim4_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim4_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }else if(pin_state2 == 1 && pin_state3 == 1){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim4_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim4_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }
        }
        else if(L1_Size > L2_Size && L1_Size > L3_Size){
          Tim4_Clock_setup(0);
          Tim5_Clock_setup(0);
          if(pin_state1 == 0){
            Tim3_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            Tim3_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }else if(pin_state1 == 1){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            Tim3_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            Tim3_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }
        }
      }
      else if((abs(L1_Size - L3_Size)) >= 0 && (abs(L1_Size - L3_Size)) <= 2){
        if(L3_Size > L2_Size && L1_Size > L2_Size){
          Tim4_Clock_setup(0);
          if(pin_state1 == 0 && pin_state3 == 0){
            Tim3_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim3_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }else if(pin_state1 == 0 && pin_state3 == 1){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim3_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim3_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }else if(pin_state1 == 1 && pin_state3 == 0){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            Tim3_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim3_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }else if(pin_state1 == 1 && pin_state3 == 1){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim3_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
            Tim3_Clock_setup(Motor_speed);
            Tim5_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }
        }
        else if(L2_Size > L1_Size && L2_Size > L3_Size){
          Tim3_Clock_setup(0);
          Tim5_Clock_setup(0);
          if(pin_state2 == 0){
            Tim4_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            Tim4_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }else if(pin_state2 == 1){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            Tim4_Clock_setup(Motor_speed);
            HAL_Delay(a);
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
            Tim4_Clock_setup(Motor_speed);
            HAL_Delay(a);
            Tim_Clock_All_setup(0);
            HAL_Delay(b);
          }
        }
      }
      else if(L1_Size > L2_Size && L1_Size > L3_Size){
        Tim4_Clock_setup(0);
        Tim5_Clock_setup(0);
        if(pin_state1 == 0){
          Tim3_Clock_setup(Motor_speed);
          HAL_Delay(a);
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
          Tim3_Clock_setup(Motor_speed);
          HAL_Delay(a);
          Tim_Clock_All_setup(0);
          HAL_Delay(b);
        }else if(pin_state1 == 1){
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
          Tim3_Clock_setup(Motor_speed);
          HAL_Delay(a);
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
          Tim3_Clock_setup(Motor_speed);
          HAL_Delay(a);
          Tim_Clock_All_setup(0);
          HAL_Delay(b);
        }
      }
      else if(L2_Size > L1_Size && L2_Size > L3_Size){
        Tim3_Clock_setup(0);
        Tim5_Clock_setup(0);
        if(pin_state2 == 0){
          Tim4_Clock_setup(Motor_speed);
          HAL_Delay(a);
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
          Tim4_Clock_setup(Motor_speed);
          HAL_Delay(a);
          Tim_Clock_All_setup(0);
          HAL_Delay(b);
        }else if(pin_state2 == 1){
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
          Tim4_Clock_setup(Motor_speed);
          HAL_Delay(a);
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
          Tim4_Clock_setup(Motor_speed);
          HAL_Delay(a);
          Tim_Clock_All_setup(0);
          HAL_Delay(b);
        }
      }
      else if(L3_Size > L1_Size && L3_Size > L2_Size){
        Tim3_Clock_setup(0);
        Tim4_Clock_setup(0);
        if(pin_state3 == 0){
          Tim5_Clock_setup(Motor_speed);
          HAL_Delay(a);
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
          Tim5_Clock_setup(Motor_speed);
          HAL_Delay(a);
          Tim_Clock_All_setup(0);
          HAL_Delay(b);
        }else if(pin_state3 == 1){
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
          Tim5_Clock_setup(Motor_speed);
          HAL_Delay(a);
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
          Tim5_Clock_setup(Motor_speed);
          HAL_Delay(a);
          Tim_Clock_All_setup(0);
          HAL_Delay(b);
        }
      }
      else if(L1_Size == L2_Size && L2_Size == L3_Size){
        Tim_Clock_All_setup(0);
      }
      else if(abs(L1_Size - L2_Size) < 3 && abs(L2_Size - L3_Size) < 3 && abs(L1_Size - L3_Size) < 3){
        Tim_Clock_All_setup(0);
      }
      
      cnt = 0;
    }
    
    /* USER CODE END WHILE */
    
    /* USER CODE BEGIN 3 */
    
  }
  /* USER CODE END 3 */
  
}

/**
* @brief System Clock Configuration
* @retval None
*/
void SystemClock_Config(void)
{
  
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  
  /**Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
    |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  /**Configure the Systick interrupt time 
  */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
  
  /**Configure the Systick 
  */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* TIM3 init function */
static void MX_TIM3_Init(void)
{
  
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1000-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 100;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  HAL_TIM_MspPostInit(&htim3);
  
}

/* TIM4 init function */
static void MX_TIM4_Init(void)
{
  
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 1000-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 1000-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 100;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  HAL_TIM_MspPostInit(&htim4);
  
}

/* TIM5 init function */
static void MX_TIM5_Init(void)
{
  
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 1000-1;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 1000-1;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_PWM_Init(&htim5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 100;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  HAL_TIM_MspPostInit(&htim5);
  
}

/* TIM9 init function */
static void MX_TIM9_Init(void)
{
  
  TIM_ClockConfigTypeDef sClockSourceConfig;
  
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 839;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 20-1;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{
  
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
}

/* USART6 init function */
static void MX_USART6_UART_Init(void)
{
  
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
}

/** 
* Enable DMA controller clock
*/
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  
  /* DMA interrupt init */
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  
}

/** Configure pins as 
* Analog 
* Input 
* Output
* EVENT_OUT
* EXTI
*/
static void MX_GPIO_Init(void)
{
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10 
                    |GPIO_PIN_11, GPIO_PIN_RESET);
  
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);
  
  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
  
  /*Configure GPIO pins : LD2_Pin PA8 PA9 PA10 
  PA11 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10 
    |GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /*Configure GPIO pins : PB2 PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
* @brief  This function is executed in case of error occurrence.
* @param  file: The file name as string.
* @param  line: The line in file as a number.
* @retval None
*/
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
  tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
