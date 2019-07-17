#include <lpc17xx.h>
#include "stdio.h"
#include "uart.h"
#include "GLCD.h"
#include <cmsis_os2.h>
#include "random.h"
#include "lfsr113.h"
#include <stdbool.h>

#define MAX_ROW_LCD 9 //Max number of Rows for LCD
#define MAX_COL_LCD 19 //Max number of Columns for LCD

//Global Variables
//Invader Position
struct player {
	uint8_t player1; //Status to track if player 1 or 2 is controlling character
	uint8_t score;
	uint16_t x;
	uint16_t y;
};

typedef struct player Player;

Player invader;
Player king;

//Game status, (play/stop)
uint8_t gameStatus;

//Reload status for King's shooting
osMutexId_t reloading;

//Screen boundaries => 320x240 pixels
const uint16_t WIDTH_PX = 319;
const uint16_t HEIGHT_PX = 239;

//Task Regulating the Invader's movement
void invaderMovement(void *arg) {
	char graphic = 'O';
	
	while(1){
		//Clear invader's position
		GLCD_DisplayChar(invader.y, invader.x, 1, ' ');

		//Joystick left
		if (!(LPC_GPIO1->FIOPIN & (1<<23)) && invader.y>0) {
			invader.y = invader.y-1;
			graphic = 'U';	
		}
		//Joystick Right
		else if (!(LPC_GPIO1->FIOPIN & (1<<24)) && invader.x<MAX_COL_LCD) {
			invader.x = invader.x+1;
			graphic = 'R';			
		}
		//Joystick Down
		else if (!(LPC_GPIO1->FIOPIN & (1<<25)) && invader.y<MAX_ROW_LCD) {
			invader.y = invader.y+1;
			graphic = 'D';
			
		}
		//Joystick Left
		else if (!(LPC_GPIO1->FIOPIN & (1<<26)) && invader.x>0) {
			invader.x = invader.x-1;
			graphic = 'L';
		}

		GLCD_DisplayChar(invader.y, invader.x, 1, graphic);

		//Joystick pressed
		if (!(LPC_GPIO1->FIOPIN & (1<<20))) {

		}

		osDelay(100);

		//Check if invader hit right border (game won)
		if(invader.x == MAX_COL_LCD) {
			GLCD_DisplayChar(invader.y, invader.x, 1, ' ');
			invader.x = 0;
			invader.y = MAX_ROW_LCD/2;
			
			invader.score = invader.score + 1;
		}
	}
}

//Task Regulating the King's movement
void kingMovement(void *arg) {
	uint16_t move = -1;
	
	//Populate border on right side
	for(uint8_t border = 0; border <= MAX_ROW_LCD; border++) {
		GLCD_DisplayChar(border, MAX_COL_LCD, 1, '|');
	}
	
	while(1) {
		if(king.y == 0) {
			move = 1;
		}
		else if(king.y == MAX_ROW_LCD) {
			move = -1;
		}
		
		//Clear old position
		GLCD_DisplayChar(king.y, MAX_COL_LCD, 1, '|');
		
		king.y = king.y + move;
		
		//Move King up and down right side of screen at constant speed
		GLCD_DisplayChar(king.y, MAX_COL_LCD, 1, 'K');
	
		osDelay(100);
	}
}

//Task Regulating the King's reload time
void kingReload(void *arg) {
	//LEDs 1, 2, 3 on bits 28, 29, 31 on GPIO1
	LPC_GPIO1->FIODIR |= 0xB0000000;
	//LEDs 4, 5, 6, 7, 8 on bits 2, 3, 4, 5, 6 on GPIO2
	LPC_GPIO2->FIODIR |= 0x0000007C;

	while(1){
		osDelay(100);
		osMutexAcquire(reloading, osWaitForever);
		LPC_GPIO1->FIOCLR |= 0xB0000000;
		LPC_GPIO2->FIOCLR |= 0x0000007C;

		for(uint8_t led = 0; led<8; led++) {
			//Turn LED On GPIO1
			if(led == 5) {
				LPC_GPIO1->FIOSET |= (1<<31);
			}
			else if(led==6) {
				LPC_GPIO1->FIOSET |= (1<<29);
			}
			else if(led==7) {
				LPC_GPIO1->FIOSET |= (1<<28);
			}

			//Turn LED On GPIO2
			else {
				LPC_GPIO2->FIOSET |= (1<<6-led);
			}

			osDelay(200);
		}
		osMutexRelease(reloading);
	}
}

void shot(uint16_t y) {
	printf("SHOT FIRED");
	
	GLCD_DisplayChar(y, MAX_COL_LCD, 1, '|');
	
	for(int16_t position = MAX_COL_LCD-1;position>=0; position--) {
		
		//Invader hit
		if(position == invader.x) {
			invader.x = 0;
			invader.y = MAX_ROW_LCD/2;
			king.score = king.score + 1;
		}
		
		GLCD_DisplayChar(y, position, 1, '-');
		osDelay(50);
		GLCD_DisplayChar(y, position, 1, ' ');
		
		printf("%d\n", position);
	}
}

//Task Regulating the King's shot
void kingShot(void *arg) {
	while(1){
		//osMutexAcquire(reloading, osWaitForever);

		//Pushbutton pressed -- shot fired
		//Also check if reload time is up
		if(!(LPC_GPIO2->FIOPIN & (1<<10))) {
			//osMutexRelease(reloading);
			printf("shooting");
			shot(king.y);
		}
		printf("looping");
	}
}


//Reference code to manipulate GLCD display
/*void display(void *arg) {
	GLCD_Init();
	while(1) {
		//GLCD Screen Code
		//Screen is 320x240
		//Should be able to use WIDTH and HEIGHT to access GLCD Screen size
		GLCD_Clear(White);
		GLCD_SetBackColor(White);   
		GLCD_SetTextColor(Black);
		
		GLCD_DisplayString(1, 0, 1, "sent");
		
		char print_msg [8];
		uint32_t number = 6;
		sprintf(print_msg, "%d", number);
		GLCD_DisplayString(1, 5, 1, print_msg);

		
		//fi: font size, 0-> 6x8, 1 -> 16x24
		GLCD_Init(void);
		GLCD_PutPixel(unsigned int x, unsigned int y);
		GLCD_SetTextColor(unsigned short color);
		GLCD_SetBackColor(unsigned short color);
		GLCD_Clear(unsigned short color);
		GLCD_DrawChar(unsigned int x,  unsigned int y, unsigned int cw, unsigned int ch, unsigned char *c);
		GLCD_DisplayChar(unsigned int ln, unsigned int col,unsigned char fi, unsigned char c);
		GLCD_DisplayString(unsigned int ln, unsigned int col,unsigned char fi, unsigned char *s);
		GLCD_ClearLn(unsigned int ln, unsigned char fi);
		GLCD_Bargraph(unsigned int x, unsigned int y,unsigned int w, unsigned int h, unsigned int val);
		GLCD_Bitmap(unsigned int x, unsigned int y,unsigned int w, unsigned int h, unsigned char *bitmap);
		
	}
}*/

int main(void){
	printf("Start of King of the LCD\n");

	GLCD_Init();
	GLCD_Clear(Black);
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(Green);
	
	gameStatus = true;
	reloading = false;
	
	invader.player1 = 1;
	invader.score = 0;
	invader.x = 0;
	invader.y = MAX_ROW_LCD/2;
	
	king.player1 = 0;
	king.score = 0;
	king.x = MAX_COL_LCD;
	king.y = MAX_ROW_LCD/2;
	
	//Start four threads
	osKernelInitialize();
	
	osThreadNew(invaderMovement, NULL, NULL);
	osThreadNew(kingMovement, NULL, NULL);
	osThreadNew(kingReload, NULL, NULL);
	osThreadNew(kingShot, NULL, NULL);
	//osThreadNew(display, NULL, NULL);
	osKernelStart();
	for( ; ; ) {}
}



//Additional Potentiometer code if needed
/*
void potentiometer(void const *arg) {
	LPC_PINCON->PINSEL1 &= ~(0x03<<18);
	LPC_PINCON->PINSEL1 |= (0x01<<18);
	LPC_SC->PCONP |= 0x00001000;
	LPC_ADC->ADCR = (1<<2)|(4<<8)|(1<<21);
	LPC_ADC->ADCR |= (1<<24);
	
	while(1)
	{
		if (LPC_ADC->ADGDR & 0x80000000) {
			printf("%d\n", (LPC_ADC->ADGDR & (0x0FFF<<4)) >> 4);
			LPC_ADC->ADCR |= (1<<24);
		}
	}
}
*/
