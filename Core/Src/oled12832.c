#include "oled12832.h"
#include "stdlib.h"
#include "oledfont.h"  
#include "myiic.h"
//////////////////////////////////////////////////////////////////////////////////	  

//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
uint8_t OLED_GRAM[128][4];	 

/**********************************************
// IIC Write Command
**********************************************/
void OLED_Write_IIC_Command(uint8_t IIC_Command)
{
    MyIIC_Start();
    MyIIC_Send_Byte(0x78);				//Slave address,SA0=0
    MyIIC_Wait_Ack();
    MyIIC_Send_Byte(0x00);				//write command
    MyIIC_Wait_Ack();
    MyIIC_Send_Byte(IIC_Command);
    MyIIC_Wait_Ack();
    MyIIC_Stop();
}

/**********************************************
// IIC Write Data
**********************************************/
void OLED_Write_IIC_Data(uint8_t IIC_Data)
{
    MyIIC_Start();
    MyIIC_Send_Byte(0x78);				//D/C#=0; R/W#=0
    MyIIC_Wait_Ack();
    MyIIC_Send_Byte(0x40);				//write data
    MyIIC_Wait_Ack();
    MyIIC_Send_Byte(IIC_Data);
    MyIIC_Wait_Ack();
    MyIIC_Stop();
}

void OLED_Write_IIC_Datas(uint8_t *IIC_Data,uint8_t len)
{
    MyIIC_Start();
    MyIIC_Send_Byte(0x78);				//D/C#=0; R/W#=0
    MyIIC_Wait_Ack();
    MyIIC_Send_Byte(0x40);				//write data
    MyIIC_Wait_Ack();
    for(uint8_t cnt=0;cnt<len;cnt++)
    {
        MyIIC_Send_Byte(*(IIC_Data+cnt));
        MyIIC_Wait_Ack();
    }
    MyIIC_Stop();
}

//�����Դ浽LCD		 
void OLED_Refresh_Gram(void)
{
	for(uint8_t i=0;i<8;i++)  
	{  
		OLED_Write_IIC_Command (0xb0+i);    //����ҳ��ַ��0~7��
		OLED_Write_IIC_Command (0x00);      //������ʾλ�á��е͵�ַ
		OLED_Write_IIC_Command (0x10);      //������ʾλ�á��иߵ�ַ
        uint8_t temp_gram[128];
        
		for(uint8_t n=0;n<128;n++)
        temp_gram[n] = OLED_GRAM[n][i];
        OLED_Write_IIC_Datas(temp_gram,128); 
	}   
}

//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_Write_IIC_Command(0X8D);  //SET DCDC����
	OLED_Write_IIC_Command(0X14);  //DCDC ON
	OLED_Write_IIC_Command(0XAF);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_Write_IIC_Command(0X8D);  //SET DCDC����
	OLED_Write_IIC_Command(0X10);  //DCDC OFF
	OLED_Write_IIC_Command(0XAE);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	for(uint8_t i=0;i<4;i++)
    {
        for(uint8_t n=0;n<128;n++)
        OLED_GRAM[n][i]=0X00; 
    }
	OLED_Refresh_Gram();//������ʾ
}
//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>31)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)
        OLED_GRAM[x][pos]|=temp;
	else 
        OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)  
{  
	for(uint8_t x=x1;x<=x2;x++)
	{
		for(uint8_t y=y1;y<=y2;y++)
            OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//������ʾ
}

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{      			    
	uint8_t temp;
	uint8_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
    for(uint8_t t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t];	//����1608����
		else if(size==16)temp=asc2_1608[chr][t];	//����1608����
		else if(size==24)temp=asc2_2412[chr][t];	//����2412����
		else return;								//û�е��ֿ�
        for(uint8_t t1=0;t1<8;t1++)
		{
			if(temp&0x80)
                OLED_DrawPoint(x,y,mode);
			else 
                OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//m^n����
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//��ʾ�ַ���
//x,y:�������  
//size:�����С 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowString(uint8_t x,uint8_t y,const char *p,uint8_t size,uint8_t mode)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(32-size))
        {
            y=x=0;
            OLED_Clear();
        }
        OLED_ShowChar(x,y,*p,size,mode);	 
        x+=size/2;
        p++;
    }  
	OLED_Refresh_Gram();//������ʾ
}	
void OLED_Init(void)
{ 	 				 	 					     		  
	OLED_Write_IIC_Command(0xAF);//--turn on oled panel

	OLED_Write_IIC_Command(0xA4);//--Entire Display ON


    OLED_Write_IIC_Command(0xAE);//--display off
    
	OLED_Write_IIC_Command(0x20);
	OLED_Write_IIC_Command(0x02);
    
	OLED_Write_IIC_Command(0x00);//---set low column address
	OLED_Write_IIC_Command(0x10);//---set high column address
    
 	OLED_Write_IIC_Command(0xB0);//---set pages address
   
	OLED_Write_IIC_Command(0x40);//--set start line address  
    
	OLED_Write_IIC_Command(0x81); // contract control
	OLED_Write_IIC_Command(0xFF);//--128
    
	OLED_Write_IIC_Command(0xA1);//set segment remap 
	OLED_Write_IIC_Command(0xC0);//Com scan direction
	OLED_Write_IIC_Command(0xA6);//--normal / reverse
    
	OLED_Write_IIC_Command(0xA8);//--set multiplex ratio(1 to 64)
	OLED_Write_IIC_Command(0x1F);//--1/32 duty
    
	OLED_Write_IIC_Command(0xD3);//-set display offset
	OLED_Write_IIC_Command(0x00);//
    
	OLED_Write_IIC_Command(0xD5);//set osc division
	OLED_Write_IIC_Command(0x80);
    
	OLED_Write_IIC_Command(0xD9);//Set Pre-Charge Period
	OLED_Write_IIC_Command(0xF1);//
    
	OLED_Write_IIC_Command(0xDA);//set com pin configuartion
	OLED_Write_IIC_Command(0x02);//
    
	OLED_Write_IIC_Command(0xDB);//set Vcomh
	OLED_Write_IIC_Command(0x40);//set start line
    
	OLED_Write_IIC_Command(0x8D);//set charge pump enable
	OLED_Write_IIC_Command(0x14);//
    
	OLED_Write_IIC_Command(0xA4);//display accordding to GDDRAM
	OLED_Write_IIC_Command(0xA6);//set normal display
    
	OLED_Write_IIC_Command(0xAF);//--turn on oled panel
	OLED_Clear();
}  


