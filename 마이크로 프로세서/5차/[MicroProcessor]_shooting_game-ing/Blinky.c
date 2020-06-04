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
#define missile_max 20

int key;
int fighter_posX;
int fighter_posY;
int meteor_posX;
int meteor_posY;
int meteor_speed;

int inGame;
int isShoot;
int isPassed;
int isMissile[50] = {0};
int missile_posX[100];
int missile_posY[100];
int missile_num;

int point;

char fighter_shape;
char meteor_shape;
char missile_shape;

void createmeteor(){	
	meteor_posX = 300;
	meteor_posY = (rand()%Line7) + 24;	
	GLCD_displayChar(meteor_posX, meteor_posY, meteor_shape);	
}

void missileMove(){
	int i =0;
	while(i<missile_max){		
		if(isMissile[i]==1){
			if(meteor_posY-12 <= missile_posY[i] && meteor_posY+12 >= missile_posY[i] && meteor_posX <=missile_posX[i]){
				int tmpp;
				int x = 100, y = Line9;
				isMissile[i] = 0;
				GLCD_displayChar(meteor_posX, meteor_posY, ' ');
				GLCD_displayChar(missile_posX[i], missile_posY[i], ' ');			
				createmeteor();
				meteor_speed*=1.1;
				point++;
				tmpp = point;
				while(tmpp>0){
					char tmp = (char)(tmpp%10+'0');	
					GLCD_displayChar(x, y, tmp);
					x-=20;
					tmpp = tmpp/10;
				}
				break;
			}	
			else{
				GLCD_displayChar(missile_posX[i], missile_posY[i], ' ');
				missile_posX[i] += 6;
				GLCD_displayChar(missile_posX[i], missile_posY[i], missile_shape);
				if(missile_posX[i] >=320){
					isMissile[i] = 0;				
				}
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
	if(isPassed == 0){
		GLCD_displayStringLn(Line1, "     GAME OVER     ");
		GLCD_displayStringLn(Line2, "  You Missed meteor ");
	}
	else{
		GLCD_displayStringLn(Line1, "     GAME OVER  ");
		GLCD_displayStringLn(Line2, "   fighter Crashed");
	}
	GLCD_displayStringLn(Line3, "  Your Point : ");	
	if(point == 0){
		GLCD_displayChar(260, Line3, '0');
	}
	else{
		int x=260, y=Line3;
		while(point>0){
			char tmp = (char)(point%10+'0');	
			GLCD_displayChar(x, y, tmp);
			x-=20;
			point = point/10;
		}	
	}	
	GLCD_displayStringLn(Line5, "  Push RESET Button ");	
	while(1){
	}	
}

void fighterCrash(){
	if((meteor_posY+10 >= fighter_posY && meteor_posY -10<= fighter_posY) &&(meteor_posX <= fighter_posX +20)){
		gameOver();
	}
}

void movemeteor(){
	GLCD_displayChar(meteor_posX, meteor_posY, ' ');
	meteor_posX -= meteor_speed;	
	fighterCrash();
	GLCD_displayChar(meteor_posX, meteor_posY, meteor_shape);
	if(meteor_posX <= 1) {
		isPassed -= 1;
		GLCD_displayChar(200, Line9, (char)(isPassed+'0'));
		if(isPassed == 0){
			gameOver();
		}		
		GLCD_displayChar(meteor_posX, meteor_posY, ' ');
		createmeteor();		
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
		if(fighter_posY == Line8) fighter_posY = Line8;
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
	meteor_posX = 300;
	meteor_posY = 120;
	missile_num = 0;
	meteor_speed = 5;
	key = -1;
	isShoot = 0;
	isPassed = 3;	
	inGame = 0;
	point = 0;
	fighter_shape = '>';
	meteor_shape ='O';
	missile_shape ='-';
	
	GLCD_setBackColor(White);
	GLCD_setTextColor(Black);
	GLCD_displayChar(100, Line9, (char)(point+'0'));
	GLCD_displayChar(200, Line9, (char)(isPassed+'0'));
}

void joyStick (void) {
	int counter=0;	
  uint32_t kbd_val;

  kbd_val = (LPC_GPIO1->FIOPIN >> 20) & KBD_MASK;
	key = -1;
  if ((kbd_val & KBD_UP)     == 0) key = up;  
  if ((kbd_val & KBD_LEFT)   == 0) key = left;  
  if ((kbd_val & KBD_RIGHT)  == 0) key = right;  
  if ((kbd_val & KBD_DOWN)   == 0) key = down;  
	
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
		movemeteor();
	}
}

int main (void)
{
	GLCD_init();                              /* Initialize the GLCD           */
	GLCD_clear(White);                        /* Clear the GLCD                */
	
	runGame();

}
