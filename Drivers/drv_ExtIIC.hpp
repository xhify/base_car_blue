#pragma once

#ifdef __cplusplus
	extern "C" {
#endif

#include "sys.h"
#include "stdbool.h"
#include "stdint.h"
		
//Ϊ1����ʹ��Ӳ��IIC
#define IIC_CONFIG 0
		
//IO��������
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)8<<12;}	//SDA����ģʽ
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)3<<12;}  //SDA���ģʽ
#define SCL_OUT() {GPIOB->CRH&=0XFFFFF0FF;GPIOB->CRH|=(u32)3<<8;} 	//SCL���ģʽ

#define SCL_PORT GPIOB												//SCL�˿�
#define SDA_PORT GPIOB												//SDA�˿�
#define SCL_PIN GPIO_Pin_10										//SCL�ܽ�
#define SDA_PIN GPIO_Pin_11										//SDA�ܽ�

//IO��������	 
#define IIC_SCL_High()  GPIO_SetBits(SCL_PORT,SCL_PIN) 				//SCL_High
#define IIC_SCL_Low()   GPIO_ResetBits(SCL_PORT,SCL_PIN) 			//SCL_Low
#define IIC_SDA_High()  GPIO_SetBits(SDA_PORT,SDA_PIN) 				//SDA_High
#define IIC_SDA_Low()   GPIO_ResetBits(SDA_PORT,SDA_PIN) 			//SDA_Low

#define READ_SDA    PBin(11)  										//����SDA 
#define IIC_SCL     PBout(10) 										//SCL
#define IIC_SDA     PBout(11) 										//SDA	

//���IIC��������
void IIC_Delay(void);													//IIC��ʱ����
void IIC_Init(void);                					//��ʼ��IIC��IO��				 
void IIC_Start(void);													//����IIC��ʼ�ź�
void IIC_Stop(void);	  											//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);										//IIC����һ���ֽ�
void IIC_Ack(void);														//IIC����ACK�ź�
void IIC_NAck(void);													//IIC������ACK�ź�
u8 IIC_Read_Byte(unsigned char ack);					//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 												//IIC�ȴ�ACK�ź�
u8 Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);	//IIC����д
u8 Read_Len(u8 addr,u8 reg,u8 len,u8 *buf); 	//IIC������ 
u8 I2C_ReadOneByte(u8 I2C_Addr,u8 addr);
// u8 soft_i2c_buffer_read(u8 devAddr, u8 regAddr,u8 *pBuffer, u8 numByteToRead);

//Ӳ��IIC��������
/*������֤ͨ��������
	����֮��������
	Sync_waitTime����ʱʱ��
*/
bool Lock_ExtIIC( double Sync_waitTime);
void Unlock_ExtIIC();
/*
	7λ��ַ���͵�������
	devaddr��7λ������ַ
	addr��7λ�����Ĵ�����ַ
	datas��Ҫ���͵�����ָ��
	Sync_waitTime����ʱʱ��
*/
bool ExtIIC_SendOneAddr7(uint8_t devaddr, uint8_t addr, const uint8_t tx_data, double Sync_waitTime );
/*7λ��ַ��������
	addr��7λ��������ַ
	datas��Ҫ���͵�����ָ��
	length�����ݳ���
	Sync_waitTime����ʱʱ��
*/
bool ExtIIC_SendAddr7(uint8_t devaddr, uint8_t addr, uint16_t tx_length, const uint8_t* tx_datas);

/*7λ��ַ��������
	addr��7λ��������ַ
	datas��Ҫ���͵�����ָ��
	length�����ݳ���
	Sync_waitTime����ʱʱ��
*/
bool ExtIIC_ReceiveOneAddr7(uint8_t devaddr, uint8_t addr, uint8_t rx_data, double Sync_waitTime );

/*7λ��ַ������������
	addr��7λ��������ַ
	datas��Ҫ���͵�����ָ��
	length�����ݳ���
	Sync_waitTime����ʱʱ��
*/
bool ExtIIC_ReceiveAddr7(uint8_t devaddr, uint8_t addr, uint16_t rx_length, uint8_t* rx_datas);

void init_drv_ExtIIC();

#ifdef __cplusplus
	}
#endif
	

	