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

	}
}

//Task Regulating the King's movement
void kingMovement(void *arg) {
	while(1){

	}
}

//Task Regulating the King's reload time
void kingReload(void *arg) {
	while(1){

	}
}

//Task Regulating the King's shot
void kingShot(void *arg) {
	GLCD_Init();
	
	while(1){
		//Delay one second
		osDelay(1000);
		
		char print_msg [8];
		
		GLCD_Clear(Black);
		GLCD_SetBackColor(Black);   
		GLCD_SetTextColor(Green);
		
		//Total messages sent successfully
		GLCD_DisplayString(1, 0, 1, "sent");
		
		sprintf(print_msg, "%d", msgSent1);
		GLCD_DisplayString(1, 5, 1, print_msg);
		
	}
}

int main(void){
	printf("Start lab 5\n");
	
	//Start four threads
	osKernelInitialize();
	
	osThreadNew(invaderMovement, NULL, NULL);
	osThreadNew(kingMovement, NULL, NULL);
	osThreadNew(kingReload, q_id2, NULL);
	osThreadNew(kingShot, NULL, NULL);
	osKernelStart();
	for( ; ; ) {}
}