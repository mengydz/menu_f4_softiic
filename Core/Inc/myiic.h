/**************************************************************************
  * @brief     : 模拟IIC驱动程序
  * @author    : 何运富(yunfu.he@leadchina.cn)
  * @copyright : LeadChina
  * @version   : V1.0
  * @note      : 注意事项
  * @history   : 20200211
***************************************************************************/
#ifndef _MYIIC_H
#define _MYIIC_H
#include "stm32f4xx_hal.h"
	
//IO方向设置
#define MyIIC_SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PE12输入模式
#define MyIIC_SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PE12输出模式
//IO操作
#define MyIIC_SCL(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET)) //SCL
#define MyIIC_SDA(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET)) //SDA
#define MyIIC_READ_SDA    HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)  //输入SDA


void delay_iic(uint8_t Micros);
//IIC所有操作函数
void MyIIC_Init(void);       //初始化IIC的IO口				 
void MyIIC_Start(void);				//发送IIC开始信号
void MyIIC_Stop(void);	  		//发送MyIIC停止信号
void MyIIC_Send_Byte(uint8_t txd);	//MyIIC发送一个字节
uint8_t MyIIC_Read_Byte(unsigned char ack);//MyIIC读取一个字节
uint8_t MyIIC_Wait_Ack(void); 			//MyIIC等待ACK信号
void MyIIC_Ack(void);					//MyIIC发送ACK信号
void MyIIC_NAck(void);				//MyIIC不发送ACK信号

#endif

