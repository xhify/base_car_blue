#include "control.h"
#include "drv_Encoder.hpp"


struct PID
{
	float kp;
	float ki;
	float kd;
};

//Ĭ������ת�� turn_speedΪ����������ת��turn_speed Ϊ����

int get_speed(){




	return 0;
}
//λ��ʽPID������
//��������ת��
int PositionPID(float deviation)
{
	PID pid;
	pid.kp=100;
	pid.ki=0;
	pid.kp=-30;
	float Position_KP=pid.kp,Position_KI=pid.ki,Position_KD=pid.kd;
	static float Bias,Pwm,Integral_bias,Last_Bias;
	Bias=deviation;                         		         //����ƫ��
	Integral_bias+=Bias;	                                 //���ƫ��Ļ���
	Pwm=Position_KP*Bias+Position_KI*Integral_bias+Position_KD*(Bias-Last_Bias); //λ��ʽPID������
	Last_Bias=Bias;                                      	 //������һ��ƫ�� 
	return Pwm;    
}
int PositionPIDToSpd(float deviation)
{
	PID pid;
	pid.kp=-50;pid.ki=-6;pid.kd=0;
	int MAX_MOTOR_PWM=7000;
	float Position_KP=pid.kp,Position_KI=pid.ki,Position_KD=pid.kd;
	static float Bias,Pwm,Integral_bias,Last_Bias,pwmKI=0;
	Bias=deviation;                         		         //����ƫ��
	Integral_bias+=Bias;	                                 //���ƫ��Ļ���
	pwmKI=Position_KI*Integral_bias;
	if(pwmKI>MAX_MOTOR_PWM) Integral_bias=MAX_MOTOR_PWM/Position_KI;
	Pwm=Position_KP*Bias+pwmKI+Position_KD*(Bias-Last_Bias);       //λ��ʽPID������
	Last_Bias=Bias;                                      	 //������һ��ƫ�� 
	return Pwm;    
}

int ChangeTraceTurn(int TraceDate)
{
	int pwm=0;
	int bias;
	bias=TraceDate;
	pwm=PositionPID(bias);
	return pwm;
}

/*@brief:����pid������ߵ����Ŀ���ٶ�
 * @param:
 *        [in]int EncodeSpdL: ��ǰ��������������ֵ
 *        [in]float TarSpdL:��ߵ��Ŀ���ٶ�,����ٶ�Խ1.19m/s
 * @return: ������ߵ��������pwmռ�ձ�
 */
int ChangeSpeedMotorL(int NowEncodeSpdL,float TarSpdL)
{
	int pwm=0;
	int bias;
	int TarEncodeSpdL;
	//TarEncodeSpdL=(int)((TarSpdL*ACircleEncoder)/(WheelOneCircleDis*100)+0.5f);//����Ŀ���ٶ����Ŀ��������ٶ�
	bias=NowEncodeSpdL-TarEncodeSpdL;
	pwm=PositionPIDToSpd(bias);
	return pwm;
}

/*@brief:����pid�����ұߵ����Ŀ���ٶ�
 * @param:
 *        [in]int EncodeSpdL: ��ǰ�ҵ������������ֵ
 *        [in]float TarSpdL:�ұߵ��Ŀ���ٶ�,����ٶ�Խ1.27m/s
 * @return: �����ұߵ��������pwmռ�ձ�
 */
int ChangeSpeedMotorR(int NowEncodeSpdR,float TarSpdR)
{
	int pwm=0;
	int bias;
	int TarEncodeSpdR;
	//TarEncodeSpdR=(int)((TarSpdR*ACircleEncoder)/(WheelOneCircleDis*100)+0.5f);//����Ŀ���ٶ����Ŀ��������ٶ�
	bias=NowEncodeSpdR-TarEncodeSpdR;
	return pwm;
}

/*@brief: ��С������ѭ��������
 *@param:
 *        [in]TraceDate: ѭ�������������ֵ
 *        [in]TarSpeed:ѭ����Ŀ���ٶ�
 *@return: ����Ŀ��㷵��1�����򷵻�0
 */
void TraceMove(int TraceDate,float TarSpeed)
{
	int turnpwm=0;
	int spdpwml=0,spdpwmr=0;
	int pwml=0,pwmr=0;
	
	turnpwm=ChangeTraceTurn(TraceDate);
	
	int Encode_Left=Read_Encoder(4);
	int Encode_Right= Read_Encoder(3);
	
	spdpwml=ChangeSpeedMotorL(Encode_Left,TarSpeed);
	spdpwmr=ChangeSpeedMotorR(Encode_Right,TarSpeed);
	
	
	pwmr=turnpwm+spdpwmr;
	
	pwml=-turnpwm+spdpwml;
	
	
	R_PWMset(pwmr);
	L_PWMset(pwml);
	

}



