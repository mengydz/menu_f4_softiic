#include "oled12832.h"
#include "stdlib.h"
#include "oledfont.h"  
#include "myiic.h"
//////////////////////////////////////////////////////////////////////////////////	  

//OLED的显存
//存放格式如下.
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

//更新显存到LCD		 
void OLED_Refresh_Gram(void)
{
	for(uint8_t i=0;i<8;i++)  
	{  
		OLED_Write_IIC_Command (0xb0+i);    //设置页地址（0~7）
		OLED_Write_IIC_Command (0x00);      //设置显示位置—列低地址
		OLED_Write_IIC_Command (0x10);      //设置显示位置—列高地址
        uint8_t temp_gram[128];
        
		for(uint8_t n=0;n<128;n++)
        temp_gram[n] = OLED_GRAM[n][i];
        OLED_Write_IIC_Datas(temp_gram,128); 
	}   
}

//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_Write_IIC_Command(0X8D);  //SET DCDC命令
	OLED_Write_IIC_Command(0X14);  //DCDC ON
	OLED_Write_IIC_Command(0XAF);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_Write_IIC_Command(0X8D);  //SET DCDC命令
	OLED_Write_IIC_Command(0X10);  //DCDC OFF
	OLED_Write_IIC_Command(0XAE);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	for(uint8_t i=0;i<4;i++)
    {
        for(uint8_t n=0;n<128;n++)
        OLED_GRAM[n][i]=0X00; 
    }
	OLED_Refresh_Gram();//更新显示
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>31)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)
        OLED_GRAM[x][pos]|=temp;
	else 
        OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)  
{  
	for(uint8_t x=x1;x<=x2;x++)
	{
		for(uint8_t y=y1;y<=y2;y++)
            OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{      			    
	uint8_t temp;
	uint8_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	chr=chr-' ';//得到偏移后的值		 
    for(uint8_t t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t];	//调用1608字体
		else if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[chr][t];	//调用2412字体
		else return;								//没有的字库
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
//m^n函数
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
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
//显示字符串
//x,y:起点坐标  
//size:字体大小 
//*p:字符串起始地址 
void OLED_ShowString(uint8_t x,uint8_t y,const char *p,uint8_t size,uint8_t mode)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
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
	OLED_Refresh_Gram();//更新显示
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


