/*----------------------------------------------------------------------------
 *      R T L   K e r n e l   E x a m p l e
 *----------------------------------------------------------------------------
 *      Name:    BLINKY.C
 *      Purpose: RTX example program
 *      Rev.:    V3.50
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2008 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <RTL.h>
#include <LPC17xx.H>                    /* LPC17xx definitions                */
#include "GLCD.h"
#include "LED.h"


OS_TID t_phaseA;                        /* assigned task id of task: phase_a */
OS_TID t_phaseB;                        /* assigned task id of task: phase_b */
OS_TID t_phaseC;                        /* assigned task id of task: phase_c */
OS_TID t_phaseD;                        /* assigned task id of task: phase_d */
OS_TID t_clock;                         /* assigned task id of task: clock   */
OS_TID t_lcd;                           /* assigned task id of task: lcd     */

OS_MUT mut_GLCD;                        /* Mutex to controll GLCD access     */

#define LED_A      LED_0
#define LED_B      LED_1
#define LED_C      LED_2
#define LED_D      LED_3
#define LED_CLK    LED_7

#define KBD_SELECT      0x01               
#define KBD_LEFT        0x08               
#define KBD_UP          0x10               
#define KBD_RIGHT       0x20               
#define KBD_DOWN        0x40
#define KBD_MASK        0x79  

#define right 1
#define left 2
#define up 3
#define down 4
#define sh 240
#define sw 340
#define missile_max 20

int key;
int fighter_posX;
int fighter_posY;
int meteo_posX;
int meteo_posY;
int meteo_speed;

int inGame;
int isShoot;
int isPassed;
int isMissile[50] = {0};
int missile_posX[100];
int missile_posY[100];
int missile_num;


char fighter_shape;
char meteo_shape;
char missile_shape;

void createMeteo(){
	//srand( (unsigned)time(NULL) );  
	meteo_posX = 300;
	meteo_posY = (rand()%200) + 24;	
	GLCD_displayChar(meteo_posX, meteo_posY, meteo_shape);	
}
//void meteoCrash(){
//	int i = 0;
//	while(i <50){		
//		if(meteo_posY <= missile_posY[i] && meteo_posY+24 >= missile_posY[i] && meteo_posX <=missile_posX[i] && isMissile[i]==1){
//			isMissile[i] = 0;
//			GLCD_displayChar(meteo_posX, meteo_posY, ' ');
//			GLCD_displayChar(missile_posX[i], missile_posY[i], ' ');			
//			createMeteo();
//			break;
//		}	
//		i++;
//	}	
//}
void missileMove(){
	int i =0;
	while(i<missile_max){		
		if(isMissile[i]==1){
			if(meteo_posY-12 <= missile_posY[i] && meteo_posY+12 >= missile_posY[i] && meteo_posX <=missile_posX[i]){
				isMissile[i] = 0;
				GLCD_displayChar(meteo_posX, meteo_posY, ' ');
				GLCD_displayChar(missile_posX[i], missile_posY[i], ' ');			
				createMeteo();
				meteo_speed*=1.1;
				break;
			}	
			GLCD_displayChar(missile_posX[i], missile_posY[i], ' ');
			missile_posX[i] += 6;
			GLCD_displayChar(missile_posX[i], missile_posY[i], missile_shape);
			if(missile_posX[i] >=320){
				isMissile[i] = 0;				
			}
		}
		i++;
	}		
}
void fire(){
	missile_posX[missile_num] = 24;
	missile_posY[missile_num] = fighter_posY;	
	isMissile[missile_num] = 1;
	missile_num++;
	if(missile_num == missile_max) missile_num=0;
	missileMove();
}

void gameOver(){
	GLCD_clear(Black);
	GLCD_setBackColor(Black);
	GLCD_setTextColor(White);
	if(isPassed == 3){
		GLCD_displayStringLn(Line1, "     GAME OVER     ");
		GLCD_displayStringLn(Line3, "  You Missed Meteo ");
	}
	else{
		GLCD_displayStringLn(Line1, "     GAME OVER  ");
		GLCD_displayStringLn(Line3, "   fighter Crashed");
	}
	GLCD_displayStringLn(Line5, "  Push RESET Button ");	
	while(1){
	}	
}
void moveMeteo(){
	GLCD_displayChar(meteo_posX, meteo_posY, ' ');
	meteo_posX -= meteo_speed;	
	GLCD_displayChar(meteo_posX, meteo_posY, meteo_shape);
	if(meteo_posX <= 1) {
		isPassed += 1;
		if(isPassed == 3){
			gameOver();
		}
		GLCD_displayChar(meteo_posX, meteo_posY, ' ');
		createMeteo();		
	}
}
void fighterCrash(){
	if((meteo_posY == fighter_posY) && (meteo_posX <= fighter_posX +20)){
		gameOver();
	}
}
void fighterMove(int key){
	int tmpX = fighter_posX;
	int tmpY = fighter_posY;		
	
	if(key == up) {
		if(fighter_posY == 0) fighter_posY = 0;
		else fighter_posY -= 4;
	}
	else if(key == down) {
		if(fighter_posY == 240-24) fighter_posY = 240-24;
		else fighter_posY += 4;
	}
	else if(key == right){
		fire();
	}			
	key = -1;
	GLCD_displayChar(tmpX, tmpY, ' ');
	GLCD_displayChar(fighter_posX, fighter_posY, fighter_shape);
}
void gameInit(){
	fighter_posX = 0;
	fighter_posY = 120;	
	meteo_posX = 300;
	meteo_posY = 120;
	missile_num = 0;
	meteo_speed = 5;
	key = -1;
	isShoot = 0;
	isPassed = 0;	
	inGame = 0;
	
	fighter_shape = '>';
	meteo_shape ='O';
	missile_shape ='-';
	
	GLCD_setBackColor(White);
	GLCD_setTextColor(Black);
}

void joyStick (void) {
	int counter=0;	
  uint32_t kbd_val;

  kbd_val = (LPC_GPIO1->FIOPIN >> 20) & KBD_MASK;
	key = -1;
  if ((kbd_val & KBD_UP)     == 0) key = up;  /* up     pressed means 0 */
  if ((kbd_val & KBD_LEFT)   == 0) key = left;  /* left   pressed means 0 */
  if ((kbd_val & KBD_RIGHT)  == 0) key = right;  /* right  pressed means 0 */  
  if ((kbd_val & KBD_DOWN)   == 0) key = down;  /* down   pressed means 0 */	
	
	while(counter<700000)	
	{	
		counter++;
	}
}


void runGame(){
	gameInit();
	inGame = 1;
	while(inGame == 1){
		key = -1;		
		joyStick();	
		fighterMove(key);
		missileMove();
		moveMeteo();
		//meteoCrash();
		fighterCrash();
	}
}

/*----------------------------------------------------------------------------
 *        Function 'signal_func' called from multiple tasks
 *---------------------------------------------------------------------------*/
//void signal_func (OS_TID task)
//{
//	os_evt_set (0x0100, t_clock);          /* send event signal to clock task  */
//	os_dly_wait (50);                      /* delay 50 clock ticks             */
//	os_evt_set (0x0100, t_clock);          /* send event signal to clock task  */
//	os_dly_wait (50);                      /* delay 50 clock ticks             */
//	os_evt_set (0x0001, task);             /* send event to task 'task'        */
//	os_dly_wait (50);                      /* delay 50 clock ticks             */
//}

/*----------------------------------------------------------------------------
 *        Task 1 'phaseA': Phase A output
 *---------------------------------------------------------------------------*/
//__task void phaseA (void)
//{
//	for (;;)
//	{
//		os_evt_wait_and (0x0001, 0xffff);    /* wait for an event flag 0x0001    */
//		LED_On (LED_A);
//		signal_func (t_phaseB);              /* call common signal function      */
//		LED_Off(LED_A);
//	}
//}

/*----------------------------------------------------------------------------
 *        Task 2 'phaseB': Phase B output
 *---------------------------------------------------------------------------*/
//__task void phaseB (void)
//{
//	for (;;)
//	{
//		os_evt_wait_and (0x0001, 0xffff);    /* wait for an event flag 0x0001    */
//		LED_On (LED_B);
//		signal_func (t_phaseC);              /* call common signal function      */
//		LED_Off(LED_B);
//	}
//}

/*----------------------------------------------------------------------------
 *        Task 3 'phaseC': Phase C output
 *---------------------------------------------------------------------------*/
//__task void phaseC (void)
//{
//	for (;;)
//	{
//		os_evt_wait_and (0x0001, 0xffff);    /* wait for an event flag 0x0001    */
//		LED_On (LED_C);
//		signal_func (t_phaseD);              /* call common signal function      */
//		LED_Off(LED_C);
//	}
//}

/*----------------------------------------------------------------------------
 *        Task 4 'phaseD': Phase D output
 *---------------------------------------------------------------------------*/
//__task void phaseD (void)
//{
//	for (;;)
//	{
//		os_evt_wait_and (0x0001, 0xffff);    /* wait for an event flag 0x0001    */
//		LED_On (LED_D);
//		signal_func (t_phaseA);              /* call common signal function      */
//		LED_Off(LED_D);
//	}
//}

/*----------------------------------------------------------------------------
 *        Task 5 'clock': Signal Clock
 *---------------------------------------------------------------------------*/
//__task void clocks (void)
//{
//	for (;;)
//	{
//		os_evt_wait_and (0x0100, 0xffff);    /* wait for an event flag 0x0100    */
//		LED_On (LED_CLK);
//		os_dly_wait (8);                     /* delay 8 clock ticks              */
//		LED_Off(LED_CLK);
//	}
//}

/*----------------------------------------------------------------------------
 *        Task 6 'lcd': LCD Control task
 *---------------------------------------------------------------------------*/
//__task void lcd (void)
//{
//	for (;;)
//	{
//		GLCD_setTextColor(Black);                          /* Set the Text Color */			
//		GLCD_displayChar(0, 60, 'o');
//		GLCD_displayChar(0, 40, 'o');
//		GLCD_displayChar(0, 20, 'o');	
//	}
//}

/*----------------------------------------------------------------------------
 *        Task 7 'init': Initialize
 *---------------------------------------------------------------------------*/

//__task void init (void)
//{
//	os_mut_init(mut_GLCD);
//
//	t_phaseA = os_tsk_create (phaseA, 0);  /* start task phaseA                */
//	t_phaseB = os_tsk_create (phaseB, 0);  /* start task phaseB                */
//	t_phaseC = os_tsk_create (phaseC, 0);  /* start task phaseC                */
//	t_phaseD = os_tsk_create (phaseD, 0);  /* start task phaseD                */
//	t_clock  = os_tsk_create (clocks, 0);   /* start task clock                 */
//	t_lcd    = os_tsk_create (lcd, 0);     /* start task lcd                   */
//	os_evt_set (0x0001, t_phaseA);         /* send signal event to task phaseA */
//	os_tsk_delete_self ();
//}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void)
{
//	SystemInit();                             /* Initialize the MCU clocks     */

//	LED_init ();                              /* Initialize the LEDs           */

	GLCD_init();                              /* Initialize the GLCD           */
	GLCD_clear(White);                        /* Clear the GLCD                */
	
	runGame();
	//GLCD_displayChar(0, 24, 'o');
	//os_sys_init(init);                        /* Initialize RTX and start init */
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
