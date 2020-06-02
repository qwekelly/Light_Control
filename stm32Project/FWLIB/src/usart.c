#include "usart.h"


__IO uint8_t usart1_rcvd_flag;
__IO uint8_t usart2_rcvd_flag;
__IO uint8_t usart3_rcvd_flag;


__IO uint8_t usart1_rcvd_len = 0;
__IO uint8_t usart2_rcvd_len = 0;
__IO uint8_t usart3_rcvd_len = 0;


uint8_t usart1_rcvd_buf[USART1_RX_BUF_LEN];
char usart2_rcvd_buf[USART2_RX_BUF_LEN];
char usart3_rcvd_buf[USART3_RX_BUF_LEN];


void Init_USART(void)
{
	Init_USART1();//PC
//	Init_USART2();
	Init_USART3();//ESP8266
}


void my_NVIC_init(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void Init_USART1(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	Stm32_Uartx_Init(USART1,USART1_BAUDRATE,FORMAT_8none);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void Init_USART2(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	/*
	*  USART2_TX -> PA2 , USART2_RX -> PA3
	*/				
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	Stm32_Uartx_Init(USART2,USART2_BAUDRATE,FORMAT_8none);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}
void Init_USART3(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
	/*
	*  USART3_TX -> PB10 , USART3_RX -> PB11
	*/				
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	Stm32_Uartx_Init(USART3,USART3_BAUDRATE,FORMAT_8none);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

}


void Stm32_Uartx_Init(USART_TypeDef* USARTx,uint32_t Baudrate,uint16_t Format)
{
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = Baudrate;
	switch(Format)
	{
		default:
		case FORMAT_8none:
		{ 
			USART_InitStruct.USART_WordLength = USART_WordLength_8b;
			USART_InitStruct.USART_Parity = USART_Parity_No;
			break;
		}
		case FORMAT_8even:
		{ 
			USART_InitStruct.USART_WordLength = USART_WordLength_9b;
			USART_InitStruct.USART_Parity = USART_Parity_Even;
			break;
		}
		case FORMAT_8odd:
		{ 
			USART_InitStruct.USART_WordLength = USART_WordLength_9b;
			USART_InitStruct.USART_Parity = USART_Parity_Odd;
			break;
		}
	}

	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_Init(USARTx, &USART_InitStruct);
	USART_Cmd(USARTx, ENABLE);
	USART_ClearFlag(USARTx, USART_FLAG_TC);
}


void usart_send_str(USART_TypeDef* USARTx,char *str)
{
	USART_GetFlagStatus(USARTx, USART_FLAG_TC);
	while((*str)!='\0')
	{
	  USART_SendData(USARTx,*str);
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
		str++;
	}
}

//printf("%s\r\n","hello");
int fputc(int ch, FILE *f)
{
	USART_SendData(USART2, (uint8_t) ch);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);		
	return (ch);

}	

int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
	return (char)USART_ReceiveData(USART2);
}

//整形转字符，stm32不支持标准的itoa,这里自己实现
char *myitoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

//字符串转整形，stm32不支持标准的atoi,这里自己实现
int myatoi(const char *str)
{
	int s=0;
	uint8_t falg=0;
	
	while(*str==' ')
	{
		str++;
	}

	if(*str=='-'||*str=='+')
	{
		if(*str=='-')
		falg=1;
		str++;
	}

	while(*str>='0'&&*str<='9')
	{
		s=s*10+*str-'0';
		str++;
		if(s<0)
		{
			s=2147483647;
			break;
		}
	}
	return s*(falg?-1:1);
}
//printf();
//简单的printf实现，在例程中将printf重定向到了串口2上，使用该函数可以实现其他串口的printf功能，注意是部分实现。
void USARTx_printf(USART_TypeDef* USARTx,char *Data,...)
{
  const char *str;
  int d;   
  char buf[16];
  va_list ap;
  va_start(ap, Data);
  while ( *Data != 0)
  {	
			USART_GetFlagStatus(USARTx, USART_FLAG_TC);
      if ( *Data == 0x5c )  //'\'
      {									  
              switch ( *++Data )
              {
                      case 'r':
                              USART_SendData(USARTx,0x0d);
                              Data ++;
                              break;

                      case 'n':
                              USART_SendData(USARTx,0x0a);	
                              Data ++;
                              break;
                      
                      default:
                              Data ++;
                          break;
              }			 
      }
      else if ( *Data == '%')
      {
				switch ( *++Data )
				{				
				case 's':	
					str = va_arg(ap, const char *);
					for ( ; *str; str++) 
					{
						USART_SendData(USARTx,*str);
						while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
					}
					Data++;
					break;
				case 'd':	
					d = va_arg(ap, int);
					myitoa(d, buf, 10);
					for (str = buf; *str; str++) 
					{
						USART_SendData(USARTx,*str);
						while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
					}
					Data++;
					break;
				default:
					Data++;
					break;
			}		 
    } 
    else 
		{
			USART_SendData(USARTx,*Data++);
			while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
		}
  }
}


void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE))
	{
		uint16_t data =  USART_ReceiveData(USART1);
		USART_SendData(USART3,data);//echo test
	  //command = data;
		//USARTx_printf(USART1,"ENTER 1\r\n");
		usart1_rcvd_buf[usart1_rcvd_len++%USART1_RX_BUF_LEN] = data;
		usart1_rcvd_flag = 1 ;
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}


void USART2_IRQHandler(void)
{
  if(USART_GetITStatus(USART2, USART_IT_RXNE))
	{
		usart2_rcvd_buf[usart2_rcvd_len++%USART2_RX_BUF_LEN] = USART_ReceiveData(USART2);
	//	USART_SendData(USART3,USART_ReceiveData(USART2));
		usart2_rcvd_flag = 1 ;
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
}


void USART3_IRQHandler(void)
{
  if(USART_GetITStatus(USART3, USART_IT_RXNE))
	{
		uint16_t data = USART_ReceiveData(USART3);
		usart3_rcvd_buf[usart3_rcvd_len++%USART3_RX_BUF_LEN] = data;
		USART_SendData(USART1,data);
		usart3_rcvd_flag = 1 ;
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	}
}
void USARTx_buffClear(USART_TypeDef* USARTx)
{
		 if(USARTx == USART1)
		 {
						usart1_rcvd_len = 0;
						memset(usart1_rcvd_buf,0,sizeof(usart1_rcvd_buf));
						usart1_rcvd_flag = 0;
		}
		else if(USARTx == USART2)
		{
						usart2_rcvd_len = 0;
						memset(usart2_rcvd_buf,0,sizeof(usart2_rcvd_buf));
						usart2_rcvd_flag = 0;
		}
		else if(USARTx == USART3)
		{
						usart3_rcvd_len = 0;
						memset(usart3_rcvd_buf,0,sizeof(usart3_rcvd_buf));
						usart3_rcvd_flag = 0;
		}
}

int haveDatareceived(void)
{
	return usart1_rcvd_flag;
}

int getData(void)
{
	usart1_rcvd_flag = 0;
	usart1_rcvd_len--;
	return usart1_rcvd_buf[0];
}
