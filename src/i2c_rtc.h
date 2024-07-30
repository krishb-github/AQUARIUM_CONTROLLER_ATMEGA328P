int dec2bcd(int dec)
{
	int a,bcd=0;
	a=dec%10;
	dec=dec/10;
	bcd=((dec<<4)|0x0F)&(a|0xF0);
	return bcd;
}

int bcd2dec(int bcd)
{
	int a, dec=0;
	a = bcd & 0x0F;
	bcd=(bcd>>4)&0x0F;
	dec=(bcd*10)+a;
	return dec;
}
/*--------------------------------**********-------------------------------------*/
int i2c_rtc_get_sec()
{	
	i2c_start(0xD0);
	i2c_write(0x00);
	i2c_start(0xD1);
	return bcd2dec(i2c_read_nack());
}

int i2c_rtc_get_min()
{
	i2c_start(0xD0);
	i2c_write(0x01);
	i2c_start(0xD1);
	return bcd2dec(i2c_read_nack());
}

int i2c_rtc_get_hour()
{	
	i2c_start(0xD0);
	i2c_write(0x02);
	i2c_start(0xD1);
	return bcd2dec(i2c_read_nack() & 0x3F);
}

int i2c_rtc_get_day()
{	
	i2c_start(0xD0);
	i2c_write(0x03);
	i2c_start(0xD1);
	return bcd2dec(i2c_read_nack());
}

int i2c_rtc_get_date()
{	
	i2c_start(0xD0);
	i2c_write(0x04);
	i2c_start(0xD1);
	return bcd2dec(i2c_read_nack());
}

int i2c_rtc_get_month()
{	
	i2c_start(0xD0);
	i2c_write(0x05);
	i2c_start(0xD1);
	return bcd2dec(i2c_read_nack());
}

int i2c_rtc_get_year()
{	
	i2c_start(0xD0);
	i2c_write(0x06);
	i2c_start(0xD1);
	return bcd2dec(i2c_read_nack());
}

int i2c_rtc_get_temp()
{
	i2c_start(0xD0);
	i2c_write(0x11);
	i2c_start(0xD1);
	return i2c_read_nack();
}

int i2c_rtc_get_tempd()
{
	i2c_start(0xD0);
	i2c_write(0x12);
	i2c_start(0xD1);
	int tmp=i2c_read_nack();
	if(tmp>>6==0x00) return 0;
	else if(tmp>>6==0x01) return 25;
	else if(tmp>>6==0x02) return 50;
	else return 75;
} 	

int i2c_rtc_get_status()
{
	i2c_start(0xD0);
	i2c_write(0x0F);
	i2c_start(0xD1);
	return i2c_read_nack();
}
	
/*-----------------------------------*********-----------------------------------*/
void i2c_rtc_alarm_init()
{
	i2c_start(0xD0);		//Initialise Control registers
	i2c_write(0x0E);
	i2c_write(0x1F);
	i2c_stop();
	_delay_ms(20);

	i2c_start(0xD0);		//Disable alarm2 date/day
	i2c_write(0x0D);
	i2c_write(0xFF);
	i2c_stop();
	_delay_ms(20);

	i2c_start(0xD0);		//Disable alarms1 day/date
	i2c_write(0x0A);
	i2c_write(0xFF);
	i2c_stop();
	_delay_ms(20);

	i2c_start(0xD0);		//Clear Status registers
	i2c_write(0x0F);
	i2c_write(0x00);
	i2c_stop();
	_delay_ms(20);
}

void i2c_rtc_alarm_clr()
{
	i2c_start(0xD0);		//Clear Alarm Flag registers
	i2c_write(0x0F);
	i2c_write(0x00);
	i2c_stop();
}

void i2c_rtc_alarm2_set()
{
	int cmin=i2c_rtc_get_min();
	int chour=i2c_rtc_get_hour();
	int a2min,a2hour;
	
	if(cmin>=15)
	{
		a2min=0; a2hour=chour+1;
		if(chour==23) a2hour=0;
	}
	else 
	{a2min=15; a2hour=chour;}
	
	a2min=dec2bcd(a2min) & 0x7F;
	a2hour=dec2bcd(a2hour)& 0x3F;
	
	i2c_start(0xD0);
	i2c_write(0x0B);
	i2c_write(a2min);
	i2c_stop();
	i2c_start(0xD0);
	i2c_write(0x0C);
	i2c_write(a2hour);
	i2c_stop();
	
}

void i2c_rtc_alarm1_set()
{
	int cmin=i2c_rtc_get_min();
	int chour=i2c_rtc_get_hour();
	int a1min=30,a1hour=6,a1sec=0;

	if(((cmin<30) & (chour<=6)) | (chour<6))
	{
		a1min=30;
		a1hour=6;
	}
	if(((cmin>=30) & (chour>=6)) | (chour>=7))
	{
		a1min=30;
		a1hour=17;
	}
	if(((cmin>=30) & (chour>=17)) | (chour>=18))
	{
		a1min=30;
		a1hour=22;
	}
	if(((cmin>=30) & (chour>=22)) | (chour>=23))
	{
		a1min=30;
		a1hour=6;
	}

	a1min=dec2bcd(a1min) & 0x7F;
	a1hour=dec2bcd(a1hour) & 0x3F;
	a1sec=dec2bcd(a1sec) & 0x7F;	

	i2c_start(0xD0);
	i2c_write(0x08);
	i2c_write(a1min);
	i2c_stop();
	i2c_start(0xD0);
	i2c_write(0x09);
	i2c_write(a1hour);
	i2c_stop();
	i2c_start(0xD0);		
	i2c_write(0x07);
	i2c_write(a1sec);
	i2c_stop();
}

/*-----------------------------------*********-----------------------------------*/
void i2c_rtc_set_time(int sec, int min, int hour, int day, int date, int month, int year)
{
	i2c_start(0xD0);
	i2c_write(0x00);
	i2c_write(dec2bcd(sec));
	i2c_stop();

	i2c_start(0xD0);
	i2c_write(0x01);
	i2c_write(dec2bcd(min));
	i2c_stop();

	i2c_start(0xD0);
	i2c_write(0x02);
	i2c_write(dec2bcd(hour));
	i2c_stop();

	i2c_start(0xD0);
	i2c_write(0x03);
	i2c_write(dec2bcd(day));
	i2c_stop();

	i2c_start(0xD0);
	i2c_write(0x04);
	i2c_write(dec2bcd(date));
	i2c_stop();

	i2c_start(0xD0);
	i2c_write(0x05);
	i2c_write(dec2bcd(month));
	i2c_stop();

	i2c_start(0xD0);
	i2c_write(0x06);
	i2c_write(dec2bcd(year));
	i2c_stop();
}

