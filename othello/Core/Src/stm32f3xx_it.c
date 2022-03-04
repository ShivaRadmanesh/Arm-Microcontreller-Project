/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f3xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "LiquidCrystal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
int board[8][8];
int change[8][8];
//for debounce handling
int lastEx0 = 0;
int lastEx1 = 0;
int lastEx2 = 0;
int lastEx3 = 0;
int lastEx12 = 0;
int last = 0;
// location of player using keypad
int x = 3;
int y = 3;
// uart location
int ux = -1;
int uy = -1;
//current turn
int turn = 1;
int t = -1;  //received turn via uart
int state = 0; //0-> start  1->game  2->end
int invalidAlarm = 0; //led alarm for invalid move
int invalidStart = 0; //holds the start time of invalid alarm event
int winner = -1;
int clock = 0; //end animation
int whiteScore = 0;
int blackScore = 0;
// 7seg
int time = 13;
int remain = 60;
int volume = 0;
int counter = 0;
//current location string for displaying on LCD
char currentLocation[2] ="";
//strings for displaying scores
char whiteStr[5] = "";
char blackStr[5] = "";
//for uart
int pos = 0;
char buffer[100] = "";
char uart_Board[71] = "";
//string for uart result
char uartRes[12] = "";
char response[7] = "";
char move[4] = "";
//buzzerAlarm
int buzzAlarm = 0;
int effect = 0;
//end animation
int end = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

typedef unsigned char byte;

byte white[] = {
  0x00,
  0x0A,
  0x15,
  0x11,
  0x11,
  0x0A,
  0x04,
  0x00
};


byte black[] = {
  0x00,
  0x0A,
  0x1F,
  0x1F,
  0x1F,
  0x0E,
  0x04,
  0x00
};





void startBoard(){
	int i, j;
    for(i = 0; i < 8; i++){
        for(j = 0; j< 8; j++){
            board[i][j] = -1;
        }
    }

    board[3][4] = 1;
    board[4][4] = 0;
    board[4][3] = 1;
    board[3][3] = 0;

}

void printBoard(){
	int i, j;
	for(i=0; i< 8; i+=2){
		for(j=0; j<8; j++){
			setCursor(j, i/2);

			if(board[i][j] == -1 && board[i+1][j] == -1){
				print(" ");
			}

			else if(board[i][j] == -1 && board[i+1][j] == 0){
				write(0);
			}

			else if(board[i][j] == 0 && board[i+1][j] == -1){
				write(1);
			}

			else if(board[i][j] == -1 && board[i+1][j] == 1){
				write(2);
			}
			else if(board[i][j] == 1 && board[i+1][j] == -1){
				write(3);
			}

			else if(board[i][j] == 1 && board[i+1][j] == -0){
				write(4);
			}

			else if(board[i][j] == 0 && board[i+1][j] == 1){
				write(5);
			}

			else if(board[i][j] == 0 && board[i+1][j] == 0){
				write(6);
			}

			else if(board[i][j] == 1 && board[i+1][j] == 1){
				write(7);
			}

		}
	}

}

void printTurn(){
	setCursor(14, 0);
	if(turn == 0){
		print("W");
	}
	else{
		print("B");
	}
}

void printLocation(){
    switch(y){
        case 0:
             sprintf(currentLocation, "A%d",x+1);
             break;

        case 1:
             sprintf(currentLocation, "B%d",x+1);
             break;

        case 2:
             sprintf(currentLocation, "C%d",x+1);
             break;

        case 3:
             printf("x: %d\n", x);
             sprintf(currentLocation, "D%d",x+1);
             break;

        case 4:
             sprintf(currentLocation, "E%d",x+1);
             break;

        case 5:
             sprintf(currentLocation, "F%d",x+1);
             break;

        case 6:
             sprintf(currentLocation, "G%d",x+1);
             break;

        case 7:
             sprintf(currentLocation, "H%d",x+1);
             break;
    }
    setCursor(17, 0);
    print(currentLocation);
}

void resetChange(){
    int i, j;
    for(i = 0; i < 8; i++){
        for(j = 0; j< 8; j++){
            change[i][j] = 0;
        }
    }
}

int min(int a, int b){
    return(a > b) ? b : a;
}

int max(int a, int b){
    return(a < b) ? b : a;
}

int isValid(int turn, int mi, int mj){
    int i, j;

    int opponent = (turn + 1) %2;
    if(board[mi][mj] != -1){
        return(0);
    }
    //up
    for(i = mi- 1; board[i][mj]==opponent && i >= 0; i--);
    if(i >= 0 && mi-1!=i){
        if(board[i][mj] == turn){
            printf("1\n");
            return(1);

        }
    }
    //down
    for(i = mi+ 1; board[i][mj]==opponent && i < 8; i++);
    if(i < 8 && mi+1!=i){
        if(board[i][mj] == turn){
            printf("2\n");
            return(1);

        }
    }
    //left
    for(j = mj - 1; board[mi][j]==opponent && j >= 0; j--);
    if(j >= 0 && mj-1!=j){
        if(board[mi][j] == turn){
            printf("3\n");
            return(1);

        }
    }
    //right
    for(j = mj + 1; board[mi][j]==opponent && j < 8; j++);
    if(j < 8 && mj+1!=j){
        if(board[mi][j] == turn){
            printf("4\n");
            return(1);

        }
    }
    //left up diagonal
    for(i = mi-1, j = mj-1; board[i][j]==opponent && i>=0 && j>=0; i--, j--);
    if(i >=0 && j >= 0 && mi-1!=i && mj-1!=j){
        if(board[i][j] == turn){
            printf("5\n");
            return(1);

        }
    }
    //right down diagonal
    for(i = mi+1, j = mj+1; board[i][j]==opponent && i<8 && j<8; i++, j++);
    if(i < 8 && j < 8 && mi+1!=i && mj+1!=j){
        if(board[i][j] == turn){
            printf("6\n");
            return(1);

        }
    }
    //right up diagonal
    for(i = mi-1, j = mj+1; board[i][j]==opponent && i>=0 && j<8; i--, j++);
    if(i >=0 && j < 8 && mi-1!=i && mj+1!=j){
        if(board[i][j] == turn){
            printf("7\n");
            return(1);

        }
    }
    //left down diagonal
    for(i = mi+1, j = mj-1; board[i][j]==opponent && i<8 && j>=0; i++, j--);
    if(i < 8 && j >= 0 && mi+1!=i && mj-1!=j){
        if(board[i][j] == turn){
            printf("8\n");
            return(1);
            printf("%d %d \n", i , j);

        }
    }
    return(0);
}

int canPlay(int turn){
    int i, j;
    int opponent = (turn + 1) %2;
    int right, left, up, down;

    for(i = 0; i < 8; i++){
        for(j = 0; j < 8; j++){
            if(board[i][j] == -1){
                left = max(j-1, 0);
                right = min(j+1, 7);
                up = max(i-1, 0);
                down = min(i+1, 7);

                if(board[i][left] == opponent || board[i][right] == opponent
                   || board[down][j] == opponent || board[up][j] == opponent
                   || board[up][left] == opponent || board[up][right] == opponent
                   || board[down][left] == opponent || board[up][right] == opponent){

                       if(isValid(turn, i, j) == 1){
                           printf("i:%d  j:%d \n", i, j);
                           return(1);

                       }
                   }
            }

        }
    }
    return(0);
}

void sandwich(int turn, int mi, int mj){
    int i, j, k , z;
    int opponent = (turn + 1) %2;
    board[mi][mj] = turn;
    change[mi][mj] = 1;

    //up

    for(i = mi- 1; board[i][mj]==opponent && i >= 0; i--);
    if(i >= 0 && mi-1!=i){
        if(board[i][mj] == turn){
            printf("1\n");

            for(k = i+1; k < mi; k++){
                board[k][mj] = turn;
                change[k][mj] = 1;
            }
        }
    }

    //down
    for(i = mi+ 1; board[i][mj]==opponent && i < 8; i++);
    if(i < 8 && mi+1!=i){
        if(board[i][mj] == turn){
            printf("2\n");

            for(k=i-1; k > mi; k--){
                board[k][mj] = turn;
                change[k][mj] = 1;
            }

        }
    }
    //left
    for(j = mj - 1; board[mi][j]==opponent && j >= 0; j--);
    if(j >= 0 && mj-1!=j){
        if(board[mi][j] == turn){
            printf("3\n");

            for(z=j+1; z<mj; z++){
                board[mi][z] = turn;
                change[mi][z] = 1;
            }

        }
    }
    //right
    for(j = mj + 1; board[mi][j]==opponent && j < 8; j++);
    if(j < 8 && mj+1!=j){
        if(board[mi][j] == turn){
            printf("4\n");

            for(z=j-1; z>mj; z--){
                board[mi][z] = turn;
                change[mi][z] = 1;
            }

        }
    }
    //left up diagonal
    for(i = mi-1, j = mj-1; board[i][j]==opponent && i>=0 && j>=0; i--, j--);
    if(i >=0 && j >= 0 && mi-1!=i && mj-1!=j){
        if(board[i][j] == turn){
            printf("5\n");

            for(k=i+1,z=j+1; k<mi && z<mj; k++,z++){
                board[k][z] = turn;
                change[k][z] = 1;
            }

        }
    }
    //right down diagonal
    for(i = mi+1, j = mj+1; board[i][j]==opponent && i<8 && j<8; i++, j++);
    if(i < 8 && j < 8 && mi+1!=i && mj+1!=j){
        if(board[i][j] == turn){
            printf("6\n");

            for(k=i-1, z=j-1; k>mi && z>mj; k--, z--){
                board[k][z] = turn;
                change[k][z] = 1;
            }

        }
    }
    //right up diagonal
    for(i = mi-1, j = mj+1; board[i][j]==opponent && i>=0 && j<8; i--, j++);
    if(i >=0 && j < 8 && mi-1!=i && mj+1!=j){
        if(board[i][j] == turn){
            printf("7\n");

            for(k=i+1, z=j-1; k<mi && z>mj; k++, z--){
                board[k][z] = turn;
                change[k][z] = 1;
            }

        }
    }

    //left down diagonal
    for(i = mi+1, j = mj-1; board[i][j]==opponent && i<8 && j>=0; i++, j--);
    if(i < 8 && j >= 0 && mi+1!=i && mj-1!=j){
        if(board[i][j] == turn){
            printf("8\n");
            printf("%d %d \n", i , j);

            for(k=i-1, z=j+1; k>mi && z<mj; k--, z++){
                board[k][z] = turn;
                change[k][z] = 1;
            }

        }
    }


}

void resetLED(){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, 0);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 0);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, 0);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 0);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 0);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, 0);

}

int getScore(int player){
    int score = 0;
    int i, j;

    for(i=0; i<8; i++){
        for(j=0; j<8; j++){
            if(board[i][j] == player){
                score++;
            }
        }
    }

    return(score);

}

int getWinner(){

	if(whiteScore > blackScore)
		return 0;
	if(whiteScore < blackScore)
		return 1;

	return -1;
}

void printScore(){
	setCursor(14, 2);
	print(whiteStr);
	setCursor(14, 3);
	print(blackStr);
}

int getY(char c){

    if(c == 'A' || c == 'a')
        return(0);

    else if(c == 'B' || c == 'b')
        return(1);

    else if(c == 'C' || c =='c')
        return(2);

    else if(c == 'D' || c =='d')
        return(3);

    else if(c == 'E' || c =='e')
        return(4);

     else if(c == 'F' || c =='f')
        return(5);

    else if(c == 'G' || c =='g')
        return(6);

    else if(c == 'H' || c =='f')
        return(7);

    return(-1);

}

int getTurn(char c){
    if(c == 'B' || c =='b')
        return 1;
    if(c == 'W' || c == 'w')
        return 0;
    return -1;
}

void number2BCD(int i){
	int x1 = i&1;
	int x2 = i&2;
	int x3 = i&4;
	int x4 = i&8;
	if(x1>0) x1 = 1;
	if(x2>0) x2 = 1;
	if(x3>0) x3 = 1;
	if(x4>0) x4 = 1;

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, x1);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, x2);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, x3);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, x4);
}


void sendBoard(int mode){
    int i, j;
    if(mode ==0)
        uart_Board[0] = 'U';
    else if(mode == 1)
        uart_Board[0] = 'S';

    uart_Board[1] = ' ';
    uart_Board[66] = ' ';
    uart_Board[67] = 'T';
    uart_Board[68] = ' ';
    uart_Board[69] = turn + '0';
    uart_Board[70] = '\n';


    for(i=0; i<8; i++){
        for(j=0; j<8;j++){
            if(board[i][j] != -1)
                uart_Board[(i*8)+j+2] = board[i][j] + '0';
            else
                 uart_Board[(i*8)+j+2] = '2';
        }
    }
}



/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc4;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */
extern unsigned char data[1];
extern TIM_HandleTypeDef htim3;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  if(state == 1){

		  switch(counter % 4){
			  case 0:
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, 1);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, 0);
				  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, 1);
				  number2BCD(time%10);
				  break;


			  case(1):
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7, 0);
				  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, 1);
				  number2BCD((time/10) % 10);
				  break;


			  case 2:
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7, 0);
				  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, 0);
				  number2BCD(remain % 10);
				  break;


			  case 3:
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7 | GPIO_PIN_5 | GPIO_PIN_6, 0);
				  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, 1);
				  number2BCD((remain/10)%10);
				  break;
			  }
		   counter++;

  }
  if(remain<=10 && remain > 0 && state == 1){
		  if(HAL_GetTick() % (remain*50) == 0){
			      PWM_Change_Tone(1568, 100);
			  	  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 100);

		  }
		  else
			  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);




  }
  if(!buzzAlarm && state == 1){
	  if(HAL_GetTick() - effect > 1000){
		  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);

	  }
  }


  if(HAL_GetTick()- invalidStart > 3000){
	  invalidAlarm = 0;
  }
  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F3xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f3xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line0 interrupt.
  */
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */
  if(HAL_GetTick() - lastEx0 >= 500){
	  clear();
	  startBoard();
	  printBoard();
	  printTurn();
	  printLocation();

	  whiteScore = getScore(0);
	  blackScore = getScore(1);

	  sprintf(whiteStr, "W: %d ", whiteScore);
	  sprintf(blackStr, "B: %d ", blackScore);

	  printScore();
	  state = 1;
	  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
	  remain = time;
  }
  lastEx0 = HAL_GetTick();
  /* USER CODE END EXTI0_IRQn 1 */
}

/**
  * @brief This function handles EXTI line1 interrupt.
  */
void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */

  /* USER CODE END EXTI1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
  /* USER CODE BEGIN EXTI1_IRQn 1 */
  if(HAL_GetTick() - lastEx1 >= 500 && state == 1){

	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 0);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 0);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, 0);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, 0);

	  //left

	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 1);
	  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1)){
		  resetLED();
		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
		  y = max(0, y-1);
		  printLocation();
	  }




	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 1);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 1);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, 1);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, 1);

  }
  lastEx1 = HAL_GetTick();
  /* USER CODE END EXTI1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line2 and Touch Sense controller.
  */
void EXTI2_TSC_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_TSC_IRQn 0 */

  /* USER CODE END EXTI2_TSC_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
  /* USER CODE BEGIN EXTI2_TSC_IRQn 1 */
  if(HAL_GetTick() - lastEx2 >= 500  && state == 1){



   	  for(int i =0; i<2; i++){

   	   	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 0);
   	   	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 0);
   	   	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, 0);
   	   	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, 0);

   		  //up
   		  if(i == 0){
   		   	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, 1);
   		   	  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2)){
   		   		  resetLED();
   		   		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 1);
   		   		  x = max(0, x-1);
   		   		  printLocation();
   		   	  }
   		  }
   		  //down
   		  else if(i == 1){
   		   	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 1);
   		   	  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2)){
   		   		  resetLED();
   		   		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, 1);
   		   		  x = min(x+1, 7);
   		   		  printLocation();
   		   	  }
   		  }

   	   	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 1);
   	   	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 1);
   	   	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, 1);
   	   	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, 1);

   	  }
  }
     lastEx2 = HAL_GetTick();
  /* USER CODE END EXTI2_TSC_IRQn 1 */
}

/**
  * @brief This function handles EXTI line3 interrupt.
  */
void EXTI3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI3_IRQn 0 */

  /* USER CODE END EXTI3_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
  /* USER CODE BEGIN EXTI3_IRQn 1 */
  if(HAL_GetTick() - lastEx3 >= 500  && state == 1){

 	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 0);
 	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 0);
 	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, 0);
 	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, 0);

 	  //right

 	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 1);
 	  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3)){
 		  resetLED();
 		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 1);
 		  y = min(y+1, 7);
 		  printLocation();
 	  }



 	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 1);
 	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 1);
 	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, 1);
 	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, 1);

   }
   lastEx3 = HAL_GetTick();
  /* USER CODE END EXTI3_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */


  if(invalidAlarm == 1){
  	  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_8);
  	  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);
  	  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_10);
  	  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_11);
  	  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12);
  	  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
  	  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_14);
  	  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_15);

  }

  if(state == 0){
	  if(clock%6 ==0){
		  PWM_Change_Tone(1568, 80);
		  setCursor(5,0);
		   write(5);
		   setCursor(6,0);
		   write(4);

		   setCursor(19, 0);
		   write(7);

		   setCursor(0, 0);
		   write(7);

		   setCursor(18, 2);
		   write(6);

		   setCursor(1, 2);
		   write(6);



	  }
	  else if(clock%6 == 1){
		  PWM_Change_Tone(1046, 80);
		  setCursor(5,0);
		   write(4);

		   setCursor(6,0);
		   write(5);

		   setCursor(19, 1);
		   write(7);

		   setCursor(0, 1);
		   write(7);

		   setCursor(18, 1);
		   write(6);

		   setCursor(1, 1);
		   write(6);


	  }
	  else if(clock %6 ==2){

		  PWM_Change_Tone(1174, 50);
		  setCursor(5,0);
		   write(5);
		   setCursor(6,0);
		   write(4);

		   setCursor(19, 2);
		   write(7);

		   setCursor(0, 2);
		   write(7);

		   setCursor(18, 0);
		   write(6);

		   setCursor(1, 0);
		   write(6);

	  }
	  else if(clock % 6 ==3){
		  PWM_Change_Tone(1318, 50);
		  setCursor(5,0);
		   write(4);

		   setCursor(6,0);
		   write(5);

		   setCursor(19, 0);
//		   write(6);
		   print(" ");

		   setCursor(0, 0);
//		   write(7);
		   print(" ");

		   setCursor(18, 2);
		   print(" ");

		   setCursor(1, 2);
		   print(" ");

	  }

	  else if(clock%6 == 4){
		  PWM_Change_Tone(1174, 50);
		  setCursor(5,0);
		   write(5);

		   setCursor(6,0);
		   write(4);

		   setCursor(19, 1);
//		   write(6);
		   print(" ");

		   setCursor(0, 1);
//		   write(7);
		   print(" ");

		   setCursor(18, 1);
		   print(" ");

		   setCursor(1, 1);
		   print(" ");

	  }
	  else if(clock %6==5){

		  PWM_Change_Tone(1046, 50);
		  setCursor(5,0);
		   write(4);
		   setCursor(6,0);
		   write(5);

		   setCursor(19, 2);
//		   write(6);
		   print(" ");

		   setCursor(0, 2);
//		   write(7);
		   print(" ");

		   setCursor(18, 0);
		   print(" ");

		   setCursor(1, 0);
		   print(" ");

	  }


  }

  if(state == 2){

	  if(winner != -1){
		  if(clock % 2 == 0){
			  for(int i=0; i<20; i++){
				  setCursor(i, 0);
				  write(winner);
			  }

			  for(int j=1; j<3; j++){
				  setCursor(19, j);
				  write(winner);
			  }

			  for(int i=19; i>=0; i--){
				  setCursor(i, 3);
				  write(winner);
			  }

			  for(int j=2; j>0; j--){
				  setCursor(0, j);
				  write(winner);
			  }

			  setCursor(8, 1);
			  print(whiteStr);
			  setCursor(8, 2);
			  print(blackStr);
		  }

		  else{

			  for(int i=0; i<20; i++){
				  setCursor(i, 0);
				  print(" ");
			  }

			  for(int j=1; j<3; j++){
				  setCursor(19, j);
				  print(" ");
			  }

			  for(int i=19; i>=0; i--){
				  setCursor(i, 3);
				  print(" ");
			  }

			  for(int j=2; j>0; j--){
				  setCursor(0, j);
				  print(" ");
			  }

			  if(winner == 0)
				  setCursor(8, 1);

			  else
				  setCursor(8, 2);

			  print("     ");
		  }

	  }
	  else{
		  if(clock % 2 == 0){
			  for(int i=0; i<20; i++){
				  setCursor(i, 0);
				  write(0);
			  }

			  for(int j=1; j<3; j++){
				  setCursor(19, j);
				  write(0);
			  }

			  for(int i=19; i>=0; i--){
				  setCursor(i, 3);
				  write(0);
			  }

			  for(int j=2; j>0; j--){
				  setCursor(0, j);
				  write(0);
			  }

			  setCursor(8, 1);
			  print(whiteStr);
			  setCursor(8, 2);
			  print(blackStr);
		  }

		  else{

			  for(int i=0; i<20; i++){
				  setCursor(i, 0);
				  write(1);
			  }

			  for(int j=1; j<3; j++){
				  setCursor(19, j);
				  write(1);
			  }

			  for(int i=19; i>=0; i--){
				  setCursor(i, 3);
				  write(1);
			  }

			  for(int j=2; j>0; j--){
				  setCursor(0, j);
				  write(1);
			  }

		  }

	  }

	  switch(clock%4){
	 	  case 0:
	 		  PWM_Change_Tone(1568, 40);
	 		  break;
	 	  case 1:
	 		  PWM_Change_Tone(1318, 40);
	 		  break;
	 	  case 2:
	 		  PWM_Change_Tone(1174, 40);
	 		  break;
	 	  case 3:
	 		  PWM_Change_Tone(1046, 40);
	 		  break;
	 	  }


  }

  clock = clock + 1;

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */

  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */
  if(state == 1){
	  remain = remain - 1;
	  if(remain == 0){
		  buzzAlarm = 0;
		  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
		  state = 2;
		  winner = (turn+1) % 2;
		  clear();
		  blackScore = getScore(1);
		  whiteScore = getScore(0);

		  createChar(0, white);
		  createChar(1, black);

		  sprintf(whiteStr, "W: %d ", whiteScore);
		  sprintf(blackStr, "B: %d ", blackScore);

		  switch(winner){
		  case 0:
			  sprintf(uartRes, "R White win\n");
			  break;
		  case 1:
			  sprintf(uartRes, "R Black win\n");
			  break;
		  case -1:
			  sprintf(uartRes, "R Tie!!!! 0\n");
			  break;
		  }
		  HAL_UART_Transmit(&huart2, uartRes, sizeof(uartRes), 1000);

	  }
	  if(remain == 10){
		  buzzAlarm = 1;
//		  PWM_Change_Tone(100000, 100);

	  }



  }

  /* USER CODE END TIM4_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
  if(data[0] != '\n'){

  	  buffer[pos] = data[0];
  	  buffer[pos + 1] = '\0';
  	  pos = pos + 1;
    }
  else if(data[0] == '\n'){
//	  uart player
	  if(buffer[0] != 'U'){

		  uy = getY(buffer[0]);
		  ux = buffer[1] - '0' - 1;
		  t = getTurn(buffer[3]);

		  if(t == turn && uy >= 0 && uy <8 && ux >= 0 && ux <8){

			  if(isValid(turn, ux, uy)){
				  	  	  sprintf(response, "V %c%c%c%c\n" ,buffer[0], buffer[1], buffer[2], buffer[3]);
						  HAL_UART_Transmit(&huart2, response, sizeof(response), 1000);

						  x = ux;
						  y = uy;
						  printLocation();

			  			  sandwich(turn, x, y);

			  			  whiteScore = getScore(0);
			  			  blackScore = getScore(1);

			  			  sprintf(whiteStr, "W: %d ", whiteScore);
			  			  sprintf(blackStr, "B: %d ", blackScore);
			  			  printScore();
			  			  printBoard();

			  			  remain = time;


			  			  if(canPlay((turn+1)%2)){
			  				  turn = (turn +1) % 2;
			  				  printTurn();
							  sendBoard(0);
							  HAL_UART_Transmit(&huart2, uart_Board, sizeof(uart_Board), 1000);
			  			  }
			  			  //end
			  			  else if(canPlay(turn) == 0){
			  					  state = 2;
			  					  clear();
			  					  blackScore = getScore(1);
			  					  whiteScore = getScore(0);
			  					  winner = getWinner();
			  					  createChar(0, white);
			  					  createChar(1, black);

			  					  sprintf(whiteStr, "W: %d ", whiteScore);
			  					  sprintf(blackStr, "B: %d ", blackScore);

			  					  switch(winner){
			  					  case 0:
			  						  sprintf(uartRes, "R White win\n");
			  						  break;
			  					  case 1:
			  						  sprintf(uartRes, "R Black win\n");
			  						  break;
			  					  case -1:
			  						  sprintf(uartRes, "R Tie!!!! 0\n");
			  						  break;
			  					  }
			  					  HAL_UART_Transmit(&huart2, uartRes, sizeof(uartRes), 1000);

			  			  }
			  }

			  else{
				  resetLED();
				  invalidAlarm = 1;
				  invalidStart = HAL_GetTick();
		  	  	  sprintf(response, "I %c%c%c%c\n" ,buffer[0], buffer[1], buffer[2], buffer[3]);
				  HAL_UART_Transmit(&huart2, response, sizeof(response), 1000);
			  }
		  }
		  else{
	  	  	  sprintf(response, "I %c%c%c%c\n" ,buffer[0], buffer[1], buffer[2], buffer[3]);
			  HAL_UART_Transmit(&huart2, response, sizeof(response), 1000);
		  }
	  }
	  pos = 0;
  }


  HAL_UART_Receive_IT(&huart2, data, sizeof(data));
  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */
  if(HAL_GetTick() - lastEx12 >= 500 && state == 1){

	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 0);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 0);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, 0);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, 0);

	  //select

	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 1);
	  if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12)){
		  resetLED();
		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
		  if(isValid(turn, x, y)){
			  buzzAlarm = 0;
			  sandwich(turn, x, y);

			  whiteScore = getScore(0);
			  blackScore = getScore(1);

			  sprintf(whiteStr, "W: %d ", whiteScore);
			  sprintf(blackStr, "B: %d ", blackScore);
			  printScore();
			  printBoard();

			  remain = time;




			  if(!buzzAlarm){
				  effect = HAL_GetTick();
				  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 70);
			  }

			  if(canPlay((turn+1)%2)){
				  turn = (turn +1) % 2;
				  printTurn();
				  sendBoard(0);
				  HAL_UART_Transmit(&huart2, uart_Board, sizeof(uart_Board), 1000);
			  }
			  //end
			  else if(canPlay(turn) == 0){
					  state = 2;
					  clear();
					  blackScore = getScore(1);
					  whiteScore = getScore(0);
					  winner = getWinner();
					  createChar(0, white);
					  createChar(1, black);

					  sprintf(whiteStr, "W: %d ", whiteScore);
					  sprintf(blackStr, "B: %d ", blackScore);

					  switch(winner){
					  case 0:
						  sprintf(uartRes, "R White win\n");
						  break;
					  case 1:
						  sprintf(uartRes, "R Black win\n");
						  break;
					  case -1:
						  sprintf(uartRes, "R Tie!!!! 0\n");
						  break;
					  }
					  HAL_UART_Transmit(&huart2, uartRes, sizeof(uartRes), 1000);

			  }

		  }
		  else{
			  resetLED();
			  invalidAlarm = 1;
			  invalidStart = HAL_GetTick();
		  }
	  }

	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 1);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 1);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, 1);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, 1);

  }

   lastEx12 = HAL_GetTick();

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles ADC4 interrupt.
  */
void ADC4_IRQHandler(void)
{
  /* USER CODE BEGIN ADC4_IRQn 0 */

  /* USER CODE END ADC4_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc4);
  /* USER CODE BEGIN ADC4_IRQn 1 */
  volume = HAL_ADC_GetValue(&hadc4);
  time = (volume*50/4095) + 10;
  HAL_ADC_Start_IT(&hadc4);
  /* USER CODE END ADC4_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
