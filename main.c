#include <lpc17xx.h>
#include "stdio.h"
#include "uart.h"
#include "GLCD.h"
#include <cmsis_os2.h>
#include "random.h"
#include "lfsr113.h"

//Task Regulating the Invader's movement
void invaderMovement(void *arg) {
	while(1){

		//Joystick left
		if (!(LPC_GPIO1->FIOPIN & (1<<23))) {
			
		}
		//Joystick Up
		else if (!(LPC_GPIO1->FIOPIN & (1<<24))) {

		}
		//Joystick Right
		else if (!(LPC_GPIO1->FIOPIN & (1<<25))) {

		}
		//Joystick Down
		else if (!(LPC_GPIO1->FIOPIN & (1<<26))) {

		}
		
		//Check if invader hit right border (game won)

		//Joystick pressed
		if (!(LPC_GPIO1->FIOPIN & (1<<20))) {

		}
	}
}

//Task Regulating the King's movement
void kingMovement(void *arg) {
	while(1){
		//Move King up and down right side of screen at constant speed
		
		//Delay by one second
		osDelay(1000);
	}
}

//Task Regulating the King's reload time
void kingReload(void *arg) {
	while(1){
		//LEDs 1, 2, 3 on bits 28, 29, 31 on GPIO1
		LPC_GPIO1->FIODIR |= 0xB0000000;
		//LEDs 4, 5, 6, 7, 8 on bits 2, 3, 4, 5, 6 on GPIO2
		LPC_GPIO2->FIODIR |= 0x0000007C;
		
		LPC_GPIO1->FIOCLR |= 0xB0000000;
		LPC_GPIO2->FIOCLR |= 0x0000007C;

		//Turn LED Off
		LPC_GPIO2->FIOCLR |= (1<<6);

		//Turn LED On
		LPC_GPIO2->FIOSET |= (1<<6);
	}
}

//Task Regulating the King's shot
void kingShot(void *arg) {
	while(1){
		//Pushbutton pressed -- shot fired
		//Also check if reload time is up
		if(!(LPC_GPIO2->FIOPIN & (1<<10))) {
		}
	}
}


//Reference code to manipulate GLCD display
void display(void *arg) {
	//GLCD Screen Code
	GLCD_Clear(Black);
	GLCD_SetBackColor(Black);   
	GLCD_SetTextColor(Green);
	
	GLCD_DisplayString(1, 0, 1, "sent");
	
	char print_msg [8];
	uint32_t number;
	sprintf(print_msg, "%d", number);
	GLCD_DisplayString(1, 5, 1, print_msg);
}

int main(void){
	printf("Start of King of the LCD\n");
	
	//Start four threads
	osKernelInitialize();
	
	osThreadNew(invaderMovement, NULL, NULL);
	osThreadNew(kingMovement, NULL, NULL);
	osThreadNew(kingReload, q_id2, NULL);
	osThreadNew(kingShot, NULL, NULL);
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