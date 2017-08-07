#include "reg52.h"			
#include "temp.h"
#include "lcd.h"
#include "stdio.h"

typedef unsigned int u16;	
typedef unsigned char u8;

sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;
sbit LED=P2^0;
sbit IRIN=P3^2;

u8 IrValue[6];
u8 Time;
char flag=1;
char timer_flag=0;

void delay(u16 i)
{
	while(i--);	
}

void IrInit()
{
	IT0=1;
	EX0=1;
	EA=1;
	IRIN=1;
}
void Timer0Init()
{
	TMOD|=0X01;
	TH0=60;
	TL0=176;
	ET0=1;
	EA=1;
	TR0=1;		
}


void main()
{	
	int temp;
	float temp_f;
	bit flag = 1;
	u8 temp_str[10];
	LcdInit();
	IrInit();
	Timer0Init();
	LED=1;
	while(1)
	{	if(IrValue[2]==0x45)
		{
			flag=~flag;
			IrValue[2]=0x00;
			clear_screen();
		}
		if(flag==1)
		{	
			if(timer_flag==1){
				timer_flag=0;
				display(0,1,"Temp:");
				temp = datapros(Ds18b20ReadTemp());
				temp_f = (float)temp / 100.00;
				sprintf(temp_str,"%5.2f",temp_f);
				display(5,1,temp_str);
			}
		}else{
			display(0,1,"paused");
		}
	}
}

void ReadIr() interrupt 0
{
	u8 j,k;
	u16 err;
	Time=0;					 
	delay(700);	//7ms
	if(IRIN==0)	
	{	 
		err=1000;				
		while((IRIN==0)&&(err>0))	
		{			
			delay(1);
			err--;
		} 
		if(IRIN==1)	
		{
			err=500;
			while((IRIN==1)&&(err>0))
			{
				delay(1);
				err--;
			}
			for(k=0;k<4;k++)
			{				
				for(j=0;j<8;j++)
				{

					err=60;		
					while((IRIN==0)&&(err>0))
					{
						delay(1);
						err--;
					}
					err=500;
					while((IRIN==1)&&(err>0))	
					{
						delay(10);
						Time++;
						err--;
						if(Time>30)
						{
							return;
						}
					}
					IrValue[k]>>=1;	
					if(Time>=8)		
					{
						IrValue[k]|=0x80;
					}
					Time=0;
				}
			}
		}
	}
}
void Timer0() interrupt 1
{
	static u16 i;
	TH0=60;
	TL0=176;
	i++;
	if(i==40)
	{
		i=0;
		timer_flag=1;
		LED=~LED;
	}
}
