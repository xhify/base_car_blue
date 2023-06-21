#pragma once

#include "Basic.hpp"

//��ʱʱ��
#define TIMEOUT 2.0*configTICK_RATE_HZ
	
/*RGB565��ɫ*/
#define  BLACK	0x0000			// ��ɫ
#define  NAVY	0x000F				// ����ɫ
#define  DGREEN  0x03E0     // ����ɫ
#define  DCYAN   0x03EF     // ����ɫ
#define  MAROON  0x7800     // ���ɫ
#define  PURPLE  0x780F     // ��ɫ
#define  OLIVE   0x7BE0     // �����
#define  LGRAY   0xC618     // �Ұ�ɫ
#define  DGRAY   0x7BEF     // ���ɫ
#define  BLUE   0x001F      // ��ɫ
#define  GREEN   0x07E0     // ��ɫ
#define  CYAN   0x07FF      // ��ɫ
#define  RED    0xF800      // ��ɫ
#define  MAGENTA  0xF81F    // Ʒ��
#define  YELLOW  0xFFE0     // ��ɫ
#define  WHITE   0xFFFF     // ��ɫ
/*RGB565��ɫ*/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);//ָ�����������ɫ
void LCD_DrawPoint(u16 x,u16 y,u16 color);//��ָ��λ�û�һ����
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);//��ָ��λ�û�һ����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);//��ָ��λ�û�һ������
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);//��ָ��λ�û�һ��Բ

//������ʾ�����⣬Ϊc++ c �������ĳ�ͻ
//void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ���ִ�
//void LCD_ShowChinese12x12(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����12x12����
//void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����16x16����
//void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����24x24����
//void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����32x32����

void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾһ���ַ�
void LCD_ShowString(u16 x,u16 y,const char *p,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ�ַ���
u32 mypow(u8 m,u8 n);//����
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey);//��ʾ��������
void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey);//��ʾ��λС������
void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[]);//��ʾͼƬ
void init_GUI();

extern int picture_id;