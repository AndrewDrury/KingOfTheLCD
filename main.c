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

//Screen boundaries => 320x240 pixels
#define WIDTH_PX 319
#define HEIGHT_PX 239

//16x24 sprite of the King character
unsigned short kingSprite[] ={
0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFDF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0010 (16) pixels
0xFFDF, 0xFFFF, 0xFFDF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x18C3, 0x0862, 0xFFFF, 0xFFDF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFDF,   // 0x0020 (32) pixels
0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x1082, 0x4A6A, 0x4A8A, 0x10A3, 0xFFDF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0030 (48) pixels
0xFFDF, 0xFFDF, 0xFFFF, 0xFFFF, 0x0862, 0x52AB, 0x2966, 0x636E, 0x634E, 0x08A3, 0x4A8A, 0x1082, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0040 (64) pixels
0xFFFF, 0xFFFF, 0x528B, 0x0862, 0x632D, 0x5B2E, 0x4AAC, 0x5B4E, 0x636F, 0x426B, 0x5B2E, 0x5B2D, 0x0041, 0x52AA, 0xFFFF, 0xFFDF,   // 0x0050 (80) pixels
0xFFFF, 0xFFFF, 0x0862, 0x0882, 0x634E, 0x636F, 0x532E, 0x5B6F, 0x5B4E, 0x5B4F, 0x636F, 0x634E, 0x10C4, 0x0862, 0xFFDF, 0xFFFF,   // 0x0060 (96) pixels
0xFFDF, 0xF7DF, 0x4A8A, 0x4ACC, 0x5B4E, 0x5B2D, 0x5B6E, 0x5B4E, 0x5B4E, 0x5B2D, 0x5B4D, 0x5B4D, 0x428B, 0x4A8A, 0xFFFF, 0xFFFF,   // 0x0070 (112) pixels
0xFFFF, 0x08A2, 0x1945, 0x5B4D, 0x52AA, 0x5289, 0x4A69, 0x4A69, 0x4A89, 0x4A69, 0x4A69, 0x528A, 0x634D, 0x4AAB, 0x0862, 0xFFFF,   // 0x0080 (128) pixels
0xFFFF, 0x10C3, 0x5B2D, 0x6B4D, 0xC574, 0x2060, 0x6288, 0x732B, 0x7B4C, 0x5226, 0x28A0, 0xC594, 0x630D, 0x634E, 0x0883, 0xFFFF,   // 0x0090 (144) pixels
0xFFFF, 0x10A3, 0x632D, 0x20A1, 0xCD52, 0xF655, 0xDE14, 0xDE15, 0xDE15, 0xF696, 0xE5F3, 0xCD51, 0x4A07, 0x6B4D, 0x1083, 0xFFFF,   // 0x00A0 (160) pixels
0xFFFF, 0x5289, 0x1880, 0xCD92, 0xEE13, 0xF612, 0xEE13, 0xEE54, 0xEE34, 0xEE13, 0xF612, 0xEE13, 0xC531, 0x2080, 0x4A48, 0xFFFF,   // 0x00B0 (176) pixels
0xFFFF, 0x4A48, 0x2080, 0xE635, 0xEE12, 0xF612, 0xEE13, 0xE613, 0xEE33, 0xEE12, 0xF612, 0xEE13, 0xE635, 0x20A0, 0x5269, 0xFFFF,   // 0x00C0 (192) pixels
0xF7DF, 0x52AA, 0x732B, 0x28A0, 0xEE34, 0xE5F2, 0x2860, 0xDE35, 0xE635, 0x2000, 0xEDF3, 0xEE14, 0x2040, 0x6AEA, 0x528A, 0xF7BF,   // 0x00D0 (208) pixels
0xFFFF, 0x18C3, 0x6B2B, 0x8BAC, 0xD5B3, 0xFF38, 0x2860, 0xE655, 0xDE15, 0x30A0, 0xFF9A, 0xCD72, 0xC573, 0x6AEB, 0x4A28, 0xFFFF,   // 0x00E0 (224) pixels
0xFFFF, 0x1081, 0x5247, 0xD5F5, 0x7B29, 0x8349, 0x2860, 0xDDD3, 0xDDF3, 0x2860, 0x7B29, 0x7309, 0xDE56, 0x5A68, 0x1061, 0xFFFF,   // 0x00F0 (240) pixels
0xFFFF, 0xFFFE, 0x2060, 0xEE76, 0xE614, 0xE613, 0xEE33, 0xEE12, 0xEE32, 0xEE12, 0xEE33, 0xE633, 0xE654, 0x2060, 0xFFFE, 0xFFFF,   // 0x0100 (256) pixels
0xFFFF, 0xFFFE, 0x3060, 0xBC4B, 0xCC4A, 0xC42A, 0xC44B, 0xBC4A, 0xDD2C, 0xDD4B, 0xE529, 0xE529, 0xDD4B, 0x2860, 0xFFFD, 0xFFFF,   // 0x0110 (272) pixels
0xFFFF, 0xFFFD, 0x3860, 0xD449, 0xCBE7, 0xA2A2, 0x4820, 0x4840, 0x4860, 0x5080, 0xE4E4, 0xF567, 0xED48, 0x61A0, 0xFFFC, 0xFFFF,   // 0x0120 (288) pixels
0xFFFF, 0xFFBC, 0xC46B, 0xCC27, 0xDC26, 0xDC05, 0xDC46, 0xD403, 0xED05, 0xFD45, 0xECE3, 0xF525, 0xED27, 0xDD4C, 0xFFDB, 0xFFFF,   // 0x0130 (304) pixels
0xFFFF, 0xFFDD, 0xBC4B, 0x4860, 0xCC49, 0xCC27, 0xD405, 0xE444, 0xFD25, 0xF545, 0xED47, 0xE528, 0x4880, 0xD52D, 0xFFFD, 0xFFDF,   // 0x0140 (320) pixels
0xFFDF, 0x1881, 0xA44E, 0x3080, 0xFFDD, 0xFFDB, 0xCC28, 0xD445, 0xF527, 0xE528, 0xFFD8, 0xFFFA, 0x2840, 0xCD70, 0x1880, 0xFFFF,   // 0x0150 (336) pixels
0xFFFF, 0x1041, 0xFFDE, 0xFFDE, 0xFFDF, 0xFFFE, 0xBC4B, 0x4860, 0x4020, 0xD54C, 0xFFDC, 0xFFFF, 0xFFFD, 0xFFDD, 0x1061, 0xFFFF,   // 0x0160 (352) pixels
0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFDF, 0xFFDB, 0xBCCD, 0xCD8F, 0xFFF9, 0xFFFE, 0xFFDF, 0xFFFE, 0xFFFE, 0xFFFF, 0xFFFF,   // 0x0170 (368) pixels
0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFD, 0xFFDB, 0xFFDA, 0xFFDB, 0xFFFE, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0180 (384) pixels
};

//16x1 sprite of a laser
unsigned short thinLaser[] ={
0xFFFF, 0xFFFF, 0xFFFF, 0xFFDF, 0xFFFF, 0xFEFB, 0x8041, 0xB8A3, 0xB8A3, 0x9062, 0xF576, 0xFF9E, 0xFFDF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0010 (16) pixels
};

//Global Variables

//Invader Position
struct player {
	int16_t player; //Status to track if player 1 or 2 is controlling character
	int16_t score; //Player's current score
	int16_t x; //Player's current horizontal position
	int16_t y; //Player's current vertical position
};
typedef struct player Player;

//Declare two Player characters, the invader and the king
Player invader;
Player king;

int16_t gameStatus; //Game status, (play/stop)
int16_t invaderWin; //Invader winning status, (win/loss)
int16_t reloading; //Reload status for the King's shooting

//Switching players task, ran whenever the players switch positions
void switchPlayers(void *arg) {
	while(1) {
		//Wait until the gameStatus is 1, which indicates a stoppage and a switch
		if(gameStatus == 0) {
			GLCD_Clear(Navy);
			GLCD_Clear(White);
				
			//Increment the winner's score
			if(invaderWin) {
				invader.score = invader.score + 1;
			}
			else {
				king.score = king.score + 1;
			}
			
			//If this is not the first time the game is run, display SWITCH CONTROLS
			if(invader.score > 0 || king.score > 0)
			{	
				GLCD_DisplayString(4, 2, 1, "SWITCH CONTROLS");
				
				//Wait for pushbutton
				while((LPC_GPIO2->FIOPIN & (1<<10))){}
				GLCD_Clear(White);	
			}
			
			//Switch player scores
			int16_t invaderScore = invader.score;
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
			
			//Reset the invader's position
			invader.x = 0;
			invader.y = MAX_ROW_LCD/2 + 1;
			
			//Display the players's score at the top of the LCD
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
			
			//Populate the King's border on the right side of the screen
			for(uint8_t border = 1; border <= MAX_ROW_LCD; border++) {
				GLCD_DisplayChar(border, MAX_COL_LCD, 1, '|');
			}
			
			osDelay(1000);
			
			//Clear SWITCH CONTROLS line
			GLCD_DisplayString(5, 2, 1, "               ");
			
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
		//Run the loop while the game is in play
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
			
			//Display the invader at updated position
			GLCD_DisplayChar(invader.y, invader.x, 1, graphic);
			
			//Small delay slows down each loop
			osDelay(100);

			//Check if invader hit right border (game won)
			if(invader.x == MAX_COL_LCD) {
				GLCD_DisplayChar(invader.y, invader.x, 1, ' ');
				gameStatus = 0;
				invaderWin = 1;
			}
			
			//Clear the invader's previous location (since redrawing the invader during the next loop)
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
	
	//Define a variable which tracks direction of King's movement, (-1 = UP, 1 = DOWN)
	int16_t move = -1;
	//Store all possible pixel locations (in y-direction) for the King to be in
	int16_t kingYPos[] = {0,24,48,72,96,120,144,168,192,216};
	
	while(1) {
		//Run the loop while the game is in play
		while(gameStatus){
			//Once the King reaches row 1, start moving downwards
			if(king.y == 1) {
				move = 1;
			}
			//If the King is at the last row, start moving upwards
			else if(king.y == MAX_ROW_LCD) {
				move = -1;
			}
			
			//Clear old position
			GLCD_DisplayChar(king.y, MAX_COL_LCD, 1, '|');
			
			//Move the king's position
			king.y = king.y + move;
			
			//Display the King's sprite at the new location
			GLCD_Bitmap(WIDTH_PX-16,kingYPos[king.y],16,24,(unsigned char*)kingSprite);
		
			//Define a variable which will be the delay for each loop
			int16_t ticks = 100;
			
			//Get an updated tick delay from the potentiometer, turning the potentiometer will speed up or slow down the King
			if (LPC_ADC->ADGDR & 0x80000000) {
				int16_t pot = (LPC_ADC->ADGDR & (0x0FFF<<4)) >> 4;
				ticks = 200 * pot/4096 + 50;
				LPC_ADC->ADCR |= (1<<24);
			}
			
			//Delay the loop
			osDelay(ticks);
			
			//Redraw the border
			for(uint16_t kingY = 1; kingY < 10; kingY++) {
				GLCD_DisplayChar(kingY, MAX_COL_LCD, 1, '|');
			}
			
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
		//Run the loop while the game is in play
		while(gameStatus) {
			//Incrementally turn on each LEDs, until all LEDs are on (reloading period)
			for(uint8_t led = 0; led<9 && reloading == 1; led++) {
				//Clear all LEDs
				if(led==0){
					LPC_GPIO1->FIOCLR |= 0xB0000000;
					LPC_GPIO2->FIOCLR |= 0x0000007C;
				}
				
				//Turn LEDs On GPIO1
				else if(led == 6) {
					LPC_GPIO1->FIOSET |= (1<<31);
				}
				else if(led==7) {
					LPC_GPIO1->FIOSET |= (1<<29);
				}
				else if(led==8) {
					LPC_GPIO1->FIOSET |= (1<<28);
				}

				//Turn LEDs On GPIO2
				else {
					LPC_GPIO2->FIOSET |= (1<<7-led);
				}
				
				//Delay between each LED turning on
				osDelay(225);
			}
			//The king can now shoot, set reloading status to 0
			reloading = 0;
		}
	}
}

//A shot that is fired by the King
void shot(uint16_t y) {
	//Replace the King's sprite with the border
	GLCD_DisplayChar(y, MAX_COL_LCD, 1, '|');
	
	//Move the volley of arrows across the screen, from right to left, checking that the game is still in play
	for(int16_t position = MAX_COL_LCD-1;position>=0 && gameStatus; position--) {
		//Shoot a row of shots
		for(uint16_t shotRow = 1; shotRow<=MAX_ROW_LCD; shotRow++) {
			//If the invader is hit, the King wins and the game is stopped
			if(position == invader.x && y != invader.y) {
				GLCD_DisplayChar(invader.y, invader.x, 1, ' ');
				invaderWin = 0;
				gameStatus = 0;
			}
			//Otherwise draw the shot on the screen (as long as it isn't in the original location of the King)
			else if(shotRow != y) {
				GLCD_DisplayChar(shotRow, position, 1, '|');
			}
		}
		//Small delay slows down the speed of the shot
		osDelay(75);
		
		//Clear the shot row and the prev row
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
	float time = timer_read()/1E6;
	while(1){
		//Run the loop while the game is in play
		while(gameStatus) {
			//Pushbutton pressed -- shot fired if reload time is up (reloading status is 0)
			if(!(LPC_GPIO2->FIOPIN & (1<<10)) && reloading == 0) {
				reloading = 1;
				shot(king.y);
			}
		}
	}
}

int main(void){
	printf("Start of King of the LCD\n");

	timer_setup();
	
	//Setup the GLCD
	GLCD_Init();
	GLCD_Clear(White);
	GLCD_SetBackColor(White);
	GLCD_SetTextColor(Black);
	
	//Set all the initial values
	gameStatus = 1;
	reloading = 0;
	invaderWin =1;
	
	//Set the invader's initial values
	invader.player = 2;
	invader.score = -1;
	invader.x = 0;
	invader.y = MAX_ROW_LCD/2 + 1;
	
	//Set the king's initial values
	king.player = 1;
	king.score = 0;
	king.x = MAX_COL_LCD;
	king.y = MAX_ROW_LCD/2 + 1;
	
	osKernelInitialize();

	//Display an intro to the game, explaining the controls
	GLCD_DisplayString(4, 3, 1, "King of the LCD");
	while((LPC_GPIO2->FIOPIN & (1<<10))){}
	GLCD_Clear(White);
	osDelay(200);

	//Explain Player 1's instructions
	GLCD_DisplayString(1, 6, 1, "Player 1");
	GLCD_DisplayString(3, 1, 1, "Use the joystick");
	GLCD_DisplayString(5, 1, 1, "Get the invader");
	GLCD_DisplayString(6, 1, 1, "across the");
	GLCD_DisplayString(7, 1, 1, "battlefield");
	GLCD_DisplayString(8, 1, 1, "to the castle");
	osDelay(200);	
	while((LPC_GPIO2->FIOPIN & (1<<10))){}
	GLCD_Clear(White);
  osDelay(200);
	
	//Explain Player 2's instructions
	GLCD_DisplayString(1, 6, 1, "Player 2");
	GLCD_DisplayString(3, 1, 1, "Use the button");
	GLCD_DisplayString(5, 1, 1, "Shoot the invader");	
	GLCD_DisplayString(6, 1, 1, "with a volley");	
	GLCD_DisplayString(7, 1, 1, "of arrows");		
	osDelay(200);	
	while((LPC_GPIO2->FIOPIN & (1<<10))){}
		
	GLCD_Clear(White);
	GLCD_SetBackColor(White);
	GLCD_SetTextColor(Black);
		
	//Start four threads
	osThreadNew(invaderMovement, NULL, NULL);
	osThreadNew(kingMovement, NULL, NULL);
	osThreadNew(kingReload, NULL, NULL);
	osThreadNew(kingShot, NULL, NULL);
	osThreadNew(switchPlayers, NULL, NULL);
	osKernelStart();
	for( ; ; ) {}
}
