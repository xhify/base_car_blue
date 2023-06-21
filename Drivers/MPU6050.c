#include "MPU6050.h"
#include "mpuiic.h"
#include "inv_mpu.h"
//#include "filter.h"
#include "self_filter.h"
#include "delay.h"
#include <math.h>
//extern "Cplusplus"{
//#include "drv_LED.hpp"
//}

//#include <limits>
//using namespace std;
//�����Ǳ���
short gyro[3], accel[3], sensors;
//����Ƕ� Z��������
float Pitch,Roll,Yaw,Gryo_Z; 
float L_Pitch,L_Roll;
//���ٶȴ�����ԭʼ����
short aacx,aacy,aacz;	
//������ԭʼ����
short gyrox,gyroy,gyroz;	
float G_X,G_Z,G_Y;
float Accel_Y,Accel_Z,Accel_X,Gyro_X,Gyro_Z,Gyro_Y;
KalmanFilter kf_roll, kf_pitch;
//�¶�
short temp;			
static uint8_t _mpu_half_resolution;
 int Temperature; 
 float Angle_Balance,Gyro_Balance,Gyro_Turn; 
 float Acceleration_Z; 
short predata[6];
short ave_predata[6];
int16_t accRaw[3], gyroRaw[3];
int16_t temperuteRaw;
//float predata[6];
extern short ave_predata[6];

/*IIC����*/

	//IICдһ���ֽ� 
	//reg:�Ĵ�����ַ
	//data:����
	//����ֵ:0,����
	//����,�������
	u8 Write_Byte(u8 reg,u8 data) 				 
	{ 
		IIC_Start(); 
		IIC_Delay();
		//����������ַ+д����	
		IIC_Send_Byte((MPU_ADDR<<1)|0);
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
		//��������
		IIC_Send_Byte(data);
		//�ȴ�ACK
		if(IIC_Wait_Ack())	
		{
			IIC_Stop();	 
			return 1;		 
		}		 
		IIC_Stop();	 
		return 0;
	}
	/*
	 * �������ܣ�IIC��һ���ֽ� 
	 * reg���Ĵ�����ַ
	 * ����ֵ:����������
	 */
	u8 res;
	u8 Read_Byte(u8 reg)
	{
//		u8 res;
		IIC_Start(); 
		//����������ַ+д����	
		IIC_Send_Byte((MPU_ADDR<<1)|0);
		//�ȴ�Ӧ�� 
		IIC_Wait_Ack();		
		//д�Ĵ�����ַ
		IIC_Send_Byte(reg);	
		//�ȴ�Ӧ��
		IIC_Wait_Ack();		
		IIC_Start();
		//����������ַ+������	
		IIC_Send_Byte((MPU_ADDR<<1)|1);
		//�ȴ�Ӧ��
		IIC_Wait_Ack();		 
		//��ȡ����,����nACK 
		res=IIC_Read_Byte(0);
		//����һ��ֹͣ���� 
		IIC_Stop();			
		return res;		
	}
/*IIC����*/
	
/* MPU6050���� */
	
	/*
	 * �������ܣ���ʼ��MPU6050
	 * ����ֵ:0,�ɹ� ����,�������
	 */
	u8 MPU_Init(void)
	{ 
		u8 res;
		//��ʼ��IIC����
		IIC_Init();			
		//��λMPU6050
		Write_Byte(MPU_PWR_MGMT1_REG,0X80);	
		delay_ms(100);
		//����MPU6050
		Write_Byte(MPU_PWR_MGMT1_REG,0X00);	 
		//�����Ǵ�����,��2000dps 65536/4000=16.4 LSB
		MPU_Set_Gyro_Fsr(3);				
		//���ٶȴ�����,��8g 65536/16=4096 LSB
		MPU_Set_Accel_Fsr(2);		
		//���ò�����500Hz		
		MPU_Set_Rate(500);						
		//�ر������ж�
		Write_Byte(MPU_INT_EN_REG,0X00);	
		//I2C��ģʽ�ر�
		Write_Byte(MPU_USER_CTRL_REG,0X00);	
		//�ر�FIFO
		Write_Byte(MPU_FIFO_EN_REG,0X00);	
		//INT���ŵ͵�ƽ��Ч
		Write_Byte(MPU_INTBP_CFG_REG,0X80);	
		res=Read_Byte(MPU_DEVICE_ID_REG);
		//����CLKSEL,PLL Z��Ϊ�ο�
		Write_Byte(MPU_PWR_MGMT1_REG,0X03);	
		//���ٶ��������Ƕ�������������͹���ģʽ
		Write_Byte(MPU_PWR_MGMT2_REG,0X00);		
			int i=0;
	for(i=0;i<100;i++){
		MPU_Get_Gyroscope(&predata[0],&predata[1],&predata[2]);
		MPU_Get_Accelerometer(&predata[3],&predata[4],&predata[5]);
		ave_predata[0]+=predata[0]/100;
		ave_predata[1]+=predata[1]/100;
		ave_predata[2]+=predata[2]/100;
		ave_predata[3]+=predata[3]/100;
		ave_predata[4]+=predata[4]/100;
		ave_predata[5]+=predata[5]/100;
	}

		kalman_filter_init(&kf_roll, 0.0,0.005,0.003,0.001);
    kalman_filter_init(&kf_pitch, 0.0,0.005,0.003,0.001);
		return 0;
	}
	/*
	 * �������ܣ�����MPU6050�����Ǵ����������̷�Χ 
	 * fsr��0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
	 * ����ֵ:0,���óɹ� ����,����ʧ�� 
	 */
	u8 MPU_Set_Gyro_Fsr(u8 fsr)
	{
		//���������������̷�Χ  
		return Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);
	}
	/*
	 * �������ܣ�����MPU6050���ٶȴ����������̷�Χ
	 * fsr��0,��2g;1,��4g;2,��8g;3,��16g
	 * ����ֵ:0,���óɹ� ����,����ʧ�� 
	 */
	u8 MPU_Set_Accel_Fsr(u8 fsr)
	{
		//���ü��ٶȴ����������̷�Χ  
		return Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);
	}
	/*
	 * �������ܣ�����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
	 * rate��4~1000(Hz)
	 * ����ֵ:0,���óɹ� ����,����ʧ�� 
	 */
		u8 MPU_Set_LPF(u16 lpf)
	{
		u8 data=0;
		if(lpf>=188)data=1;
		else if(lpf>=98)data=2;
		else if(lpf>=42)data=3;
		else if(lpf>=20)data=4;
		else if(lpf>=10)data=5;
		else data=6; 
		//�������ֵ�ͨ�˲���  
		return Write_Byte(MPU_CFG_REG,data);
	}
	u8 MPU_Set_Rate(u16 rate)
	{
		u8 data;
		if(rate>1000)rate=1000;
		if(rate<4)rate=4;
		data=1000/rate-1;
		data=Write_Byte(MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
		return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
	}
/*
	 * �������ܣ��õ��¶�ֵ
	 * ����ֵ:�¶�ֵ(������10��)
	 */
	int MPU_Get_Temperature(void)
	{
		float Temp;
	  Temp=(Read_Byte(MPU_TEMP_OUTH_REG)<<8)+Read_Byte(MPU_TEMP_OUTL_REG);
		//��������ת��
		if(Temp>32768) 
			Temp-=65536;	
		//�¶ȷŴ�ʮ�����
		Temp=(36.53+Temp/340)*10;	  
	  return (int)Temp;
	}
	/*
	 * �������ܣ��õ�������ֵ(ԭʼֵ)
	 * gx,gy,gz��������x,y,z���ԭʼ����(������)
	 * ����ֵ:0,�ɹ� ����,������� 
	 */
	u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
	{
		u8 buf[6],res;  
		res=Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
		if(res==0)
		{
			*gx=((u16)buf[0]<<8)|buf[1];  
			*gy=((u16)buf[2]<<8)|buf[3];  
			*gz=((u16)buf[4]<<8)|buf[5];
		} 	
		return res;
	}
	/*
	 * �������ܣ��õ����ٶ�ֵ(ԭʼֵ)
	 * gx,gy,gz��������x,y,z���ԭʼ����(������)
	 * ����ֵ:0,�ɹ� ����,������� 
	 */
	u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
	{
		u8 buf[6],res;  
		res=Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
		if(res==0)
		{
			*ax=((u16)buf[0]<<8)|buf[1];  
			*ay=((u16)buf[2]<<8)|buf[3];  
			*az=((u16)buf[4]<<8)|buf[5];
		} 	
		return res;
	}
		/*
	 * �������ܣ���ȡMPU6050����DMP����̬��Ϣ
	 */
	void Read_DMP(void)
	{	
		mpu_dmp_get_data(&Pitch,&Roll,&Yaw);
	}
	/*
	 * �������ܣ���ȡMPU6050ԭʼ��Ϣ
	 */
	uint8_t MPU6050_read(int32_t* acc, int32_t* gyro)
	{		
		uint8_t IMUBuffer[14];
		uint8_t axis;
		
		if(!Read_Len(MPU_ADDR, 0x3B, 14, IMUBuffer))
		{
			accRaw[0]     = (int16_t)(((uint16_t)IMUBuffer[0] << 8)  | (uint16_t)IMUBuffer[1]);
			accRaw[1]     = (int16_t)(((uint16_t)IMUBuffer[2] << 8)  | (uint16_t)IMUBuffer[3]);
			accRaw[2]     = (int16_t)(((uint16_t)IMUBuffer[4] << 8)  | (uint16_t)IMUBuffer[5]);
			temperuteRaw  = (int16_t)(((uint16_t)IMUBuffer[6] << 8)  | (uint16_t)IMUBuffer[7]);
			gyroRaw[0]    = (int16_t)(((uint16_t)IMUBuffer[8] << 8)  | (uint16_t)IMUBuffer[9]);//
			gyroRaw[1]    = (int16_t)(((uint16_t)IMUBuffer[10] << 8) | (uint16_t)IMUBuffer[11]);//
			gyroRaw[2]    = (int16_t)(((uint16_t)IMUBuffer[12] << 8) | (uint16_t)IMUBuffer[13]);//	     
			
			for(axis=0; axis<3; axis++)
			{
				acc[axis] = accRaw[axis];
				if(_mpu_half_resolution) 
				{
					acc[axis] *= 2;
				}
				gyro[axis] = gyroRaw[axis];
			}    
			return 1; 
		}
		return 0;
	}
	/**************************************************************************
	�������ܣ���ȡ�Ƕ�	
	��ڲ�����way����ȡ�Ƕȵ��㷨 1��DMP  2�������� 3�������˲�
	����  ֵ����
	**************************************************************************/	
	float Accel_Angle_x,Accel_Angle_y,Accel_Angle_z;
	void Get_Angle(uint8_t way)
	{ 
		
		uint8_t IMUBuffer[14];
		//DMP�Ķ�ȡ�����ݲɼ��ж϶�ȡ���ϸ���ѭʱ��Ҫ��
		Temperature=MPU_Get_Temperature();      
		if(way==1)                          
		{	
			//��ȡ���ٶȡ����ٶȡ����(������ת)
			if(mpu_dmp_get_data(&Roll,&Pitch,&Yaw)==0)
			{ 
				MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
				MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
				Angle_Balance=Pitch;             	 				//����ƽ�����,ǰ��Ϊ��������Ϊ��
				Gyro_Balance=gyro[0];              			  //����ƽ����ٶ�,ǰ��Ϊ��������Ϊ��
				Gyro_Turn=gyro[2];                 				//����ת����ٶ�
				Acceleration_Z=accel[2];           				//����Z����ٶȼ�
			}
		}			
		else
		{
			if(!Read_Len(MPU_ADDR, 0x3B, 14, IMUBuffer))
			{
				  
					Accel_X = (int16_t)(((uint16_t)IMUBuffer[0] << 8)  | (uint16_t)IMUBuffer[1]);
					Accel_Y = (int16_t)(((uint16_t)IMUBuffer[2] << 8)  | (uint16_t)IMUBuffer[3]);
					Accel_Z = (int16_t)(((uint16_t)IMUBuffer[4] << 8)  | (uint16_t)IMUBuffer[5]);
					Gyro_X  = (int16_t)(((uint16_t)IMUBuffer[8] << 8)  | (uint16_t)IMUBuffer[9]);
					Gyro_Y  = (int16_t)(((uint16_t)IMUBuffer[10] << 8) | (uint16_t)IMUBuffer[11]);
					Gyro_Z  = (int16_t)(((uint16_t)IMUBuffer[12] << 8) | (uint16_t)IMUBuffer[13]);	
			}
			Accel_X-=ave_predata[0];
			Accel_Y-=ave_predata[1];
			Accel_Z-=ave_predata[2];
			//Gyro_X-=ave_predata[3];
			Gyro_Y-=ave_predata[4];
			//Gyro_Z-=ave_predata[5];
			//��������ת��  
			
			if(Gyro_X>32768)  Gyro_X-=65536;                
			if(Gyro_Y>32768)  Gyro_Y-=65536;                
			if(Gyro_Z>32768)  Gyro_Z-=65536;                
			if(Accel_X>32768) Accel_X-=65536;               
			if(Accel_Y>32768) Accel_Y-=65536;                
			if(Accel_Z>32768) Accel_Z-=65536;  
			Gyro_Balance=Gyro_X; 
			//������ǣ�ת����λΪ��
			Accel_Angle_x=atan2(Accel_Y,Accel_Z)*180/PI;     	
			Accel_Angle_y=atan2(Accel_X,Accel_Z)*180/PI;
			Accel_Angle_z=atan2(Accel_Y,Accel_X)*180/PI;
			//����������ת��
			G_X=Gyro_X/16.4;                              
			G_Y=Gyro_Y/16.4;  
			G_Z=Gyro_Z/16.4;
      //�������˲�
			
			if(way ==2)
			{
			Pitch = kalman_filter_update(&kf_roll, G_X, Accel_Angle_x,	0.005);
      Roll = kalman_filter_update(&kf_pitch, G_Y, Accel_Angle_y,	0.005);			
			}
	}
	}
