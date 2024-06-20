#include <stdio.h>
#include "lcd.h"
#include "font.h"
#include "string.h"
#include "driver/spi_master.h"
#include "esp_spi_lcd.h"


#define GPIO_ONPUT_PIN_SEL (1ULL << PIN_CS)  \
                          |(1ULL << PIN_SDI) \
						  |(1ULL << PIN_RST) \
						  |(1ULL << PIN_SCK) \
						  |(1ULL << PIN_DC)   //配置GPIO_OUT寄存器

//LCD的画笔颜色和背景色
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色
//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

extern spi_device_handle_t spi;

void lcd_gpio_config(void)
{
	gpio_config_t IO_Config;

	IO_Config.pin_bit_mask = GPIO_ONPUT_PIN_SEL;
	IO_Config.mode = GPIO_MODE_DEF_OUTPUT;               /*输出 */
	IO_Config.pull_up_en = 0;       /*上拉*/
	IO_Config.pull_down_en = 0;
	IO_Config.intr_type = GPIO_INTR_DISABLE;    /*不使能中断*/
	gpio_config(&IO_Config);

	//函数定义在 esp_spi_lcd.c
	lcd_spi_init();

}
//写命令函数
//data:命令值
void LCD_WR_REG(u16 data)
{
	u8 i;

	SPI_CS(0);
	SPI_DC(0);//RS=0 命令
	i = (u8)data;
	spi_write(spi, &i, 1);
	SPI_CS(1);
}
//写数据函数
//data:寄存器值
void LCD_WR_DATA(u16 data)
{
	u8 i;

	SPI_CS(0);
	SPI_DC(1);//RS=1 数据
	i = (u8)data;
	spi_write(spi, &i, 1);
	SPI_CS(1);
}
//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(u16 RGB_Code)
{
	//写十六位GRAM
	u8 rgb[2];

	rgb[1] = RGB_Code;
	rgb[0] = RGB_Code >> 8;
	SPI_CS(0);
	SPI_DC(1);
	spi_write(spi, rgb, 2);
	SPI_CS(1);
}

//LCD使用 ESP32-SPI-DMA 写GRAM
void LCD_WriteRAM_DMA(u16 *data, u16 len)
{
	u16 i, high, low,l;
	l = len / 2;
	for(i=0; i<l; i++){
		high = (data[i] >> 8) & 0xFF;
		low  = (data[i] << 8) & 0xFF00;
		data[i] = high | low;
	}

    SPI_CS(0);
    SPI_DC(1);
    spi_write(spi, (u8*)data, len);
    SPI_CS(1);
}
//写寄存器
//LCD_Reg:寄存器编号
//LCD_RegValue:要写入的值
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{
	LCD_WR_REG(LCD_Reg);
	LCD_WriteRAM(LCD_RegValue);
}

//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}
//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
	u32 index=0;
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height;  //得到总点数
	LCD_SetCursor(0x00,0x0000);	//设置光标位置
	LCD_WriteRAM_Prepare();     //开始写入GRAM
	for(index=0;index<totalpoint;index++){
		LCD_WriteRAM(color);
	}
}
//清屏函数使用ESP32 -  SPI - DMA方式
void LCD_DMA_Clear(u16 color)
{
	u16 i, g;

	LCD_SetCursor(0x00,0x0000); //设置光标位置
	LCD_WriteRAM_Prepare();     //开始写入GRAM
	for(i=0;i<10;i++){
		for(g=0; g<2048; g++){
			lcddev.farme_buff[g] = color;
		}
		LCD_WriteRAM_DMA(lcddev.farme_buff, 4096);
	}
}

//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(u8 i)
{
	while(i--);
}

//LCD开启显示
void LCD_DisplayOn(void)
{
	LCD_WR_REG(0X29);	//开启显示
}
//LCD关闭显示
void LCD_DisplayOff(void)
{
	LCD_WR_REG(0X28);	//关闭显示
}
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
		LCD_WR_REG(lcddev.setxcmd);
		LCD_WR_DATA(Xpos>>8);
		LCD_WR_DATA(Xpos&0XFF);
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(Ypos>>8);
		LCD_WR_DATA(Ypos&0XFF);
}

//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//设置光标位置
	LCD_WriteRAM_Prepare();	//开始写入GRAM
	LCD_WriteRAM(POINT_COLOR);
}
//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{
		//设置光标位置
		LCD_SetCursor(x,y);
		//写入颜色
		LCD_WriteReg(lcddev.wramcmd,color);
}


//dir:方向选择 	0-0度旋转，1-180度旋转，2-270度旋转，3-90度旋转
void LCD_Display_Dir(u8 dir)
{
	if(dir==0||dir==1)        //竖屏
	{
		lcddev.dir=0;	      //竖屏
		lcddev.width=128;
		lcddev.height=160;
		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;
		if(dir==0)            //0-0度旋转
		{
			LCD_WR_REG(0x36);
			LCD_WR_DATA((0<<3)|(1<<7)|(1<<6)|(0<<5));
		}else{                //1-180度旋转
			LCD_WR_REG(0x36);
			LCD_WR_DATA((0<<3)|(0<<7)|(0<<6)|(0<<5));
		}
	}else if(dir==2||dir==3)
	{
		lcddev.dir=1;     //横屏
		lcddev.width=160;
		lcddev.height=128;
		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;
		if(dir==2)				//2-270度旋转
		{
			LCD_WR_REG(0x36);
			LCD_WR_DATA((0<<3)|(0<<7)|(1<<6)|(1<<5));

		}else							//3-90度旋转
		{
			LCD_WR_REG(0x36);
			LCD_WR_DATA((0<<3)|(1<<7)|(0<<6)|(1<<5));
		}
	}
	//设置显示区域
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(0);
	LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.width-1)>>8);
	LCD_WR_DATA((lcddev.width-1)&0XFF);
	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(0);
	LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.height-1)>>8);
	LCD_WR_DATA((lcddev.height-1)&0XFF);


}
//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{
	u16 twidth,theight;
	twidth=sx+width-1;
	theight=sy+height-1;
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(sx>>8);
	LCD_WR_DATA(sx&0XFF);
	LCD_WR_DATA(twidth>>8);
	LCD_WR_DATA(twidth&0XFF);
	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(sy>>8);
	LCD_WR_DATA(sy&0XFF);
	LCD_WR_DATA(theight>>8);
	LCD_WR_DATA(theight&0XFF);
}


//读数据
//返回值:读到的值
u16 LCD_RD_DATA(void)
{
  u8 i;
	u16 t=0;
		for(i=0; i<8; i++)
		{
			SPI_SCK(0);
			//opt_delay(10);//延时
			t=t << 1 | SPI_SDI_RD();
			SPI_SCK(1);
      //opt_delay(10);
		}
	return t;


}

//读显示屏 ID
u16 LCD_Read_ID(u8 reg)
{
	u8 i;
	SPI_CS(0);
	SPI_DC(0);//RS=0 命令
	for(i=0; i<8; i++)
	{
		if (reg & 0x80)
		 SPI_SDI(1);
		else
		 SPI_SDI(0);
		reg <<= 1;
		SPI_SCK(0);
		SPI_SCK(1);
	}
	SPI_SCK(0);
	SPI_SCK(1);
	LCD_RD_DATA(); //7C
	lcddev.id=LCD_RD_DATA();  //89
	lcddev.id<<=8;
	lcddev.id|=LCD_RD_DATA(); //F0
	SPI_CS(1);
	return lcddev.id;
}


//读取个某点的颜色值
//x,y:坐标
//返回值:此点的颜色
u16 LCD_ReadPoint(u16 x,u16 y)
{
	int i=0,r=0,g=0,b=0,reg=0x2e;
 	u16 color=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回
	LCD_SetCursor(x,y);
	SPI_CS(0);
	SPI_DC(0);
	for(i=0; i<8; i++)
	{
		if (reg & 0x80)
		 SPI_SDI(1);
		else
		 SPI_SDI(0);
		reg <<= 1;
		SPI_SCK(0);
		SPI_SCK(1);
	}
	for(i=0; i<9; i++)							//第一次空读 后三次分别为R G B
	{
		SPI_SCK(0);
		SPI_SCK(1);
	}
	for(i=0; i<8; i++)
	{
		SPI_SCK(0);		r=r << 1 | SPI_SDI_RD();
		SPI_SCK(1);
	}
	for(i=0; i<8; i++)
	{
		SPI_SCK(0);		g=g << 1 | SPI_SDI_RD();
		SPI_SCK(1);
	}
	for(i=0; i<8; i++)
	{
		SPI_SCK(0);		b=b << 1 | SPI_SDI_RD();
		SPI_SCK(1);

	}
	color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	SPI_CS(1);
	return color;
}

//在指定区域内填充指定颜色
//区域大小:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:要填充的颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{
	u16 i,j;
	u16 xlen=0;
	u16 temp;
	if((lcddev.id==0X6804)&&(lcddev.dir==1))	//6804横屏的时候特殊处理
	{
		temp=sx;
		sx=sy;
		sy=lcddev.width-ex-1;
		ex=ey;
		ey=lcddev.width-temp-1;
 		lcddev.dir=0;
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;
		LCD_Fill(sx,sy,ex,ey,color);
 		lcddev.dir=1;
  		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;
 	}else
	{
		xlen=ex-sx+1;
		for(i=sy;i<=ey;i++)
		{
		 	LCD_SetCursor(sx,i);      				//设置光标位置
			LCD_WriteRAM_Prepare();     			//开始写入GRAM
			for(j=0;j<xlen;j++)LCD_WriteRAM(color);	//设置光标位置
		}
	}
}
//在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//设置光标位置
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)LCD_WriteRAM(color[i*width+j]);//写入数据
	}
}
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向
	else if(delta_x==0)incx=0;//垂直线
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;//水平线
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )//画线输出
	{
		LCD_DrawPoint(uRow,uCol);//画点
		xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}
//画矩形
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0
		LCD_DrawPoint(x0+b,y0+a);             //4
		LCD_DrawPoint(x0+a,y0+b);             //6
		LCD_DrawPoint(x0-a,y0+b);             //1
 		LCD_DrawPoint(x0-b,y0+a);
		LCD_DrawPoint(x0-a,y0-b);             //2
  	LCD_DrawPoint(x0-b,y0-a);             //7
		a++;
		//使用Bresenham算法画圆
		if(di<0)di +=4*a+6;
		else
		{
			di+=10+4*(a-b);
			b--;
		}
	}
}
//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}
	}
}
//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)result*=m;
	return result;
}
//显示数字,高位为0,则不显示
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//color:颜色
//num:数值(0~4294967295);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1;

		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0);
	}
}
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);
 				continue;
			}else enshow=1;

		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01);
	}
}
//显示字符串
//x,y:起点坐标
//width,height:区域大小
//size:字体大小
//*p:字符串起始地址
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }
}


//显示汉字或者字符串
void Show_Str(u16 x, u16 y,u8 *str,u8 size,u8 mode)
{
	u16 x0=x;
  	u8 bHz=0;     //字符或者中文
    while(*str!=0)//数据未结束
    {
        if(!bHz)
        {
			if(x>(lcddev.width-size/2)||y>(lcddev.height-size))
			return;
	        if(*str>0x80)bHz=1;//中文
	        else              //字符
	        {
		        if(*str==0x0D)//换行符号
		        {
		            y+=size;
		            x=x0;
		            str++;
		        }
		        else
				{
					if(size>=24)//字库中没有集成12X24 16X32的英文字体,用8X16代替
					{
					LCD_ShowChar(x,y,*str,24,mode);
					x+=12; //字符,为全字的一半
					}
					else
					{
					LCD_ShowChar(x,y,*str,size,mode);
					x+=size/2; //字符,为全字的一半
					}
				}
				str++;

	        }
        }
    }
}


//显示40*40图片
void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p) //显示40*40图片
{
  	int i;
	unsigned char picH,picL;
	LCD_Set_Window(x,y,40,40);
	LCD_WriteRAM_Prepare();

    for(i=0;i<40*40;i++)
	{
	 	picL=*(p+i*2);	//数据低位在前
		picH=*(p+i*2+1);
		LCD_WriteRAM(picH<<8|picL);
	}
	LCD_Set_Window(0,0,lcddev.width,lcddev.height);//恢复显示窗口为全屏

}

//居中显示
void Gui_StrCenter(u16 x, u16 y, u8 *str,u8 size,u8 mode)
{
	u16 x1;
	u16 len=strlen((const char *)str);
	if(size>16)
	{
		x1=(lcddev.width-len*(size/2))/2;
	}else
	{
		x1=(lcddev.width-len*8)/2;
	}

	Show_Str(x+x1,y,str,size,mode);
}


void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//清屏
 	POINT_COLOR=BLUE;//设置字体为蓝色
	BACK_COLOR=WHITE;
	LCD_ShowString(lcddev.width-24,0,200,16,16,(u8*)"RST");//显示清屏区域
  POINT_COLOR=RED;//设置画笔蓝色
}
////////////////////////////////////////////////////////////////////////////////
//电容触摸屏专有部分
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);
}
//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++)
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside
		{
 			if (x>imax)
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}

//画一条粗线
//(x1,y1),(x2,y2):线条的起始坐标
//size：线条的粗细程度
//color：线条的颜色
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	if(x1<size|| x2<size||y1<size|| y2<size)return;
	delta_x=x2-x1; //计算坐标增量
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向
	else if(delta_x==0)incx=0;//垂直线
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;//水平线
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )//画线输出
	{
		gui_fill_circle(uRow,uCol,size,color);//画点
		xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

//初始化lcd
void LCD_Init(void)
{
	lcd_gpio_config();
	lcddev.farme_buff = spi_memory_malloc(4096);
	SPI_RST(1);
	delay_ms(1);
	SPI_RST(0);
	delay_ms(10);
	SPI_RST(1);
	delay_ms(120);

	//************* Start Initial Sequence **********//
	delay_ms(120);                //ms

	LCD_WR_REG(0x11);     //Sleep out

	delay_ms(120);                //delay_ms 120ms

	LCD_WR_REG(0xB1);     //In normal mode
	LCD_WR_DATA(0x00);   //frame rate=85.3Hz
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x2B);

	LCD_WR_REG(0xB2);     //In Idle mode
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x01);

	LCD_WR_REG(0xB3);     //In partial mode
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x01);

	LCD_WR_REG(0xB4);     //DOT inversion Display Inversion Control
	LCD_WR_DATA(0x03);

	LCD_WR_REG(0xB9);     //In normal mode
	LCD_WR_DATA(0xFF);
	LCD_WR_DATA(0x83);
	LCD_WR_DATA(0x47);

	LCD_WR_REG(0xC0);     //VRH: Set the GVDD
	LCD_WR_DATA(0xA2);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x84);

	LCD_WR_REG(0xC1);     //set VGH/ VGL
	LCD_WR_DATA(0x02);   //??02 VGH=16.6 VGL=-7.5  00 VGH=11.6 VGL=-7.5  06 VGH=16.6  VGL=-10

	LCD_WR_REG(0xC2);     //APA: adjust the operational amplifier DCA: adjust the booster Voltage
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x00);

	LCD_WR_REG(0xC3);     //In Idle mode (8-colors)
	LCD_WR_DATA(0x8A);
	LCD_WR_DATA(0x2A);

	LCD_WR_REG(0xC4);     //In partial mode + Full color
	LCD_WR_DATA(0x8A);
	LCD_WR_DATA(0xEE);

	LCD_WR_REG(0xC5);     //VCOM
	LCD_WR_DATA(0x09);

	LCD_WR_REG(0x20);     //Display inversion

	LCD_WR_REG(0xC7);
	LCD_WR_DATA(0x10);

	LCD_WR_REG(0x36);     //MX, MY, RGB mode
	LCD_WR_DATA(0xC0); //08

	LCD_WR_REG(0xE0);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x1C);
	LCD_WR_DATA(0x1B);
	LCD_WR_DATA(0x1A);
	LCD_WR_DATA(0x2F);
	LCD_WR_DATA(0x28);
	LCD_WR_DATA(0x20);
	LCD_WR_DATA(0x24);
	LCD_WR_DATA(0x23);
	LCD_WR_DATA(0x22);
	LCD_WR_DATA(0x2A);
	LCD_WR_DATA(0x36);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x10);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x1A);
	LCD_WR_DATA(0x1A);
	LCD_WR_DATA(0x1A);
	LCD_WR_DATA(0x2E);
	LCD_WR_DATA(0x27);
	LCD_WR_DATA(0x21);
	LCD_WR_DATA(0x24);
	LCD_WR_DATA(0x24);
	LCD_WR_DATA(0x22);
	LCD_WR_DATA(0x2A);
	LCD_WR_DATA(0x35);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x10);

	LCD_WR_REG(0x35);     //65k mode
	LCD_WR_DATA(0x00);

	LCD_WR_REG(0x3A);     //65k mode
	LCD_WR_DATA(0x05);

	LCD_Display_Dir(0);   //dir:方向选择 	0-0度旋转，1-180度旋转，2-270度旋转，3-90度旋转

	LCD_WR_REG(0x29);     //Display on
	LCD_DMA_Clear(BACK_COLOR);
}
