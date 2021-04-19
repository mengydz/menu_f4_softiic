/**************************************************************************
  * @brief     : ģ��IIC��������
  * @author    : ���˸�(yunfu.he@leadchina.cn)
  * @copyright : LeadChina
  * @version   : V1.0
  * @note      : ע������
  * @history   : 20200211
***************************************************************************/
#ifndef _MYIIC_H
#define _MYIIC_H
#include "stm32f4xx_hal.h"
	
//IO��������
#define MyIIC_SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PE12����ģʽ
#define MyIIC_SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PE12���ģʽ
//IO����
#define MyIIC_SCL(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET)) //SCL
#define MyIIC_SDA(n)  (n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET)) //SDA
#define MyIIC_READ_SDA    HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)  //����SDA


void delay_iic(uint8_t Micros);
//IIC���в�������
void MyIIC_Init(void);       //��ʼ��IIC��IO��				 
void MyIIC_Start(void);				//����IIC��ʼ�ź�
void MyIIC_Stop(void);	  		//����MyIICֹͣ�ź�
void MyIIC_Send_Byte(uint8_t txd);	//MyIIC����һ���ֽ�
uint8_t MyIIC_Read_Byte(unsigned char ack);//MyIIC��ȡһ���ֽ�
uint8_t MyIIC_Wait_Ack(void); 			//MyIIC�ȴ�ACK�ź�
void MyIIC_Ack(void);					//MyIIC����ACK�ź�
void MyIIC_NAck(void);				//MyIIC������ACK�ź�

#endif

