#include "menu.h"
#include "gpio.h"

struct mydata rmydata;
int selectItem_current,selectItem_hidden,selectItem;
int juli,jiaodu;

int32_t my_min(int32_t a,int32_t b)
{
	if (a<b)return a;
	return b;
}
//由于定义从上往下，数组定义时子类都为NULL
//第一级
struct MenuItem MainMenu[] = {
{ 8,(uint8_t *)"Page One",NULL,NULL,NULL },
{ 6,(uint8_t *)"Page Two",NULL,NULL,NULL },
{ 6,(uint8_t *)"Page Three",NULL,NULL,NULL },
{ 6,(uint8_t *)"Page Four",NULL,NULL,NULL },
{ 6,(uint8_t *)"Page Five",NULL,NULL,NULL },
{ 6,(uint8_t *)"Page Six",NULL,NULL,NULL },
{ 6,(uint8_t *)"Parameter Init",fun2,NULL,NULL },
{ 6,(uint8_t *)"Parameter Show",fun3,NULL,NULL }
};
//第二级
struct MenuItem Setmenu1[2]={
{ 2,(uint8_t *)"Return",NULL,NULL,MainMenu },
{ 2,(uint8_t *)"Enter",NULL,NULL,MainMenu },
};
struct MenuItem Setmenu2[] = {
{ 4,(uint8_t *)"Return",NULL,NULL,MainMenu },
{ 3,(uint8_t *)"Parameter Read",fun3,NULL,MainMenu },
{ 3,(uint8_t *)"Parameter Len",fun4,NULL,MainMenu },
{ 3,(uint8_t *)"Enter",fun1,NULL,MainMenu } };
struct MenuItem Setmenu3[] = {
{ 4,(uint8_t *)"Return",NULL,NULL,MainMenu },
{ 3,(uint8_t *)"Parameter Len",fun5,NULL,MainMenu },
{ 3,(uint8_t *)"Parameter Ang",fun6,NULL,MainMenu },
{ 3,(uint8_t *)"Enter",NULL,NULL,MainMenu } };
struct MenuItem* MenuPoint=MainMenu;//当前菜单

void MenuInit(void)
{
	MainMenu[0].ChildrenMenus = Setmenu1;
	MainMenu[1].ChildrenMenus = Setmenu2;
	MainMenu[2].ChildrenMenus = Setmenu3;
	selectItem_current =1;
	selectItem_hidden  =0;
	selectItem=selectItem_current+selectItem_hidden;
	display(MenuPoint,selectItem_current,selectItem_hidden);
}

void display(struct MenuItem * MenuPoint,int selectItem_current,int selectItem_hidden)
{
	uint16_t x=16;
	uint16_t y=0;
	OLED_ShowString(0,(selectItem_current*2-2)*8,(char *)"->",16,1);
	for (uint16_t j= selectItem_hidden; j < my_min(MenuPoint->MenuCount,linenumber+selectItem_hidden);j++)
	{
		OLED_ShowString(x,y*2*8,(char *)MenuPoint[j].DisplayString,16,1);
		y+=1;
	}
}

void caidan(void)
{
    int16_t key_num=KEY_Scan();
	if(key_num>0)
	switch(key_num)
	{
		case 1:
		{//上
			OLED_Clear();
			selectItem_current--;
			if (selectItem_current==0)
			{
				if(selectItem_hidden>0)selectItem_hidden--;
				selectItem_current++;
			}
			selectItem=selectItem_current+selectItem_hidden;
			display(MenuPoint,selectItem_current,selectItem_hidden);
		};break;
		case 2:
		{//下
			OLED_Clear();
			selectItem_current++;
			if(selectItem_current>linenumber)//当前的行数超过总行数
			{
				if (selectItem_current+selectItem_hidden <= MenuPoint->MenuCount)
					selectItem_hidden++;
				selectItem_current--;
			}
			else if(selectItem_current>MenuPoint->MenuCount)selectItem_current--;
			selectItem=selectItem_current+selectItem_hidden;
			display(MenuPoint,selectItem_current,selectItem_hidden);
		};break;
		case 3:
		{//右 确认进入此项目
			if(selectItem== 1&&MenuPoint!=MainMenu)//满足退回上一级条件
			{
					OLED_Clear();
					MenuPoint = MenuPoint[selectItem - 1].ParentMenus;
					selectItem_current =1;
					selectItem_hidden  =0;
					//selectItem = 1;
					display(MenuPoint,selectItem_current,selectItem_hidden);
			}
			else if (MenuPoint[selectItem - 1].ChildrenMenus != NULL)//判断是否有下一级
			{				
				OLED_Clear();
				MenuPoint = MenuPoint[selectItem - 1].ChildrenMenus;
				selectItem_current =1;
				selectItem_hidden  =0;
				display(MenuPoint,selectItem_current,selectItem_hidden);
			}
			else if(MenuPoint[selectItem - 1].Subs != NULL)
				MenuPoint[selectItem - 1].Subs();//功能函数执行
		};break;
	}
}
void fun1()
{
	OLED_ShowString(16,0*16,(char*)"fun1",16,1);
}
void fun2()//初始化参数
{
	rmydata.length1=1.23;
	rmydata.length2=2.34;
	rmydata.angle=10.2;
//	STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&rmydata,sizeof(rmydata));
}
void fun3()//读取并显示
{
	uint8_t key;
	char str[10];
	OLED_Clear();
//	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)&rmydata,sizeof(rmydata));
	sprintf(str,"length 1:%.2f",(float)rmydata.length1);
	OLED_ShowString(16,0*8,(char*)str,16,1);
	sprintf((char*)str,"length 2:%.2f",(float)rmydata.length2);
	OLED_ShowString(16,2*8,(char*)str,16,1);
	while(1)
	{
		key=KEY_Scan();
		for(uint32_t key_delay=0;key_delay<1000;key_delay++);//去抖动 
		if(key!=0)
		{
			OLED_Clear();
			display(MenuPoint,selectItem_current,selectItem_hidden);
			return;
		}
	}
}

void fun4()
{
	char str[10];
	uint8_t lengthwei[3],wei=2,jieshu=0;//0 个位 ,1小数点后1位  2小数点后2位 
	int current_length;
	uint8_t i,key;
	int time=0;
	current_length=rmydata.length1*100;
	lengthwei[0]=current_length/100;
	lengthwei[1]=current_length%100/10;
	lengthwei[2]=current_length%10;
	OLED_Clear();
	while(1)
	{
		key=KEY_Scan();
		for(uint32_t key_delay=0;key_delay<1000;key_delay++);//去抖动 
		if(key!=0)
		{
			if(key==1)jieshu=!jieshu;
			else if(key==2)
			{
				if(jieshu==0)wei++;
				else if(wei<=2) lengthwei[wei]--;
			}
			else if(key==3)
			{
				OLED_Clear();
				display(MenuPoint,selectItem_current,selectItem_hidden);
				rmydata.length1=current_length*0.01;
//				STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&rmydata,sizeof(rmydata));
				return;
			}
			else if(key==4)
			{
				if(jieshu==0)wei--;
				else if(wei<=2) lengthwei[wei]++;
			}
			if(wei>=3)wei=2;
			OLED_Clear();
		}          
		time++;      
		if(time<=20)
		{//.前一位的正下方显示^
			if(wei==0)
                OLED_ShowString(8*5,2*8,(char*)"^",16,1);
			else //.后的某一位正下方显示↑
                OLED_ShowString(8*6+8*wei,2*8,(char*)"^",16,1);
		}
		else if(time<=40)
        {
            if(jieshu==0)
                OLED_ShowString(0,2*8,(char*)"                ",16,1);
        }
        else 
            time=0;
		for(i=0;i<3;i++)if(lengthwei[wei]>=10)lengthwei[wei]=9;	
		current_length=lengthwei[0]*100+lengthwei[1]*10+lengthwei[2];
		sprintf((char*)str,"length:%.2f",current_length*0.01);
		OLED_ShowString(0,0*8,(char*)str,16,1);
	}
}
void fun5()
{
	char str[10];
	uint8_t lengthwei[3],wei=2,jieshu=0;//0 个位 ,1小数点后1位  2小数点后2位 
	int current_length;
	uint8_t i,key;
	int time=0;
	current_length=rmydata.length2*100;
	lengthwei[0]=current_length/100;
	lengthwei[1]=current_length%100/10;
	lengthwei[2]=current_length%10;
	OLED_Clear();
	while(1)
	{
		key=KEY_Scan();
		for(uint32_t key_delay=0;key_delay<1000;key_delay++);//去抖动 
		if(key!=0)
		{
			if(key==1)jieshu=!jieshu;
			else if(key==2)
			{
				if(jieshu==0)wei++;
				else if(wei<=2) lengthwei[wei]--;
			}
			else if(key==3)
			{
				OLED_Clear();
				display(MenuPoint,selectItem_current,selectItem_hidden);
				rmydata.length2=current_length*0.01;
//				STMFLASH_Write(FLASH_SAVE_ADDR,(uint16_t*)&rmydata,sizeof(rmydata));
				return;
			}
			else if(key==4)
			{
				if(jieshu==0)wei--;
				else if(wei<=2) lengthwei[wei]++;
			}
			if(wei>=3)wei=2;
			OLED_Clear();
		}
		time++;
		if(time<=20)
		{//.前一位的正下方显示^
			if(wei==0)
                OLED_ShowString(8*5,2*8,(char*)"^",16,1);
			else //.后的某一位正下方显示↑
                OLED_ShowString(8*6+8*wei,2*8,(char*)"^",16,1);
		}
		else if(time<=40)
        {
            if(jieshu==0)
                OLED_ShowString(0,2*8,(char*)"                ",16,1);}
            else 
                time=0;
		for(i=0;i<3;i++)if(lengthwei[wei]>=10)lengthwei[wei]=9;	
		current_length=lengthwei[0]*100+lengthwei[1]*10+lengthwei[2];
		sprintf((char*)str,"length:%.2f",current_length*0.01);
		OLED_ShowString(0,0*8,(char*)str,16,1);
	}
}

void fun6()
{
	char str[10];
	uint8_t lengthwei[3],wei=2,jieshu=0;//0 个位 ,1小数点后1位  2小数点后2位 
	int current_angle;
	uint8_t i,key;
	int time=0;
	current_angle=rmydata.angle*10;
	lengthwei[0]=current_angle/100;
	lengthwei[1]=current_angle%100/10;
	lengthwei[2]=current_angle%10;
	OLED_Clear();
	//key=5;
	while(1)
	{
		key=KEY_Scan();
		for(uint32_t key_delay=0;key_delay<1000;key_delay++);//去抖动 
		if(key!=0)
		{
			if(key==1)jieshu=!jieshu;
			else if(key==2)
			{
				if(jieshu==0)wei++;
				else if(wei<=2) lengthwei[wei]--;
			}
			else if(key==3)
			{
				OLED_Clear();
				display(MenuPoint,selectItem_current,selectItem_hidden);
				rmydata.angle=current_angle*0.1;
//				STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&rmydata,sizeof(rmydata));
				return;
			}
			else if(key==4)
			{
				if(jieshu==0)wei--;
				else if(wei<=2) lengthwei[wei]++;
			}
			if(wei>=3)wei=2;
			OLED_Clear();
		}
		time++;
		if(time<=20)
		{
			if(wei==2)
                OLED_ShowString(8*8,2*8,(char*)"^",16,1);
			else 
                OLED_ShowString(8*5+8*wei,2*8,(char*)"^",16,1);
		}
		else if(time<=40)
        {
            if(jieshu==0)
                OLED_ShowString(0,2*8,(char*)"                ",16,1);}
            else 
                time=0;
		for(i=0;i<3;i++)if(lengthwei[wei]>=10)lengthwei[wei]=9;	
		current_angle=lengthwei[0]*100+lengthwei[1]*10+lengthwei[2];
		sprintf((char*)str,"angle:%2.1f",current_angle*0.1f);
		OLED_ShowString(0,0*8,(char *)str,16,1);
	}
}
