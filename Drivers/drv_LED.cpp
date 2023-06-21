#include "stm32f10x.h"
#include "drv_LED.hpp"
#include "Basic.hpp"
#include "delay.h"
#include "GUI.hpp"
#include "GUI_Images.hpp" 
#define APB1TIMERCLK 36000000
extern int picture_id;
float ExtLedR = 0;
float ExtLedG = 0;
float ExtLedB = 0;


//�������Բ�������
static inline float led_linear_compensation( float in )
{
	if( in <= 8 )
		return in / 903.3f * 100;
	else
		return 0;
}

/*
	LED�����Ⱥ���
	R��G��B�����Ȱٷֱȣ�0-100��
*/
void set_LedBrightness( float R , float G , float B )
{
	if( B >= 0 && B <= 100 )
	{
		B = led_linear_compensation(B);
		ExtLedB = B;
		TIM5->CCR1 = B/100*TIM5->ARR;
	}
	if( G >= 0 && G <= 100 )
	{
		G = led_linear_compensation(G);
		ExtLedG = G;
		TIM5->CCR2 = G/100*TIM5->ARR;
	}
	if( R >= 0 && R <= 100 )
	{
		R = led_linear_compensation(R);
		ExtLedR = R;
		TIM5->CCR3 = R/100*TIM5->ARR;
	}
}

static LED_Mode led_status = normal;

/*
	������Ƶ�ʵ��ں���
	freq:������Ƶ��
*/
void set_BuzzerFreq( unsigned short freq )
{
	float B = (float)TIM5->CCR1/(float)TIM5->ARR;
	float G = (float)TIM5->CCR2/(float)TIM5->ARR;
	float R = (float)TIM5->CCR3/(float)TIM5->ARR;
	if( freq < 200 )
		freq = 200;
	TIM5->ARR = 10e6 / freq;
	if( TIM5->CCR4 != 0 )
		TIM5->CCR4 = TIM5->ARR / 2;
	TIM5->CCR1 = B*TIM5->ARR;
	TIM5->CCR2 = G*TIM5->ARR;
	TIM5->CCR3 = R*TIM5->ARR;
}

/*
	���������캯��
	on:�Ƿ�����
*/
void set_BuzzerOnOff( bool on )
{
	if(on)
		TIM5->CCR4 = TIM5->ARR / 2;
	else
		TIM5->CCR4 = 0;
}


//����LEDģʽ
void Set_LED_Mode(LED_Mode mode)
{
	led_status = mode;
}

//LED��˸����
void Led_Flash()
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_0);
	delay_ms(500);
	GPIO_SetBits(GPIOE,GPIO_Pin_0);
	delay_ms(500);
}

//TIM2 �ж�


//LED��ʼ������
void init_drv_LED(void)
{
	/*
		D3_LED(GPIOE_P0)  PE0
		LED_B(TIM5_CH1)   PA0
		LED_G(TIM5_CH2)   PA1
		LED_R(TIM5_CH3)   PA2
		Buzzer(TIM5_CH4)  PA3
	*/
	//����GPIOE��GPIOA����ʱ��,�������ù���ʱ��
	RCC->APB2ENR|=(1<<6)|(1<<2)|(1<<0);
	//D3��ʼ��
	os_delay(1e-2);
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	//ʹ�ܶ�ʱ�� 5 ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);	   
	
	//�����������ģʽ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	    //ʹ��PE�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;			    	//D3-->PE.0 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 		//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOE, &GPIO_InitStructure);			     		//��ʼ��GPIOE.0
	GPIO_SetBits(GPIOE,GPIO_Pin_0);											//PE.0 �����
	
	//GPIO��ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	    //ʹ��PA�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;			    	
	GPIO_InitStructure.GPIO_Mode = GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;; 	 		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 		//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);			     		//��ʼ��GPIOA.0~3
	
	//��ʱ��5��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 1000; 										//�������Զ���װ������ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 71; 										//����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 								//����ʱ�ӷָ� :TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); 						//��ʼ�� TIMx
	
	//��ʼ�� TIM5 Channel PWM ģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��PWMģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // �Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //������Ը�
	TIM_OC1Init(TIM5, &TIM_OCInitStructure); //��ʼ������ TIM5 OC1
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable); // ʹ��Ԥװ�ؼĴ���
	TIM_OC2Init(TIM5, &TIM_OCInitStructure); //��ʼ������ TIM5 OC2
	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable); // ʹ��Ԥװ�ؼĴ���
	TIM_OC3Init(TIM5, &TIM_OCInitStructure); //��ʼ������ TIM5 OC3
	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable); // ʹ��Ԥװ�ؼĴ���
	TIM_OC4Init(TIM5, &TIM_OCInitStructure); //��ʼ������ TIM5 OC4
	TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable); // ʹ��Ԥװ�ؼĴ���
	TIM_Cmd(TIM5, ENABLE); //ʹ�� TIM5
	
}