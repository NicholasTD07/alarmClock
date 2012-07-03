#include <REG52.H>
#include <stdio.h>
#include <intrins.h>

#define LCD_Data_BUS_Out    P2 
#define LCD_Data_BUS_In    P2


sbit LCD_EP=P0^7; 
sbit LCD_RW=P0^6; 
sbit LCD_RS=P0^5; 
 
 
code unsigned char LCD_InitialCode[]={0x30,0x30,0x30,0x38,0x01,0x06,0x0c}; 
//======================================================================== 
//  函数: void LCD_DataWrite(unsigned char Data) 
//  描述:  写一个字节的显示数据至 LCD中的显示缓冲RAM当中 
//  参数: Data  写入的数据  
//  返回:  无 
//======================================================================== 
void LCD_DataWrite(unsigned char Data) 
{ 
    unsigned int Read_Dat=0;

    LCD_EP = 0;     //EP、RS端口为低，RW 为高 
    LCD_RS = 0; 
    LCD_RW = 1; 

    do{ 
        LCD_Data_BUS_In = 0xff; 
        LCD_EP = 1; 
        //Read_Dat = LCD_Data_BUS_In&0x80;
        
        Read_Dat = LCD_Data_BUS_In&0x80; 
        LCD_EP = 0; 
    }while(Read_Dat!=0);    //读状态字并判断是否可进行读写操作 

    LCD_RW = 0;          //EP RW to Low 
    LCD_RS = 1;     //RS Hight 

    //LCD_Data_BUS_Out = Data; 
    LCD_Data_BUS_Out = Data; 

    LCD_EP = 1;          //EP to Hight 
    LCD_EP = 0;     //EP to low 

}


//======================================================================== 
//  函数: void LCD_RegWrite(unsigned char Command) 
//  描述:  写一个字节的数据至 LCD中的控制寄存器当中 
//  参数: Command    写入的数据（byte ）  
//  返回:  无 
//======================================================================== 
void LCD_RegWrite(unsigned char Command) 
{ 
    unsigned int Read_Dat=0; 

    LCD_EP = 0;     //EP、RS置低，RW 置高，表为读状态字 
    LCD_RS = 0; 
    LCD_RW = 1; 

    do{
        LCD_Data_BUS_In = 0xff; 
        LCD_EP = 1; 
        //Read_Dat = LCD_Data_BUS_In&0x80; 
  	    Read_Dat = LCD_Data_BUS_In&0x80; 
        LCD_EP = 0; 
    }while(Read_Dat!=0);    //读状态字并判断是否可进行读写操作 

    LCD_RW = 0;     //RW to Low，表为写指令 

    //LCD_Data_BUS_Out = Command;
    LCD_Data_BUS_Out = Command; 
  
    LCD_EP = 1;          //EP to Hight   
    LCD_EP = 0; 

} 


//======================================================================== 
//  函数: unsigned char LCD_DataRead(void) 
//  描述:  从LCD 中的显示缓冲RAM当中读一个字节的显示数据 
//  参数:  无   
//  返回:  读出的数据，低八位有效（byte ） 
//======================================================================== 
unsigned char LCD_DataRead(void) 
{ 
    unsigned char Read_Dat=0; 
    LCD_EP = 0;     //EP、RS置低，RW 置高，表为读状态字 
    LCD_RS = 0; 
    LCD_RW = 1; 

    do{ 
        LCD_Data_BUS_In = 0xff; 
        LCD_EP = 1; 
        //Read_Dat = LCD_Data_BUS_In&0x80; 
        Read_Dat = LCD_Data_BUS_In&0x80;
   	    LCD_EP = 0; 
    }while(Read_Dat!=0);    //读状态字并判断是否可进行读写操作 

    LCD_RS = 1;       //RS置高，表为读数据 
    LCD_EP = 1;          //EP to Hight 
    //Read_Dat = LCD_Data_BUS_In;  // 读出数据  
    Read_Dat = LCD_Data_BUS_In;  // 读出数据     
    LCD_EP = 0; 

    return Read_Dat; 
} 
//======================================================================== 
//  函数: unsigned char LCD_StatusRead(void) 
//  描述:  从LCD 中的显示缓冲RAM当中读一个字节的显示数据 
//  参数:  无   
//  返回:  读出的数据，低八位有效（byte ） 
//======================================================================== 
unsigned char LCD_StatusRead(void) 
{ 
    unsigned char Read_Dat=0; 

    LCD_EP = 0;     //EP、RS置低，RW 置高，表为读状态字 
    LCD_RS = 0; 
    LCD_RW = 1; 
    LCD_Data_BUS_In = 0xff; 
    LCD_EP = 1; 

    //Read_Dat = LCD_Data_BUS_In;  // 读状态字 
    Read_Dat = LCD_Data_BUS_In;  // 读状态字 

    LCD_EP = 0; 
    return Read_Dat; 
} 

//=======================================================================
//  函数: void LCD_Init(void) 
//  描述: LCD 初始化程序，在里面会完成LCD初始所需要设置的许多寄存器，具体如果 
//     用户想了解，建议查看DataSheet 当中各个寄存器的意义 
//  参数:  无  
//  返回:  无 
//  备注: 
//  版本: 
//      2007/11/14      First version 
//=======================================================================
// 延时程序 
void TimeDelay(int Time) 
{ 
    int i; 

    while(Time > 0) 
    { 
      for(i = 0;i < 50;i++) 
      { 
          _nop_(); 
      } 

          Time --; 
    } 
} 

void LCD_Init(void)
{ 
    unsigned char uiTemp=0,i; 
    unsigned char * Point; 

    //LCD驱动所使用到的端口的初始化 
    Point = (unsigned char *)LCD_InitialCode;   // 获取初始化序列数据的首地址 
    LCD_EP = 0; 
    LCD_RS = 0; 
    LCD_RW = 0; 

    for(i=0;i<4;i++) 
    { 
         uiTemp = *Point++; 
         LCD_Data_BUS_Out = uiTemp; 
         LCD_EP = 1;        //EP to Hight 
         LCD_EP = 0;        //EP to Hight 
         TimeDelay(5); //延一定的时间，一般要求 4.5ms 以上就可以，没有那么严格的了
    } 
   LCD_RegWrite(*Point++); 
   LCD_RegWrite(*Point++); 
   LCD_RegWrite(*Point++); 
} 


void onLCD(unsigned char y, unsigned char x, unsigned char * string)
{

    // Set the display position.
    if( y == 1)
    {
        LCD_RegWrite( 0xc0 + x );
    }
    else
    {
        LCD_RegWrite( 0x80 + x );
    }

    // Display the text.
    while(*string!=0)      //显示字符串 
    { 
        LCD_DataWrite(*string); 
        string++; 
    } 

}

void clearLCD(void)
{
    LCD_RegWrite(0x01);
}
