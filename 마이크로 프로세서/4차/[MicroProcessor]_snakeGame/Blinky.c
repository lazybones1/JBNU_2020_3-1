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

int snk_x;
int snk_y;
int snake_x[3] = {0};
int snake_y[3] = {0};
int new_head[2];
int food_x;
int food_y;
int key;
int i, j;

void initGame(){
	snk_x = sw/4;
  snk_y = sh/2;	
  snake_x[0] = snk_x;
	snake_x[1] = snk_x-10;
	snake_x[2] = snk_x-20;
	snake_y[0] = snk_y;
	snake_y[1] = snk_y;
	snake_y[2] = snk_y;  
  new_head[0] = snake_x[0];
	new_head[1] = snake_y[0];
  food_x = sw/2;
  food_y = sh/2;
  key = 1;	
}

void moveSnake(int key){
  if (key == right)
    new_head[0] += 1;
  if (key == left)
    new_head[0] -= 1;
  if (key == up)
    new_head[1] -= 1;
  if (key == down)
    new_head[1] += 1;
}

int checkFood(int x, int y){
	for(i = 0; i<3; i++){
		if(x == snake_x[i]) return 0;
		if(x == snake_x[i]) return 0;
	}
	return 1;
}

void GetInReport (void) {
  uint32_t kbd_val;

  kbd_val = (LPC_GPIO1->FIOPIN >> 20) & KBD_MASK;

  if ((kbd_val & KBD_UP)     == 0) key = up;  /* up     pressed means 0 */
  if ((kbd_val & KBD_LEFT)   == 0) key = left;  /* left   pressed means 0 */
  if ((kbd_val & KBD_RIGHT)  == 0) key = right;  /* right  pressed means 0 */  
  if ((kbd_val & KBD_DOWN)   == 0) key = down;  /* down   pressed means 0 */
}

void startGame(){
  GLCD_displayChar(snake_x[0], snake_y[0], 'o');
	GLCD_displayChar(snake_x[1], snake_y[1], 'o');
	GLCD_displayChar(snake_x[2], snake_y[2], 'o');
	GLCD_displayChar(food_x, food_y, 'f');
	
	while(1){	
		GetInReport();
	
		if(snake_x[0] < 0 || snake_x[0] > sw || snake_y[0] <0 || snake_y[0] >sh || (snake_x[0] == snake_x[1] && snake_y[0] == snake_y[1])){
			GLCD_clear(White);
			GLCD_displayStringLn(Line3, "      Game End    ");
			GLCD_displayStringLn(Line4, " push RESET Button ");
			while(1){				
			}
		}
		moveSnake(key);
  
		snake_x[1] = snake_x[0];
		snake_y[1] = snake_y[0];
		snake_x[0] = new_head[0];
		snake_y[0] = new_head[1];
  

		if (snake_x[0] == food_x && snake_y[0] == food_y){
			food_x = -1;
			food_y = -1;
			while(food_x == -1 && food_y == -1){
				//srand(time(NULL));
				//food_x = (rand()%sw) +1;
				//food_y = (rand()%sh) +1;	
				food_x = 300;
				food_y = 200;
				if(checkFood(food_x, food_y) == 1) {
					GLCD_displayChar(food_x, food_y, 'f');
				}
				else{
					food_x = -1;
					food_y = -1;
				}						
			}
		}
		GLCD_displayChar(snake_x[2], snake_y[2], ' ');
		GLCD_displayChar(snake_x[0], snake_y[0], 'o');
	}	
}

void LED_On  (unsigned char led)
{	
	LED_on (led);
}

void LED_Off (unsigned char led)
{
	LED_off(led);
}

/*----------------------------------------------------------------------------
 *        Function 'signal_func' called from multiple tasks
 *---------------------------------------------------------------------------*/
void signal_func (OS_TID task)
{
	os_evt_set (0x0100, t_clock);          /* send event signal to clock task  */
	os_dly_wait (50);                      /* delay 50 clock ticks             */
	os_evt_set (0x0100, t_clock);          /* send event signal to clock task  */
	os_dly_wait (50);                      /* delay 50 clock ticks             */
	os_evt_set (0x0001, task);             /* send event to task 'task'        */
	os_dly_wait (50);                      /* delay 50 clock ticks             */
}

/*----------------------------------------------------------------------------
 *        Task 1 'phaseA': Phase A output
 *---------------------------------------------------------------------------*/
__task void phaseA (void)
{
	for (;;)
	{
		os_evt_wait_and (0x0001, 0xffff);    /* wait for an event flag 0x0001    */
		LED_On (LED_A);
		signal_func (t_phaseB);              /* call common signal function      */
		LED_Off(LED_A);
	}
}

/*----------------------------------------------------------------------------
 *        Task 2 'phaseB': Phase B output
 *---------------------------------------------------------------------------*/
__task void phaseB (void)
{
	for (;;)
	{
		os_evt_wait_and (0x0001, 0xffff);    /* wait for an event flag 0x0001    */
		LED_On (LED_B);
		signal_func (t_phaseC);              /* call common signal function      */
		LED_Off(LED_B);
	}
}

/*----------------------------------------------------------------------------
 *        Task 3 'phaseC': Phase C output
 *---------------------------------------------------------------------------*/
__task void phaseC (void)
{
	for (;;)
	{
		os_evt_wait_and (0x0001, 0xffff);    /* wait for an event flag 0x0001    */
		LED_On (LED_C);
		signal_func (t_phaseD);              /* call common signal function      */
		LED_Off(LED_C);
	}
}

/*----------------------------------------------------------------------------
 *        Task 4 'phaseD': Phase D output
 *---------------------------------------------------------------------------*/
__task void phaseD (void)
{
	for (;;)
	{
		os_evt_wait_and (0x0001, 0xffff);    /* wait for an event flag 0x0001    */
		LED_On (LED_D);
		signal_func (t_phaseA);              /* call common signal function      */
		LED_Off(LED_D);
	}
}

/*----------------------------------------------------------------------------
 *        Task 5 'clock': Signal Clock
 *---------------------------------------------------------------------------*/
__task void clocks (void)
{
	for (;;)
	{
		os_evt_wait_and (0x0100, 0xffff);    /* wait for an event flag 0x0100    */
		LED_On (LED_CLK);
		os_dly_wait (8);                     /* delay 8 clock ticks              */
		LED_Off(LED_CLK);
	}
}

/*----------------------------------------------------------------------------
 *        Task 6 'lcd': LCD Control task
 *---------------------------------------------------------------------------*/
__task void lcd (void)
{
	for (;;)
	{
		GLCD_setTextColor(Black);                          /* Set the Text Color */			
		GLCD_displayChar(0, 60, 'o');
		GLCD_displayChar(0, 40, 'o');
		GLCD_displayChar(0, 20, 'o');	
	}
}

/*----------------------------------------------------------------------------
 *        Task 7 'init': Initialize
 *---------------------------------------------------------------------------*/
__task void init (void)
{
	os_mut_init(mut_GLCD);

	t_phaseA = os_tsk_create (phaseA, 0);  /* start task phaseA                */
	t_phaseB = os_tsk_create (phaseB, 0);  /* start task phaseB                */
	t_phaseC = os_tsk_create (phaseC, 0);  /* start task phaseC                */
	t_phaseD = os_tsk_create (phaseD, 0);  /* start task phaseD                */
	t_clock  = os_tsk_create (clocks, 0);   /* start task clock                 */
	t_lcd    = os_tsk_create (lcd, 0);     /* start task lcd                   */
	os_evt_set (0x0001, t_phaseA);         /* send signal event to task phaseA */
	os_tsk_delete_self ();
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void)
{
	SystemInit();                             /* Initialize the MCU clocks     */

	LED_init ();                              /* Initialize the LEDs           */

	GLCD_init();                              /* Initialize the GLCD           */
	GLCD_clear(White);                        /* Clear the GLCD                */
	initGame();
	startGame();
	os_sys_init(init);                        /* Initialize RTX and start init */
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
