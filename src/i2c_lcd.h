void i2c_lcd_exec_cmd(uint8_t);

void i2c_lcd_init_4bit()
{
	i2c_start(0x7E);
	{
		int hex=0x38;
		i2c_write(hex); _delay_ms(1);
		i2c_write(hex|0x04); _delay_ms(1);
		i2c_write(hex);
		_delay_ms(20);
		i2c_write(hex|0x04); _delay_ms(1);
		i2c_write(hex);
		_delay_ms(6);
		i2c_write(hex|0x04); _delay_ms(1);
		i2c_write(hex);
		_delay_ms(1);
		hex=0x28;
		i2c_write(hex); _delay_ms(1);
		i2c_write(hex|0x04); _delay_ms(1);
		i2c_write(hex);
		_delay_ms(6);
		i2c_stop();
	}
	_delay_ms(2);
	
	i2c_lcd_exec_cmd(0x28);     //4bit mode_2 lines_5x7 font
}
	
void i2c_lcd_disp_init()
{
	i2c_lcd_exec_cmd(0x01);
	i2c_lcd_exec_cmd(0x06);
	i2c_lcd_exec_cmd(0x0C);
	
}

void i2c_lcd_exec_cmd(uint8_t cmd)
{
	i2c_start(0x7E);
	{
		int hex = ((cmd & 0xF0) | 0x08);
		i2c_write(hex); _delay_ms(1);
		i2c_write(hex|0x04); _delay_ms(1);
		i2c_write(hex); _delay_ms(1);			
		hex = ( ((cmd<<4) & 0xF0) | 0x08);
		i2c_write(hex); _delay_ms(1);
		i2c_write(hex|0x04); _delay_ms(1);
		i2c_write(hex); _delay_ms(1);
		i2c_stop();
	}
}

void i2c_lcd_disp_char(int dchar)
{
	i2c_start(0x7E);
	{
		int hex = ((dchar & 0xF0) | 0x09);
		i2c_write(hex); _delay_ms(1);
		i2c_write(hex|0x04); _delay_ms(1);
		i2c_write(hex); _delay_ms(1);			
		hex = ( ((dchar<<4) & 0xF0) | 0x09);
		i2c_write(hex); _delay_ms(1);
		i2c_write(hex|0x04); _delay_ms(1);
		i2c_write(hex); _delay_ms(1);
		i2c_stop();
	}
}

void i2c_lcd_disp_str(char *dstr)
{
	int i;
	//i2c_lcd_disp_init();
	for(i=0;dstr[i]!='\0';i++)
	{
		i2c_lcd_disp_char(dstr[i]);
		if(i==15) i2c_lcd_exec_cmd(0xC0);
	}
}

void i2c_lcd_disp_num(long unsigned int num, int dp)
{
	if(num<99) dp=1;
	int rnum[5],i=0;
	for(i=0;num!=0;i++)
	{
		rnum[i]=num%10;
		num/=10;
	}
		
	i=i-1;
	while(i>=0)
	{	
		if(dp==0) i2c_lcd_disp_char('.');
		--dp;
		switch(rnum[i])
		{
			case 0:
			{ i2c_lcd_disp_char(0x30); break;}
			case 1:
			{ i2c_lcd_disp_char(0x31); break;}
			case 2:
			{ i2c_lcd_disp_char(0x32); break;}
			case 3:
			{ i2c_lcd_disp_char(0x33); break;}
			case 4:
			{ i2c_lcd_disp_char(0x34); break;}
			case 5:
			{ i2c_lcd_disp_char(0x35); break;}
			case 6:
			{ i2c_lcd_disp_char(0x36); break;}
			case 7:
			{ i2c_lcd_disp_char(0x37); break;}
			case 8:
			{ i2c_lcd_disp_char(0x38); break;}
			case 9:
			{ i2c_lcd_disp_char(0x39); break;}

		}
		--i;
		
	}
}

void i2c_lcd_disp_time(long unsigned int num, char sym)
{
	
	int rnum[5]={0,0,0,0,0},i=1;
	for(i=0;num!=0;i++)
	{
		rnum[i]=num%10;
		num/=10;
	}
		
	i=i-1;
	if(i<0) i=0;
	if(sym!='\0')
	i2c_lcd_disp_char(sym);
	while(i>=0)
	{	
		switch(rnum[i])
		{
			case 0:
			{ i2c_lcd_disp_char(0x30); break;}
			case 1:
			{ i2c_lcd_disp_char(0x31); break;}
			case 2:
			{ i2c_lcd_disp_char(0x32); break;}
			case 3:
			{ i2c_lcd_disp_char(0x33); break;}
			case 4:
			{ i2c_lcd_disp_char(0x34); break;}
			case 5:
			{ i2c_lcd_disp_char(0x35); break;}
			case 6:
			{ i2c_lcd_disp_char(0x36); break;}
			case 7:
			{ i2c_lcd_disp_char(0x37); break;}
			case 8:
			{ i2c_lcd_disp_char(0x38); break;}
			case 9:
			{ i2c_lcd_disp_char(0x39); break;}

		}
		--i;
		
	}
	
}
