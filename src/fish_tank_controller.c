#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include "i2c_master.h"
#include "i2c_lcd.h"
#include "i2c_rtc.h"

#define F_CPU 12000000UL

uint16_t avolt=0;
double temp=0.0;
int sec,min,hour,date,month,year;
int mtr=0,htr=0,fdr=0,lgt=0;

void disp_boot()
{
	//char *kzwel="krizX System Booting";
	i2c_lcd_init_4bit(); _delay_ms(10);  //LCD Initialization for 4bit mode
	i2c_lcd_disp_init(); _delay_ms(10);  //LCD Display Initialization
	
	i2c_lcd_disp_str("krizX boot init"); 
	i2c_lcd_exec_cmd(0xC0);
	for(int i=0;i<5;i++)
	{
		i2c_lcd_disp_char('.');
		_delay_ms(400);  	 //Welcome message
	}
	i2c_lcd_disp_str('\0');	
}

void disp_wtemp()
{
	//char *dmsg1=("W.T=");
	uint16_t rth=0;
	int sample=0;
	while(sample<5)
	{
		ADCSRA |= (1 << ADSC);
		_delay_ms(150);
		++sample;
	}
	
	avolt=avolt/5;						//
	avolt=(avolt*100)/205;				//To calculate the thermistor resistance
	rth=((5000000/avolt)-10000);		//
	
	temp=rth/10000.0;					//
	temp=log(temp);						//
	temp=temp/3950.0;					//Using the beta temperature equation
	temp=temp+0.00335;					//tofind the temperature from resistance 
	temp=1.0/temp;						//beta value "B"=3950 at Ro=10K@25c
	temp=temp-273.15;					//

	i2c_lcd_disp_init();
	i2c_lcd_disp_str("WT=");
	i2c_lcd_disp_num(temp*10,2);
	//i2c_lcd_disp_char(0xDF);
	//i2c_lcd_disp_char('C');
	
}

ISR(ADC_vect)
{
	avolt+=ADC;
	ADCSRA |= (1<<ADIF);
}	

void disp_atemp()
{
	int at, atd;
	at=i2c_rtc_get_temp();
	atd=i2c_rtc_get_tempd();
	at=(at*100)+atd;
	i2c_lcd_disp_str(" AT=");
	i2c_lcd_disp_num(at,2);
	i2c_lcd_disp_num(atd,0);
}

void get_time()
{
	hour=i2c_rtc_get_hour();
	min=i2c_rtc_get_min();
	sec=i2c_rtc_get_sec();
	date=i2c_rtc_get_date();
	month=i2c_rtc_get_month();
	year=i2c_rtc_get_year();
}

void disp_time()
{
	get_time();
	i2c_lcd_exec_cmd(0xC0);
	i2c_lcd_disp_time(hour,'\0');
	i2c_lcd_disp_time(min,':');
	i2c_lcd_disp_str(" _ ");
	i2c_lcd_disp_time(date,'\0');
	i2c_lcd_disp_time(month,'/');
	i2c_lcd_disp_time(year,'/');
}

void disp_status()
{
	i2c_lcd_disp_init();
	if(mtr==0)
	i2c_lcd_disp_str("MTR:OFF ");
	else i2c_lcd_disp_str("MTR:ON ");
	if(htr==0)
	i2c_lcd_disp_str("HTR:OFF");
	else i2c_lcd_disp_str("HTR:ON");
	i2c_lcd_exec_cmd(0xC0);
	if(fdr==0)
	i2c_lcd_disp_str("FOOD:OFF ");
	else i2c_lcd_disp_str("FODOD:ON ");
	if(lgt==0)
	i2c_lcd_disp_str("LGT:OFF");
	else i2c_lcd_disp_str("LGT:ON");

}

void set_mtr_status()
{	
	get_time();
	if(min<=14)
		{	
			mtr=1;	
			PORTD=PORTD|0x20;	
		}
	else if(min>=15)
		{
			mtr=0;
			PORTD=PORTD&0xDF;
		}
}

void set_fdr_status()
{
	get_time();
	int i=0;
	if(((min>=30) & (min<=58)) & ((hour==6) | (hour==17)))
	{
		fdr=1;
		PORTD=PORTD|0x40;			//FEEDER ON
		
		i=0;
		while(i<300)			
		{	_delay_ms(10); ++i;}	//Wait for 3 seconds
		
		PORTD=PORTD&0xBF;			//FEEDER OFF
		fdr=0;
	}
	else 
	{
		PORTD=PORTD&0xBF;
		fdr=0;
	}

	if(((min>=30) & (hour>=17)) | (hour>=18))
	{
		lgt=1;
		PORTD=PORTD|0x80;
	}
	if(((min>=30) & (hour>=22)) | (hour>=23))
	{
		lgt=0;
		PORTD=PORTD&0x7F;
	}
}

ISR(INT0_vect)
{
	i2c_lcd_disp_init();
	i2c_lcd_disp_str("INTRPT DETECT");
	i2c_lcd_exec_cmd(0xC0);

	int rtc_stat=i2c_rtc_get_status(),i=0;
	if (rtc_stat&0x02)
	{	
		i2c_rtc_alarm_clr();
		i2c_rtc_alarm2_set();
		i2c_lcd_disp_str("OXYGEN ALARM");
		while(i<20)			
		{	_delay_ms(100); ++i;}
		set_mtr_status();
	}

	else if (rtc_stat&0x01)
	{
		i2c_rtc_alarm_clr();
		i2c_rtc_alarm1_set();
		i2c_lcd_disp_str("FOOD ALARM");
		while(i<20)			
		{	_delay_ms(100); ++i;}
		set_fdr_status();
	}
}

int main()
{
	_delay_ms(100);
	SREG |= 0x80;

	i2c_init();				//I2C Initialization
	_delay_ms(10);   
	
	DDRD=DDRD | 0xE0;
	PORTD=PORTD & 0x1F;
	//i2c_rtc_set_time(0,50,15,3,4,10,16);
	disp_boot();   
	_delay_ms(10);
	
	ADMUX &= 0x00;			//ADC Initialization_Vref enabled_Channel 0
	ADCSRA = (0x8F<<0); 	//_delay_ms(1);
	DIDR0 &= 0xFE;			//Disable Digital input on ADC0
	_delay_ms(10);
	
	get_time();
	i2c_rtc_alarm_init();
	set_mtr_status();
	i2c_rtc_alarm2_set();

	set_fdr_status();
	i2c_rtc_alarm1_set();
	
	EICRA=0x01;
	EIMSK=0x01;

	while(1)
	{
		int i=0;
		avolt=0;
		disp_wtemp();
		disp_atemp();
		disp_time();
		while(i<70)			//change value for screen refresh rate
		{	_delay_ms(100); ++i;}
		disp_status();
		i=0;
		while(i<70)			//change value for screen refresh rate
		{	_delay_ms(100); ++i;}
	
	}
		

}
