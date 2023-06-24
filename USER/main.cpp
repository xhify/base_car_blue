#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "drv_Main.hpp"
#include "drv_LED.hpp"
#include "drv_LCD.hpp"
#include "drv_Key.hpp"
#include "Commulink.hpp"
#include "GUI.hpp"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "drv_Encoder.hpp"
#include "drv_PWMOut.hpp"
#include "drv_Uart1.hpp"
#include "drv_Uart3.hpp"
#include "drv_beep.h"
#include <stdbool.h>
#include "drv_hcsr04.h"
#include "control.h"
extern"C"{
	#include "adc.h"
#include "MPU6050.h"
#include "mpuiic.h"
}

RCC_ClocksTypeDef RCC_CLK;
//���������������е�ģʽ��Ҳ����Ŀ
int mode=0;

int is_leader=1;//�Ƿ�Ϊ��ͷС��
//����ͨ�Ŵ��ݵĲ���
extern int openmv_data_array[5];
extern int data_array[9] ;
extern char rec[80];
float VDDA;//��ص�ѹ
extern int L_code;
extern int R_code;
extern float L_speed, R_speed;
int L_count=0;
int R_count=0;;
extern int L_PWM ,R_PWM;
char str[50];
extern struct PID position_PID,speed_PID;
extern int pwml,pwmr;
int L_angle,R_angle;
extern float lengths;
int enter_cross=0;
int cross_cnt=0;//��·�ڼ��
int circle=0;//ͳ��Ȧ���Ӳ�·�ڳ�����Ϊ���һȦ��
int flag_stop=0;
int flag_wait=0;
int wait_time=0;
extern int blue_flag_stop , blue_flag_wait;//�������ݵ�ͣ����־
//����ԭ��
float targer_speed = 0.1;
void TIM2_Int_Init(u16 arr,u16 psc);
int Balance(float Angle,float Gyro);
int Velocity(int encoder_left,int encoder_right);
int turn(int encoder_left, int encoder_right, float gyro);
int turn_90_degrees(float angle_z);

int main(void)
{	
	//��ʼ�����ֳ���
	{
	//ϵͳʱ�ӳ�ʼ��
	SystemInit();
	Stm32_Clock_Init(9);
	//��ʱʱ�ӳ�ʼ��
	delay_init();	
	//��SWD�ӿ�,�رո��ù���
	JTAG_Set(SWD_ENABLE);        
	//��ȡʱ������
	RCC_GetClocksFreq(&RCC_CLK);	
	//����ϵͳ�ж����ȼ�����4	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
		//LED��ʼ������
	init_drv_LED();

	BEEP_Init();
	
	//�ر���mpu6050�ĳ�ʼ��
	//MPU_Init();	
	//while(mpu_dmp_init());
		
	//Play();

	//LED��˸
	init_Commulink();
	//��ʼ����Ļ
	init_drv_LCD();
	//��ʼ��GUI����
	init_GUI();
	init_drv_Key();
	Adc_Init();
	init_drv_Uart1(9600);
	drv_Encoder_Init_TIM3();
	drv_Encoder_Init_TIM4();
	init_drv_PWMOut();
	init_drv_Motor();
	TIM2_Int_Init(49,7199);//72mhz     0.005s
	init_drv_LED();
	//��ʼ����Ļ
	init_drv_LCD();
	//��ʼ��GUI����
	
	//��ʼ������
	init_drv_Key();
	//��ʼ����ʱ��
	//��ʼ�����������
	hcsr04Init();
	//��ʼ������2
	init_drv_Uart3(9600);
	
}
	//��ʼ�����


	//������ѭ��

	
	float length;
	
	speed_PID.kp = 100;
	speed_PID.ki =0;


//while 1ִ��Ƶ��Ϊ0.1s
	while(1)
	{	
				position_PID.kp =data_array[0]/10.0;
		position_PID.ki =data_array[1]/10000.0;
		position_PID.kd =data_array[2]/100.0;
		targer_speed  =data_array[3]/100.0;
		speed_PID.kp =data_array[6]/100.0;
		speed_PID.ki =data_array[7]/100.0;
		speed_PID.kd =data_array[8]/100.0;
		
		
		
		VDDA =Get_Adc_Average(12,5);
		VDDA = VDDA *3.3*11*1.1/4096;
		sprintf( str, "%d", mode);
			LCD_ShowString(40,0,str,BLUE,WHITE,16,0);
			//pitch
			sprintf( str, "%d", is_leader);
			LCD_ShowString(80,16,str,BLUE,WHITE,16,0);
			//yaw
			sprintf( str, "%3.1f",targer_speed);
			LCD_ShowString(48,32,str,BLUE,WHITE,16,0);
		
		/*��ȡ��̬*/
			
		sprintf( str, "%4d",L_code);	
			LCD_ShowString(48,48,str,BLUE,WHITE,16,0);
		
		sprintf( str, "%4d",R_code);	
			LCD_ShowString(48,64,str,BLUE,WHITE,16,0);
		
		
		
		sprintf( str, "%4.4f",L_speed);	
		LCD_ShowString(120,48,str,BLUE,WHITE,16,0);
		
		sprintf( str, "%4.4f",R_speed);	
		LCD_ShowString(120,64,str,BLUE,WHITE,16,0);
		
		sprintf( str, "%3d",L_angle);	
		LCD_ShowString(64,80,str,BLUE,WHITE,16,0);
		
		
		sprintf( str, "%3d",R_angle);	
		LCD_ShowString(64,96,str,BLUE,WHITE,16,0);
			
		
		
		sprintf( str, "%6d",L_PWM);	
		LCD_ShowString(48,112,str,BLUE,WHITE,16,0);

		sprintf( str, "%6d",R_PWM);	
		LCD_ShowString(48,128,str,BLUE,WHITE,16,0);	
		
		//ѭ������pid����
		sprintf( str, "%4.2f",position_PID.kp);	
		LCD_ShowString(0,144,str,BLUE,WHITE,16,0);	
		
		sprintf( str, "%4.2f",position_PID.ki);	
		LCD_ShowString(80,144,str,BLUE,WHITE,16,0);	
		
		sprintf( str, "%4.2f",position_PID.kd);	
		LCD_ShowString(160,144,str,BLUE,WHITE,16,0);
		//�ٶȿ���pid����
		sprintf( str, "%4.2f",speed_PID.kp);	
		LCD_ShowString(0,160,str,BLUE,WHITE,16,0);	
		
		sprintf( str, "%4.2f",speed_PID.ki);	
		LCD_ShowString(80,160,str,BLUE,WHITE,16,0);	
		
		sprintf( str, "%4.2f",speed_PID.kd);	
		LCD_ShowString(160,160,str,BLUE,WHITE,16,0);	
		
		sprintf( str, "%2.2f",VDDA);	
		LCD_ShowString(160,0,str,BLUE,WHITE,16,0);	
	
		
		LCD_ShowString(0,208,rec,BLUE,WHITE,16,0);
		
	
		length=UltraSonic_valuetance();
		sprintf( str, "%4.2f",lengths );
		LCD_ShowString(0,224,str,BLUE,WHITE,16,0);
		//printf("hello world");
		
	}
}




extern "C" void TIM2_IRQHandler(void) 
{
	//Ƶ��Ϊ0.005s,200hz
	//��� TIM2���� �жϷ������
	int index;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 
	{
		//��� TIM2���� �ж� ��־
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update ); 			
		
		
		L_angle=openmv_data_array[0];
		R_angle=openmv_data_array[1];
		if(is_leader ==1)
		{
			flag_stop = openmv_data_array[3];//3��ʾֹͣ��λ��Զ��2��ʾֹͣ��λ���У�1��ʾֹͣ��λ�ڽ���0���ʾ��ֹͣ�ߡ�
			flag_wait = openmv_data_array[4];//1��ʾ��⵽�ȴ�ֹͣ��
		}
		else
		{
			if(openmv_data_array[3] >0 && blue_flag_stop > 0)
			{
				if(openmv_data_array[3] < blue_flag_stop)
					flag_stop =openmv_data_array[3];
				else
					flag_stop =blue_flag_stop;
			}
			else
				flag_stop = openmv_data_array[3] || blue_flag_stop;
			
			flag_wait = openmv_data_array[4] || blue_flag_wait ;//1��ʾ��⵽�ȴ�ֹͣ��
		}
		int L_bias=L_angle-90;
		int R_bias=R_angle-90;
		
		
		
		//�ܿ��Ʋ���
		
		//��·�ڼ�ⲿ��
		if(L_angle != R_angle)
				cross_cnt++;
			//�ۻ�1.5s��ʱ����⵽������·����˵�������˲�·��
			if(cross_cnt > 300 && enter_cross ==0 && L_angle == R_angle)
			{
				enter_cross =1;
				cross_cnt =0;
			}	
			
			//�ۻ�1.5s��ʱ����⵽������·����˵���뿪�˲�·��
			if(cross_cnt > 300 && enter_cross ==1 && L_angle == R_angle)
			{
				enter_cross =0;
				cross_cnt =0;
				circle +=1;
			}
		
		//��ͷС�� ģʽ1 
		//����Ȧ��һȦͣ��
		if(is_leader == 1 && mode == 0)
		{
			targer_speed = 0.3;
					
			if(flag_stop == 3 && circle ==1)
				TraceMove(R_bias,0.2); 
			else if( flag_stop == 2 && circle ==1)
				TraceMove(R_bias,0.1); 
			else if( flag_stop == 1 && circle ==1)
				TraceMove(0,0);//ͣ��
			else if(enter_cross == 1)
				TraceMove(L_bias,targer_speed);
			else 
				TraceMove(R_bias,targer_speed);
				
		}
		//��ͷС�� ģʽ2
		//����Ȧ����Ȧ�� A��ֹͣ����ֹͣ��ֹͣ
		if(is_leader == 1 && mode == 1)
		{
			targer_speed = 0.5;
					
			if(flag_stop == 3 && circle ==2)
				TraceMove(R_bias,0.2); 
			else if( flag_stop == 2 && circle ==2)
				TraceMove(R_bias,0.1); 
			else if( flag_stop == 1 && circle ==2)
				TraceMove(0,0);//ͣ��
			else if(enter_cross == 1)
				TraceMove(L_bias,targer_speed);
			else 
				TraceMove(R_bias,targer_speed);
				
		}
		
		//��ͷС�� ģʽ3
		//��Ȧ-��Ȧ-��Ȧ���ٶȴ���0.3���ɣ�A��ֹͣ����ֹͣ��ֹͣ
		
		
		if(is_leader == 1 && mode == 2)
		{
			targer_speed = 0.5;
					
			if(flag_stop == 3 && circle ==3)
				TraceMove(R_bias,0.2); 
			else if( flag_stop == 2 && circle ==3)
				TraceMove(R_bias,0.1); 
			else if( flag_stop == 1 && circle ==3)
				TraceMove(0,0);//ͣ��
			else if( circle ==2)
				TraceMove(L_bias,targer_speed);
			else 
				TraceMove(R_bias,targer_speed);
				
		}
		
		//��ͷС�� ģʽ4
		//����Ȧ��һȦ ��E�������,�ٶ�Ϊ1
		if(is_leader == 1 && mode == 0)
		{
			targer_speed = 1;
					
			if(flag_stop == 3 && circle ==1)
				TraceMove(R_bias,0.2); 
			else if( flag_stop == 2 && circle ==1)
				TraceMove(R_bias,0.1); 
			else if( flag_stop == 1 && circle ==1)
				TraceMove(0,0);//ͣ��
			
			else if ( flag_wait ==1 && wait_time <=1000)
			{
				TraceMove(0,0);//ͣ��
				wait_time ++;
			}		
			else if(enter_cross == 1)
				TraceMove(L_bias,targer_speed);
			else 
				TraceMove(R_bias,targer_speed);				
		}
		
		//׷��С�� ģʽ1
		
		if(is_leader == 0 && mode == 0)
		{
			targer_speed = 0.3;
					
			if(flag_stop == 3 && circle ==1)
				TraceMove(R_bias,0.2); 
			else if( flag_stop == 2 && circle ==1)
				TraceMove(R_bias,0.1); 
			else if( flag_stop == 1 && circle ==1)
				TraceMove(0,0);//ͣ��
			else if(enter_cross == 1)
				TraceMove(L_bias,targer_speed);
			else 
				TraceMove(R_bias,targer_speed);
				
		}
		
		
		
	}
}

void TIM2_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//ʱ�� TIM2 ʹ��
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	//�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Period = arr; 
	//����ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	//����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//TIM���ϼ���	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//��ʼ�� TIM2
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	//��������ж�
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); 
	//�ж� ���ȼ� NVIC ����
	//TIM2 �ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
	//��ռ���ȼ� 0 ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	//�����ȼ� 3 ��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
	//IRQ ͨ����ʹ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	//��ʼ�� NVIC �Ĵ���
	NVIC_Init(&NVIC_InitStructure); 
	//ʹ�� TIM 2
	TIM_Cmd(TIM2, ENABLE); 
}


