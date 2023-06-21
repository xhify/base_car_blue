#include "mpuiic.h"
#include "inv_mpu.h"
#include "delay.h"
//#include "Commulink.hpp"
//#include "Basic.hpp"

//IIC��������
	#define IIC_NACK (0<<10)
	#define IIC_ACK (1<<10)
	#define IIC_STOP (1<<9)
	#define IIC_START (1<<8)

//IIC״̬
	#define IIC_OUT (1<<14)
	#define IIC_PECERR (1<<12)
	#define IIC_OVR (1<<11)
	#define IIC_NACKF (1<<10)
	#define IIC_ARLO (1<<9)
	#define IIC_BERR (1<<8)
	#define IIC_TE (1<<7)
	#define IIC_RXNE (1<<6)
	#define IIC_STOPF (1<<4)
	#define IIC_BTF (1<<2)
	#define IIC_ADDR (1<<1)
	#define IIC_SB (1<<0)
	//���IIC����
		/*
	 * �������ܣ�IIC ��ʱ����
	 */
	void IIC_Delay(void)
	{
		delay_us(2);
	}
		/**************************************************************************
	�������ܣ���ȡָ���豸ָ���Ĵ�����һ��ֵ
	��ڲ�����I2C_Addr���豸IIC��ַ��addr:�Ĵ�����ַ
	����  ֵ��res����ȡ������
	**************************************************************************/ 
	unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
	{
		unsigned char res=0;
		IIC_Start();	
		//����д����
		IIC_Send_Byte(I2C_Addr);	   		
		res++;
		IIC_Wait_Ack();
		//���͵�ַ
		IIC_Send_Byte(addr); res++;  		
		IIC_Wait_Ack();	  
		IIC_Start();
		//�������ģʽ	
		IIC_Send_Byte(I2C_Addr+1); res++;   		   
		IIC_Wait_Ack();
		res=IIC_Read_Byte(0);	   
		//����һ��ֹͣ����
		IIC_Stop();							
		return res;
	}
	/*
	 * �������ܣ���ʼ�����IIC
	 */
	void IIC_Init(void)
	{					     
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );	
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP ;                            
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOB,&GPIO_InitStructure);
//		IIC_SCL=1;
//		IIC_SDA=1;
	}
		/*
	 * �������ܣ�����IIC��ʼ�ź�
	 */
	void IIC_Start(void)
	{
		//SDA�����
		SDA_OUT();     	
		IIC_SDA=1;	  	  
		IIC_SCL=1;
		delay_us(4);
		//��ʼ����CLKΪ�ߣ������ɸߵ���
		IIC_SDA=0;		
		delay_us(4);
		//ǯסI2C���ߣ�׼�����ͻ�������� 
		IIC_SCL=0;		
	}
	/*
	 * �������ܣ�����IICֹͣ�ź�
	 */
	void IIC_Stop(void)
	{
		//SDA�����
		SDA_OUT();
		IIC_SCL=0;
		//ֹͣ����CLKΪ�ߣ������ɵ͵���
		IIC_SDA=0;
		delay_us(4);
		IIC_SCL=1; 
		//����I2C���߽����ź�
		IIC_SDA=1;
		delay_us(4);							   	
	}
	/*
	 * �������ܣ��ȴ�Ӧ���źŵ���
	 * ����ֵ:1������Ӧ��ʧ�� 0������Ӧ��ɹ�
	 */
	u8 IIC_Wait_Ack(void)
	{
		u8 ucErrTime=0;
		//SDA����Ϊ����  
		SDA_IN();      
		IIC_SDA=1;delay_us(1);	   
		IIC_SCL=1;delay_us(1);	 
		while(READ_SDA)
		{
			ucErrTime++;
			if(ucErrTime>250)
			{
				IIC_Stop();
				return 1;
			}
		}
		//ʱ�����0 
		IIC_SCL=0;	   
		return 0;  
	} 

	/*
	 * �������ܣ�����ACKӦ��
	 */
	void IIC_Ack(void)
	{
		IIC_SCL=0;
		SDA_OUT();
		IIC_SDA=0;
		delay_us(2);
		IIC_SCL=1;
		delay_us(2);
		IIC_SCL=0;
	}

	/*
	 * �������ܣ�������ACKӦ��
	 */	    
	void IIC_NAck(void)
	{
		IIC_SCL=0;
		SDA_OUT();
		IIC_SDA=1;
		delay_us(2);
		IIC_SCL=1;
		delay_us(2);
		IIC_SCL=0;
	}	
	//IIC����һ���ֽ�
	//���شӻ�����Ӧ��
	//1����Ӧ��
	//0����Ӧ��			  
	void IIC_Send_Byte(u8 txd)
	{                        
		u8 t;   
		SDA_OUT(); 	    
		IIC_SCL=0;                                                                 
		for(t=0;t<8;t++)
		{              
			IIC_SDA=(txd&0x80)>>7;
			txd<<=1; 	  
			delay_us(2);                                                          
			IIC_SCL=1;
			delay_us(2); 
			IIC_SCL=0;	
			delay_us(2);
		}	 
	} 	
	//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
	u8 IIC_Read_Byte(unsigned char ack)
	{
		unsigned char i,receive=0;
		SDA_IN();                                                                  
		for(i=0;i<8;i++ )
		{
				IIC_SCL=0; 
				delay_us(2);
		IIC_SCL=1;
				receive<<=1;
				if(READ_SDA)receive++;   
		delay_us(1); 
		}					 
		if (!ack)
				IIC_NAck();                                                           
		else
				IIC_Ack();                                                            
		return receive;
	}
	//IIC����д
	//addr:������ַ 
	//reg:�Ĵ�����ַ
	//len:д�볤��
	//buf:������
	//����ֵ:0,����
	//    ����,�������
	u8 Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
	{
		u8 i; 
		IIC_Start(); 
		//����������ַ+д����
		IIC_Send_Byte((addr<<1)|0);	
		//�ȴ�Ӧ��
		if(IIC_Wait_Ack())	
		{
			IIC_Stop();		 
			return 1;		
		}
		//д�Ĵ�����ַ
		IIC_Send_Byte(reg);	
		//�ȴ�Ӧ��
		IIC_Wait_Ack();		
		for(i=0;i<len;i++)
		{
			//��������
			IIC_Send_Byte(buf[i]);	
			//�ȴ�ACK
			if(IIC_Wait_Ack())		
			{
				IIC_Stop();	 
				return 1;		 
			}		
		}    
		IIC_Stop();	 
		return 0;	
	} 

	//IIC������
	//addr:������ַ
	//reg:Ҫ��ȡ�ļĴ�����ַ
	//len:Ҫ��ȡ�ĳ���
	//buf:��ȡ�������ݴ洢��
	//����ֵ:0,����
	//    ����,�������
	u8 Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
	{ 
		IIC_Start(); 
		//����������ַ+д����	
		IIC_Send_Byte((addr<<1)|0);
		//�ȴ�Ӧ��
		if(IIC_Wait_Ack())	
		{
			IIC_Stop();		 
			return 1;		
		}
		//д�Ĵ�����ַ
		IIC_Send_Byte(reg);	
		//�ȴ�Ӧ��
		IIC_Wait_Ack();		
		IIC_Start();
		//����������ַ+������	
		IIC_Send_Byte((addr<<1)|1);
		//�ȴ�Ӧ�� 
		IIC_Wait_Ack();		
		while(len)
		{
			//������,����nACK 
			if(len==1)*buf=IIC_Read_Byte(0);
			//������,����ACK 
			else *buf=IIC_Read_Byte(1);		 
			len--;
			buf++; 
		}    
		//����һ��ֹͣ����
		IIC_Stop(); 
		return 0;	
	}