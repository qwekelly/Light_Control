#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "misc.h"
#include "adc.h"
#include "onenet.h"
#include "esp8266.h"
#include "edpkit.h"
#include "stm32f10x_adc.h"
	
void Key_Control_Pwm () {
	u16 led0pwmval=0;
	int KEY_VALUE = 0;
	while(1) {
		KEY_VALUE = KEY_Scan(0);
		if (KEY_VALUE == 1) {
				LED1 = 0; // 灯亮
				led0pwmval += 1000;
			} else if (KEY_VALUE == 2) {
				LED1 = 1; // 灯暗
				led0pwmval -= 1000;
			} else if(KEY_VALUE == 3) { // 切换回光敏控制
				break;
			}
			TIM_SetCompare2(TIM3,led0pwmval);	
	}
}

void Send_Data_Onenet () {
	int Light_Value;
	int timeCount = 0;	//发送间隔变量
	int KEY_VALUE = 0;
	
	while(OneNet_DevLink())	//接入OneNET  
	delay_ms(500);

	while(1) {
		KEY_VALUE = KEY_Scan(0);
		Light_Value = Get_Adc_Average(ADC_Channel_1,10);  // ADC 光照强度值

		if(++timeCount >= 20)									//发送间隔2s
		{
			UsartPrintf(USART1,"向onenet平台发送数据:\r\n");
			OneNet_SendData(Light_Value);									//发送数据

			timeCount = 0;
			ESP8266_Clear();
		}
		//delay_ms(10);
		
		if(KEY_VALUE == 4) { // 关闭数据传输
			LED1 = 1; // LED灯熄灭
			break;
		}
	}
}

void esp8266_rec(void)			/*主程序中调用来进行接收*/
{
	unsigned char *dataPtr = NULL;
	int num = 0;
	int KEY_VALUE = 0;
	u16 led0pwmval=0;
	
	while(1) {
		KEY_VALUE = KEY_Scan(0);
		dataPtr = ESP8266_GetIPD(0);
			
		if(dataPtr != NULL)
		{
			UsartPrintf(USART_DEBUG, "接收到的数据s：%s\r\n", dataPtr);
			//data_pro(dataPtr);
			num = atoi((const char *)dataPtr);				//转为数值形式
			UsartPrintf(USART_DEBUG, "接收数据转换成num型: %d\r\n", num);
			if(num == 1)								//控制数据如果为1，代表开
			{
				LED1 = 0;
				led0pwmval += 1000;
			}
			else if(num == 0)							//控制数据如果为0，代表关
			{
				LED1 = 1;
				led0pwmval -= 1000;
			}
		}
		TIM_SetCompare2(TIM3,led0pwmval);	
		
		if(KEY_VALUE == 5) { // 关闭接收数据
			LED1 = 1; // LED灯熄灭
			break;
		}
	}
}
	
 int main()
{	
 	u16 led0pwmval=0;	
	u16 adcx;
	int KEY_VALUE=0;
	
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	Usart1_Init(115200);							//串口1，打印信息用
  Usart2_Init(115200);							//串口2，驱动ESP8266用
 	LED_Init();			     //LED端口初始化
	KEY_Init(); //初始化按键
	Adc_Init();		  		//ADC初始化
  TIM3_PWM_Init(7200-1,0);	 //不分频。PWM频率=72000/900=8Khz
	
  while(1)
	{
		KEY_VALUE = KEY_Scan(0);
		adcx = Get_Adc_Average(ADC_Channel_1,10);  // ADC 值
		
		if (KEY_VALUE == 3) { // 按键控制PWM
			Key_Control_Pwm();
		} 
		
		if (KEY_VALUE == 4) { // 上传数据到onenet平台
			LED1 = 0; // LED灯亮
			ESP8266_Init();	// 初始化ESP8266发送模式
			Send_Data_Onenet(); // 发送数据到云端
		}
		
		if (KEY_VALUE == 5) { // 安卓APP控制ESP8266
			LED1 = 0; // LED灯亮
			ESP8266_StaTcpClient_UnvarnishTest(); // 初始化ESP8266为接收模式
			esp8266_rec(); // 接收APP的数据
		}
		
		// 光敏控制PWM
		if (adcx > 3000) {
			UsartPrintf(USART_DEBUG,"######### ADC 值 #####: %d\r\n", adcx);
			led0pwmval = 5000;
		} else if (adcx > 2000) {
			UsartPrintf(USART_DEBUG,"######### ADC 值 #####: %d\r\n", adcx);
			led0pwmval = 3000;
		} else {
			led0pwmval = 1000;
		}
		TIM_SetCompare2(TIM3,led0pwmval);	
	}
}
