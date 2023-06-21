#include "drv_hcsr04.h"
#include "sys.h"
extern char value[];    //�洢ת�����ֵ
extern int Length;
int Val=0;
int countnum=0;
void hcsr04Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB2PeriphClockCmd(HCSR04_CLK,ENABLE);
	RCC_APB1PeriphClockCmd(HCSR04_TIM_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=HCSR04_TRIG;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  		//�������
	GPIO_Init(HCSR04_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(HCSR04_PORT, HCSR04_TRIG);    
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;	//��������
	GPIO_InitStructure.GPIO_Pin=HCSR04_ECHO;
	GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  

	TIM_DeInit(TIM5);
	TIM_TimeBaseStructure.TIM_Period = 65535; 
	TIM_TimeBaseStructure.TIM_Prescaler =71; 
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //��ʼ�� 

	TIM_ClearFlag(TIM5, TIM_FLAG_Update);   //��������жϣ����һ���ж����������ж�
 	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);    //�򿪶�ʱ�������ж�
	TIM_Cmd(TIM5,DISABLE);
}


float UltraSonic_valuetance(void)   //��������������
{
float lengths=0,sum=0;
		int i=0;
	int time=0;
		while(i!=8){		
		GPIO_SetBits(HCSR04_PORT, HCSR04_TRIG);   //���ߵ�ƽ�ź�
		delay_us(20);   //�ߵ�ƽ����10uS
		GPIO_ResetBits(HCSR04_PORT, HCSR04_TRIG); //���͵�ƽ�ź�    
		//�ȴ������ź�
			delay_us(10);
	while(GPIO_ReadInputDataBit(HCSR04_PORT,HCSR04_ECHO)==0);  //���յ��ź�ECHOΪ�ߵ�ƽ
		TIM_Cmd(TIM5,ENABLE);        //ʹ��TIM5��ʱ��
			i++;
	while(GPIO_ReadInputDataBit(HCSR04_PORT,HCSR04_ECHO)==1);//ֱ�������ź���ʧ
		TIM_Cmd(TIM5,DISABLE);       //ʧ��TIM5��ʱ��
	time=int(TIM_GetCounter(TIM5)+countnum*65535);
	
	lengths=(time)*17/1000;
		if(lengths<=0)
		{
			lengths=0;
		}
		sum+=lengths;
		countnum=0;
		TIM_SetCounter(TIM5,0);  //ȡ��TIM5��counter�Ĵ������ֵ
	}
  lengths=sum/8.0;
	
		return lengths;
}
	
extern "C" void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
		countnum++;
	}
}
/*
//��ʱ�� 5 ͨ�� 1 ���벶������
TIM_ICInitTypeDef TIM5_ICInitStructure;
void TIM5_Cap_Init(u16 arr,u16 psc)
{
GPIO_InitTypeDef GPIO_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
NVIC_InitTypeDef NVIC_InitStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //��ʹ�� TIM5 ʱ��
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //��ʹ�� GPIOA ʱ��
	//��ʼ�� GPIOA.0 ��
	GPIO_InitStructure.GPIO_Pin=HCSR04_TRIG;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;  		//�������
	GPIO_Init(HCSR04_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(HCSR04_PORT, HCSR04_TRIG);
	
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PA0 ���� 
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0 ���� 
GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ�� GPIOA.0
GPIO_ResetBits(GPIOC,GPIO_Pin_7); //PA0 ����
//�ڳ�ʼ�� TIM5 ����
TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ
TIM_TimeBaseStructure.TIM_Prescaler =psc; //Ԥ��Ƶ�� 
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // TDTS = Tck_tim
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���ģʽ
TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //��ʼ�� TIMx
//�۳�ʼ�� TIM5 ���벶��ͨ�� 1
TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; //ѡ������� IC1 ӳ�䵽 TI1 ��
TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; //�����ز���
TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽 TI1 ��
TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; //���������Ƶ,����Ƶ
TIM5_ICInitStructure.TIM_ICFilter = 0x00; //IC1F=0000 ���������˲��� ���˲�
TIM_ICInit(TIM5, &TIM5_ICInitStructure); //��ʼ�� TIM5 ���벶��ͨ�� 1
//�ݳ�ʼ�� NVIC �ж����ȼ�����
NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn; //TIM3 �ж�
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //��ռ���ȼ� 2 ��
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //�����ȼ� 0 ��
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ͨ����ʹ��
NVIC_Init(&NVIC_InitStructure); //��ʼ�� NVIC
TIM_ITConfig( TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);//����������жϲ����ж�
TIM_Cmd(TIM5,ENABLE ); //��ʹ�ܶ�ʱ�� 5
}
u8 TIM5CH1_CAPTURE_STA=0; //���벶��״̬ 
u16 TIM5CH1_CAPTURE_VAL;//���벶��ֵ
//�ݶ�ʱ�� 5 �жϷ������
extern "C" void TIM5_IRQHandler(void)
{ 
if((TIM5CH1_CAPTURE_STA&0X80)==0)//��δ�ɹ�����
{ 
if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
{ 
if(TIM5CH1_CAPTURE_STA&0X40) //�Ѿ����񵽸ߵ�ƽ��
{if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
{
TIM5CH1_CAPTURE_STA|=0X80; //��ǳɹ�������һ��
TIM5CH1_CAPTURE_VAL=0XFFFF;
}else TIM5CH1_CAPTURE_STA++;
}
}
if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET) //���� 1 ���������¼�
{
if(TIM5CH1_CAPTURE_STA&0X40) //����һ���½���
{ 
TIM5CH1_CAPTURE_STA|=0X80; //��ǳɹ�����һ��������
TIM5CH1_CAPTURE_VAL=TIM_GetCapture1(TIM5);
 TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising); //����Ϊ�����ز���
}else //��δ��ʼ,��һ�β���������
{
TIM5CH1_CAPTURE_STA=0; //���
TIM5CH1_CAPTURE_VAL=0;
TIM_SetCounter(TIM5,0);
TIM5CH1_CAPTURE_STA|=0X40; //��ǲ�����������
 TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling); //����Ϊ�½��ز���
} 
} 
}
 TIM_ClearITPendingBit(TIM5, TIM_IT_CC1|TIM_IT_Update); //����жϱ�־λ
}
*/



