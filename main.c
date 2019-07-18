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

unsigned char kingSprite[] ={
0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFE, 0xD636, 0xCDF5, 0xFFFD, 0xFFFE, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0010 (16) pixels
0xFFDE, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFE, 0xCD93, 0x7AE7, 0x8B68, 0xC592, 0xFFDD, 0xFFFF, 0xFFFE, 0xFFFE, 0xFFFF, 0xFFDE,   // 0x0020 (32) pixels
0xF75C, 0x8C2F, 0xE6B8, 0xFFDE, 0xFFFF, 0xE6D9, 0x9368, 0xA345, 0xB3E5, 0xAC28, 0xDEB8, 0xFFFF, 0xFFFD, 0xE6D8, 0x944E, 0xEF3A,   // 0x0030 (48) pixels
0xFFFE, 0x940E, 0x8B49, 0x9BAA, 0xEEB8, 0xE614, 0xA304, 0xAB22, 0xBBC2, 0xB404, 0xDDF2, 0xF6F7, 0xA42A, 0xA42A, 0x944D, 0xFFFE,   // 0x0040 (64) pixels
0xFFFF, 0xCDB4, 0xB3C9, 0xAB24, 0xA305, 0xAB45, 0xAAE2, 0xBB41, 0xC3E2, 0xBBC1, 0xBC24, 0xABC4, 0xBC03, 0xCD09, 0xC5B2, 0xFFFF,   // 0x0050 (80) pixels
0xFFFF, 0xF6F9, 0xAB46, 0xE489, 0xDC48, 0xDC47, 0xDC67, 0xE466, 0xF547, 0xF587, 0xF567, 0xED67, 0xF5A7, 0xBC45, 0xEED6, 0xFFFF,   // 0x0060 (96) pixels
0xFFFF, 0xFFDD, 0xA3A9, 0xA304, 0xAB24, 0xB303, 0xAB03, 0xB322, 0xBBC2, 0xC3E2, 0xC3E2, 0xC403, 0xBC04, 0xAC07, 0xFFBB, 0xFFFF,   // 0x0070 (112) pixels
0xFFDF, 0xFFFE, 0xAC4D, 0x9327, 0xAB87, 0xAB46, 0xA345, 0xAB45, 0xBC06, 0xBC26, 0xBC26, 0xBC67, 0xAC28, 0xAC6C, 0xFFFD, 0xFFFF,   // 0x0080 (128) pixels
0xFFFF, 0xFFFF, 0x732B, 0x5A26, 0x9C0C, 0xBCCF, 0xCD2F, 0xD570, 0xD56F, 0xCD2E, 0xBCEE, 0xA42C, 0x6245, 0x7309, 0xFFDE, 0xFFFF,   // 0x0090 (144) pixels
0xFFFF, 0x8C51, 0x3985, 0x7B6C, 0x6247, 0x6245, 0x6A45, 0xB48E, 0xBCCE, 0x6A45, 0x6246, 0x5A46, 0x734B, 0x39A5, 0x8410, 0xFFFF,   // 0x00A0 (160) pixels
0xFFFF, 0x6B4D, 0x5269, 0x8BCE, 0xCD92, 0x8307, 0x9389, 0xB4AD, 0xBCCE, 0x9389, 0x8308, 0xC551, 0x8C0E, 0x5269, 0x630C, 0xFFFF,   // 0x00B0 (176) pixels
0xA535, 0x4A28, 0x62AA, 0x6AC8, 0xEE54, 0xC50E, 0xBCCE, 0xF695, 0xF675, 0xC4EE, 0xC4EE, 0xEE75, 0x72E9, 0x5AA9, 0x4A28, 0x9CF4,   // 0x00C0 (192) pixels
0x9D14, 0x41C7, 0x7309, 0xEE75, 0xEE12, 0xE5D1, 0xD550, 0xBCCE, 0xC4CE, 0xD550, 0xE5D2, 0xEE13, 0xEE75, 0x7B4A, 0x39C6, 0x94D3,   // 0x00D0 (208) pixels
0x9CF4, 0x2945, 0x6288, 0xC551, 0xDDB1, 0xFE94, 0xB48D, 0xBCCE, 0xBCEF, 0xAC4C, 0xFE94, 0xDDB1, 0xCD51, 0x6AA8, 0x2944, 0x94B3,   // 0x00E0 (224) pixels
0xC618, 0x6B6D, 0x5289, 0x49E6, 0xC531, 0xEE55, 0x9BEC, 0xB4F0, 0xB4F0, 0x93CC, 0xEE54, 0xCD71, 0x49E6, 0x5289, 0x6B4D, 0xBE18,   // 0x00F0 (240) pixels
0xFFFF, 0x9D14, 0x4A6A, 0x5AEB, 0x5206, 0x6267, 0x5A67, 0x41C6, 0x41C6, 0x5A47, 0x6267, 0x5226, 0x5ACB, 0x4A8A, 0x94D3, 0xFFFF,   // 0x0100 (256) pixels
0xEF7D, 0x6B6E, 0x31C8, 0x6B6E, 0x528A, 0x4208, 0x4229, 0x4208, 0x3A08, 0x4229, 0x39E8, 0x4A8A, 0x6B8E, 0x31E8, 0x634D, 0xEF7D,   // 0x0110 (272) pixels
0xD69A, 0x94B3, 0x5AEC, 0x52EC, 0x636E, 0x5B4E, 0x5B4E, 0x5B0D, 0x530D, 0x5B4E, 0x5B4E, 0x634E, 0x530D, 0x52CC, 0x94B2, 0xCE79,   // 0x0120 (288) pixels
0xFFFF, 0xFFFF, 0x6B8E, 0x2987, 0x636E, 0x5B4E, 0x634E, 0x634E, 0x634E, 0x5B4E, 0x5B4E, 0x636F, 0x29A7, 0x632D, 0xFFFF, 0xFFFF,   // 0x0130 (304) pixels
0xFFFF, 0xC618, 0x52AB, 0x52AB, 0x4A8B, 0x5B4E, 0x4A8B, 0x5B4E, 0x5B4E, 0x4AAB, 0x5B2E, 0x4A8B, 0x4A8A, 0x528A, 0xBDF7, 0xFFFF,   // 0x0140 (320) pixels
0xFFFF, 0xEF7D, 0xF7BE, 0xEF5D, 0x6B8E, 0x4A6A, 0x31E8, 0x5B4E, 0x634E, 0x31E8, 0x4A6A, 0x634D, 0xE71C, 0xF7BF, 0xEF7D, 0xFFFF,   // 0x0150 (336) pixels
0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD76, 0x3187, 0x4A8A, 0x4A6A, 0x31A7, 0xA535, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0160 (352) pixels
0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFDF, 0xE71C, 0x7BCF, 0x7BCF, 0xE73D, 0xFFDF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0170 (368) pixels
0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0180 (384) pixels
};

//Global Variables
//Invader Position
struct player {
	uint16_t player; //Status to track if player 1 or 2 is controlling character
	uint16_t score;
	uint16_t x;
	uint16_t y;
};

typedef struct player Player;

Player invader;
Player king;

//Game status, (play/stop)
uint8_t gameStatus;
uint8_t invaderWin;

//Reload status for King's shooting
uint16_t reloading;

//Screen boundaries => 320x240 pixels
const uint16_t WIDTH_PX = 319;
const uint16_t HEIGHT_PX = 239;

/*
TODO
- Bitmap
- Glitching

- Reload with mutexes instead
*/

void switchPlayers(void *arg) {
	while(1) {
		if(gameStatus == 0) {
			if(invaderWin) {
				GLCD_Clear(Green);
				GLCD_Clear(White);
				invader.score = invader.score + 1;
			}
			else {
				GLCD_Clear(Red);
				GLCD_Clear(White);
				king.score = king.score + 1;
			}
			
			if(invader.score != 0 & king.score !=0)
			{	
				GLCD_DisplayString(5, 2, 1, "SWITCH CONTROLS");
				while((LPC_GPIO2->FIOPIN & (1<<10)))
				{}
			}
			
			//Switch player scores
			uint16_t invaderScore = invader.score;
			invader.score = king.score;
			king.score = invaderScore;
			
			if(invader.player == 1) {
				invader.player = 2;
				king.player = 1;
			}
			else {
				invader.player = 1;
				king.player = 2;
			}
			
			invader.x = 0;
			invader.y = MAX_ROW_LCD/2 + 1;
			
			if(invader.player == 1) {
				GLCD_DisplayString(0, 0, 1, "P1 -");
				GLCD_DisplayString(0, 12, 1, "P2 -");
			}
			else {
				GLCD_DisplayString(0, 0, 1, "P2 -");
				GLCD_DisplayString(0, 12, 1, "P1 -");
			}
			
			char num [8];
			sprintf(num, "%d", invader.score);
			GLCD_DisplayString(0, 5, 1, num);
			sprintf(num, "%d", king.score);
			GLCD_DisplayString(0, 17, 1, num);
			
			//Populate border on right side
			for(uint8_t border = 1; border <= MAX_ROW_LCD; border++) {
				GLCD_DisplayChar(border, MAX_COL_LCD, 1, '|');
			}
			osDelay(1000);
			
			//Clear SWITCH CONTROLS line
			GLCD_DisplayString(5, 2, 1, "               ");
			
			//GLCD_Bitmap(50, 50, 16, 24, kingSprite);
			
			gameStatus = 1;
		}
	}
}

//Task Regulating the Invader's movement
void invaderMovement(void *arg) {
	char graphic = 'O';
	uint16_t prevX = 0;
	uint16_t prevY =  MAX_ROW_LCD/2;
	
	//initialize player scores
	gameStatus = 0;
	
	while(1){
		while(gameStatus) {
			//Clear prev space
			prevX = invader.y;
			prevX = invader.x;
			
			//Joystick Up
			if (!(LPC_GPIO1->FIOPIN & (1<<23)) && invader.y>1) {
				GLCD_DisplayChar(invader.y, invader.x, 1, ' '); //Clear invader's position
				invader.y = invader.y-1;
			}
			//Joystick Right
			else if (!(LPC_GPIO1->FIOPIN & (1<<24)) && invader.x<MAX_COL_LCD) {
				osDelay(200);
				GLCD_DisplayChar(invader.y, invader.x, 1, ' '); //Clear invader's position
				invader.x = invader.x+1;	
			}
			//Joystick Down
			else if (!(LPC_GPIO1->FIOPIN & (1<<25)) && invader.y<MAX_ROW_LCD) {
				GLCD_DisplayChar(invader.y, invader.x, 1, ' '); //Clear invader's position
				invader.y = invader.y+1;
				
			}
			//Joystick Left
			else if (!(LPC_GPIO1->FIOPIN & (1<<26)) && invader.x>0) {
				osDelay(200);
				GLCD_DisplayChar(invader.y, invader.x, 1, ' '); //Clear invader's position
				invader.x = invader.x-1;
			}

			GLCD_DisplayChar(invader.y, invader.x, 1, graphic);

			//Joystick pressed
			if (!(LPC_GPIO1->FIOPIN & (1<<20))) {

			}

			osDelay(100);

			//Check if invader hit right border (game won)
			if(invader.x == MAX_COL_LCD) {
				GLCD_DisplayChar(invader.y, invader.x, 1, ' ');
				gameStatus = 0;
				invaderWin = 1;
			}
			GLCD_SetTextColor(White);
			GLCD_DisplayChar(prevY, prevX, 1, ' ');
			GLCD_SetTextColor(Black);
			
		}
	}
}

//Task Regulating the King's movement
void kingMovement(void *arg) {
	
	//Potentiometer setup
	LPC_PINCON->PINSEL1 &= ~(0x03<<18);
	LPC_PINCON->PINSEL1 |= (0x01<<18);
	LPC_SC->PCONP |= 0x00001000;
	LPC_ADC->ADCR = (1<<2)|(4<<8)|(1<<21);
	LPC_ADC->ADCR |= (1<<24);
	
	uint16_t move = -1;
	
	while(1) {
		while(gameStatus){
			
			if(king.y == 1) {
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
			
		
			int16_t ticks = 100;
			
			//Get tick delay from potentiometer
			if (LPC_ADC->ADGDR & 0x80000000) {
				int16_t pot = (LPC_ADC->ADGDR & (0x0FFF<<4)) >> 4;
				ticks = 200 * pot/4096 + 50;
				LPC_ADC->ADCR |= (1<<24);
			}
			
			osDelay(ticks);
		}
	}
}

//Task Regulating the King's reload time
void kingReload(void *arg) {
	//LEDs 1, 2, 3 on bits 28, 29, 31 on GPIO1
	LPC_GPIO1->FIODIR |= 0xB0000000;
	//LEDs 4, 5, 6, 7, 8 on bits 2, 3, 4, 5, 6 on GPIO2
	LPC_GPIO2->FIODIR |= 0x0000007C;
	
	LPC_GPIO1->FIOSET |= 0xB0000000;
	LPC_GPIO2->FIOSET |= 0x0000007C;
	

	while(1){
		while(gameStatus) {
			for(uint8_t led = 0; led<9 && reloading == 1; led++) {
				//Clear all LEDs
				if(led==0){
					LPC_GPIO1->FIOCLR |= 0xB0000000;
					LPC_GPIO2->FIOCLR |= 0x0000007C;
				}
				
				//Turn LED On GPIO1
				else if(led == 6) {
					LPC_GPIO1->FIOSET |= (1<<31);
				}
				else if(led==7) {
					LPC_GPIO1->FIOSET |= (1<<29);
				}
				else if(led==8) {
					LPC_GPIO1->FIOSET |= (1<<28);
				}

				//Turn LED On GPIO2
				else {
					LPC_GPIO2->FIOSET |= (1<<7-led);
				}

				osDelay(200);
			}
			reloading = 0;
		}
	}
}

void shot(uint16_t y) {
	GLCD_DisplayChar(y, MAX_COL_LCD, 1, '|');
	int16_t currentTime = 0;
	
	for(int16_t position = MAX_COL_LCD-1;position>=0 && gameStatus; position--) {
		//Shoot a row of shots
		for(uint16_t shotRow = 1; shotRow<=MAX_ROW_LCD; shotRow++) {
			if(position == invader.x && y != invader.y) {
				GLCD_DisplayChar(invader.y, invader.x, 1, ' ');
				invaderWin = 0;
				gameStatus = 0;
			}
			else if(shotRow != y) {
				GLCD_DisplayChar(shotRow, position, 1, '|');
			}
		}
		osDelay(50);
		
		//Clear shot row and prev row
		GLCD_SetTextColor(White);
		for(uint16_t shotRow = 1; shotRow<=MAX_ROW_LCD; shotRow++) {
			if(shotRow != y) {
				GLCD_DisplayChar(shotRow, position, 1, ' ');
			}
		}
		//Clear prev row, reducing leftover glitches
		if(position+1<MAX_COL_LCD) {
			for(uint16_t shotRow = 1; shotRow<=MAX_ROW_LCD; shotRow++) {
				if(shotRow != y) {
					GLCD_DisplayChar(shotRow, position+1, 1, ' ');
				}
			}
		}
		GLCD_SetTextColor(Black);
	}
}

//Task Regulating the King's shot
void kingShot(void *arg) {
	while(1){
		while(gameStatus) {

			//Pushbutton pressed -- shot fired
			//Also check if reload time is up
			if(!(LPC_GPIO2->FIOPIN & (1<<10)) && reloading == 0) {
				reloading = 1;
				shot(king.y);
			}
		}
	}
}
			
int main(void){
	printf("Start of King of the LCD\n");

	GLCD_Init();
	GLCD_Clear(White);
	GLCD_SetBackColor(White);
	GLCD_SetTextColor(Black);
	
	gameStatus = 1;
	reloading = 0;
	invaderWin =1;
	
	invader.player = 2;
	invader.score = -1;
	invader.x = 0;
	invader.y = MAX_ROW_LCD/2 + 1;
	
	king.player = 1;
	king.score = 0;
	king.x = MAX_COL_LCD;
	king.y = MAX_ROW_LCD/2 + 1;
	
	//Start four threads
	osKernelInitialize();
	
	GLCD_DisplayString(5, 2, 1, "King of the LCD");
	while((LPC_GPIO2->FIOPIN & (1<<10))){}
	
	osThreadNew(invaderMovement, NULL, NULL);
	osThreadNew(kingMovement, NULL, NULL);
	osThreadNew(kingReload, NULL, NULL);
	osThreadNew(kingShot, NULL, NULL);
	osThreadNew(switchPlayers, NULL, NULL);
	osKernelStart();
	for( ; ; ) {}
}
