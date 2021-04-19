#ifndef __OLED_H
#define __OLED_H			  	 
#include "stm32f4xx_hal.h"
#include "stdlib.h"	    
#include "stdio.h"

		     
//OLED¿ØÖÆÓÃº¯Êý
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
void OLED_ShowString(uint8_t x,uint8_t y,const char *p,uint8_t size,uint8_t mode);

#endif  
	 



