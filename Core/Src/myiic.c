/**************************************************************************
  * @brief     : ģ��IIC��������
  * @author    : ���˸�(yunfu.he@leadchina.cn)
  * @copyright : LeadChina
  * @version   : V1.0
  * @note      : ע������
  * @history   : 20200211
***************************************************************************/
#include "myiic.h"
#include "tim.h"
/**
*FM24V10 ���Ŷ���
*SCL:PH8
*SDA:PE12
**/


/*******************************************************************************
* ������ : void delay_iic(uint8_t Micros)
* ��  �� : us ��ʱ����
* ��  �� : Micros:��ʱ���ٸ�us
* ��  �� : ��
* ����ֵ : ��
*******************************************************************************/
void delay_iic(uint8_t us)
{
    for(uint16_t _delay=0;_delay<us;_delay++)
    for(uint8_t _delay_n=0;_delay_n<2;_delay_n++);
}

/*******************************************************************************
* ������ : void MyIIC_Init(void)
* ��  �� : ģ��IIC���ų�ʼ�� 
* ��  �� : ��
* ��  �� : ��
* ����ֵ : ��
*******************************************************************************/
void MyIIC_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();   //ʹ��GPIOEʱ��

	GPIO_Initure.Pin=GPIO_PIN_7;
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_FAST;     //����
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_6;
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_FAST;     //����
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);

	MyIIC_SDA(1);
	MyIIC_SCL(1);  
}

/*******************************************************************************
* ������ : void MyIIC_Start(void)
* ��  �� : ģ��IIC��ʼ�ź�
* ��  �� : ��
* ��  �� : ��
* ����ֵ : ��
*******************************************************************************/
void MyIIC_Start(void)
{
	MyIIC_SDA_OUT();     //sda�����
	MyIIC_SDA(1);	  	  
	MyIIC_SCL(1);
	delay_iic(1);
 	MyIIC_SDA(0);//START:when CLK is high,DATA change form high to low 
	delay_iic(1);
	MyIIC_SCL(0);//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  

/*******************************************************************************
* ������ : void MyIIC_Stop(void)
* ��  �� : ģ��IICֹͣ�ź�
* ��  �� : ��
* ��  �� : ��
* ����ֵ : ��
*******************************************************************************/
void MyIIC_Stop(void)
{
	MyIIC_SDA_OUT();//sda�����
	MyIIC_SCL(0);
	MyIIC_SDA(0);//STOP:when CLK is high DATA change form low to high
 	delay_iic(1);
	MyIIC_SCL(1); 
	delay_iic(1);			
	MyIIC_SDA(1);//����I2C���߽����ź�				   	
}

/*******************************************************************************
* ������ : uint8_t MyIIC_Wait_Ack(void)
* ��  �� : �ȴ�Ӧ���źŵ���
* ��  �� : ��
* ��  �� : ��
* ����ֵ : 1������Ӧ��ʧ��
*          0������Ӧ��ɹ�
*******************************************************************************/
uint8_t MyIIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	MyIIC_SDA_IN();      //SDA����Ϊ����  
	MyIIC_SDA(1);delay_iic(1);	   
	MyIIC_SCL(1);delay_iic(1);	 
	while(MyIIC_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			MyIIC_Stop();
			return 1;
		}
	}
	MyIIC_SCL(0);//ʱ�����0 	   
	return 0;  
} 

/*******************************************************************************
* ������ : void MyIIC_Ack(void)
* ��  �� : ���� ACK Ӧ��
* ��  �� : ��
* ��  �� : ��
* ����ֵ : ��
*******************************************************************************/
void MyIIC_Ack(void)
{
	MyIIC_SCL(0);
	MyIIC_SDA_OUT();
	MyIIC_SDA(0);
	delay_iic(1);
	MyIIC_SCL(1);
	delay_iic(1);
	MyIIC_SCL(0);
}

/*******************************************************************************
* ������ : void MyIIC_NAck(void)
* ��  �� : ������ ACK Ӧ��
* ��  �� : ��
* ��  �� : ��
* ����ֵ : 1������Ӧ��ʧ��
*          0������Ӧ��ɹ�
*******************************************************************************/	    
void MyIIC_NAck(void)
{
	MyIIC_SCL(0);
	MyIIC_SDA_OUT();
	MyIIC_SDA(1);
	delay_iic(1);
	MyIIC_SCL(1);
	delay_iic(1);
	MyIIC_SCL(0);
}			

/*******************************************************************************
* ������ : void MyIIC_Send_Byte(uint8_t txd)
* ��  �� : MyIIC����һ���ֽ�
* ��  �� : ��
* ��  �� : ��
* ����ֵ : 1����Ӧ��
*          0����Ӧ��
*******************************************************************************/		  
void MyIIC_Send_Byte(uint8_t txd)
{                        
	uint8_t t;   
	MyIIC_SDA_OUT(); 	    
	MyIIC_SCL(0);//����ʱ�ӿ�ʼ���ݴ���
	for(t=0;t<8;t++)
	{              
		MyIIC_SDA((txd&0x80)>>7);
		txd<<=1; 	  
		delay_iic(1);   //��TEA5767��������ʱ���Ǳ����
		MyIIC_SCL(1);
		delay_iic(1); 
		MyIIC_SCL(0);	
		delay_iic(1);
	}	 
} 	 
/*******************************************************************************
* ������ : uint8_t MyIIC_Read_Byte(unsigned char ack)
* ��  �� : ��ȡһ���ֽ�
* ��  �� : ack:1������ACK;0������nACK
* ��  �� : ��
* ����ֵ : 1������Ӧ��ʧ��
*          0������Ӧ��ɹ�
*******************************************************************************/
uint8_t MyIIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	MyIIC_SDA_IN();//SDA����Ϊ����
	for(i=0;i<8;i++ )
	{
		MyIIC_SCL(0); 
		delay_iic(1);
		MyIIC_SCL(1);
		receive<<=1;
		if(MyIIC_READ_SDA) receive++;   
			delay_iic(1); 
	}					 
	if (!ack)
		MyIIC_NAck();//����nACK
	else
		MyIIC_Ack(); //����ACK   
	return receive;
}


