#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "sys.h"

#define USE_HORIZONTAL 0  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����


#define LCD_W 240
#define LCD_H 240



//-----------------LCD�˿ڶ���---------------- 
//GND   ��Դ��
//VCC   3.3v��Դ
//SCL   ��SCLK��PE3
//SDA   ��MOSI��PE5
//RES   PE4
//DC    PE2
//CS    PE1
//BLK   PE6

#define LCD_SCLK_Clr() GPIO_ResetBits(GPIOE,GPIO_Pin_3)//SCL=SCLK
#define LCD_SCLK_Set() GPIO_SetBits(GPIOE,GPIO_Pin_3)

#define LCD_MOSI_Clr() GPIO_ResetBits(GPIOE,GPIO_Pin_5)//SDA=MOSI
#define LCD_MOSI_Set() GPIO_SetBits(GPIOE,GPIO_Pin_5)

#define LCD_RES_Clr()  GPIO_ResetBits(GPIOE,GPIO_Pin_4)//RES
#define LCD_RES_Set()  GPIO_SetBits(GPIOE,GPIO_Pin_4)

#define LCD_DC_Clr()   GPIO_ResetBits(GPIOE,GPIO_Pin_2)//DC
#define LCD_DC_Set()   GPIO_SetBits(GPIOE,GPIO_Pin_2)
 		     
#define LCD_CS_Clr()   GPIO_ResetBits(GPIOE,GPIO_Pin_1)//CS
#define LCD_CS_Set()   GPIO_SetBits(GPIOE,GPIO_Pin_1)

#define LCD_BLK_Clr()  GPIO_ResetBits(GPIOE,GPIO_Pin_6)//BLK
#define LCD_BLK_Set()  GPIO_SetBits(GPIOE,GPIO_Pin_6)

void LCD_GPIO_Init(void);//��ʼ��GPIO
void LCD_Writ_Bus(u8 dat);//ģ��SPIʱ��
void LCD_WR_DATA8(u8 dat);//д��һ���ֽ�
void LCD_WR_DATA(u16 dat);//д�������ֽ�
void LCD_WR_REG(u8 dat);//д��һ��ָ��
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//�������꺯��
void init_drv_LCD(void);//LCD��ʼ��
#endif




