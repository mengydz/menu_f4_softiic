#ifndef __MENU_H
#define __MENU_H
 
#include <string.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "oled12832.h"
#include "stdio.h"

#define linenumber 2
struct MenuItem
{
	short MenuCount;//当前菜单项目总数
	uint8_t *DisplayString;//当前项目要显示的字符
	void(*Subs)();//选择某一菜单后执行的功能函数
	struct MenuItem *ChildrenMenus;//当前项目的子菜单
	struct MenuItem *ParentMenus;//当前项目的父菜单
};
struct mydata
{
	float length1;
	float length2;
	float angle;
};

void MenuInit(void);
void caidan(void);
int key_scan(void);//是否按下确认键
void display(struct MenuItem * MenuPoint,int selectItem_current,int selectItem_hidden);
void fun1(void);
void fun2(void);
void fun3(void);
void fun4(void);
void fun5(void);
void fun6(void);

#endif/*__MENU_H*/

