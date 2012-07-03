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
//  ����: void LCD_DataWrite(unsigned char Data) 
//  ����:  дһ���ֽڵ���ʾ������ LCD�е���ʾ����RAM���� 
//  ����: Data  д�������  
//  ����:  �� 
//======================================================================== 
void LCD_DataWrite(unsigned char Data) 
{ 
    unsigned int Read_Dat=0;

    LCD_EP = 0;     //EP��RS�˿�Ϊ�ͣ�RW Ϊ�� 
    LCD_RS = 0; 
    LCD_RW = 1; 

    do{ 
        LCD_Data_BUS_In = 0xff; 
        LCD_EP = 1; 
        //Read_Dat = LCD_Data_BUS_In&0x80;
        
        Read_Dat = LCD_Data_BUS_In&0x80; 
        LCD_EP = 0; 
    }while(Read_Dat!=0);    //��״̬�ֲ��ж��Ƿ�ɽ��ж�д���� 

    LCD_RW = 0;          //EP RW to Low 
    LCD_RS = 1;     //RS Hight 

    //LCD_Data_BUS_Out = Data; 
    LCD_Data_BUS_Out = Data; 

    LCD_EP = 1;          //EP to Hight 
    LCD_EP = 0;     //EP to low 

}


//======================================================================== 
//  ����: void LCD_RegWrite(unsigned char Command) 
//  ����:  дһ���ֽڵ������� LCD�еĿ��ƼĴ������� 
//  ����: Command    д������ݣ�byte ��  
//  ����:  �� 
//======================================================================== 
void LCD_RegWrite(unsigned char Command) 
{ 
    unsigned int Read_Dat=0; 

    LCD_EP = 0;     //EP��RS�õͣ�RW �øߣ���Ϊ��״̬�� 
    LCD_RS = 0; 
    LCD_RW = 1; 

    do{
        LCD_Data_BUS_In = 0xff; 
        LCD_EP = 1; 
        //Read_Dat = LCD_Data_BUS_In&0x80; 
  	    Read_Dat = LCD_Data_BUS_In&0x80; 
        LCD_EP = 0; 
    }while(Read_Dat!=0);    //��״̬�ֲ��ж��Ƿ�ɽ��ж�д���� 

    LCD_RW = 0;     //RW to Low����Ϊдָ�� 

    //LCD_Data_BUS_Out = Command;
    LCD_Data_BUS_Out = Command; 
  
    LCD_EP = 1;          //EP to Hight   
    LCD_EP = 0; 

} 


//======================================================================== 
//  ����: unsigned char LCD_DataRead(void) 
//  ����:  ��LCD �е���ʾ����RAM���ж�һ���ֽڵ���ʾ���� 
//  ����:  ��   
//  ����:  ���������ݣ��Ͱ�λ��Ч��byte �� 
//======================================================================== 
unsigned char LCD_DataRead(void) 
{ 
    unsigned char Read_Dat=0; 
    LCD_EP = 0;     //EP��RS�õͣ�RW �øߣ���Ϊ��״̬�� 
    LCD_RS = 0; 
    LCD_RW = 1; 

    do{ 
        LCD_Data_BUS_In = 0xff; 
        LCD_EP = 1; 
        //Read_Dat = LCD_Data_BUS_In&0x80; 
        Read_Dat = LCD_Data_BUS_In&0x80;
   	    LCD_EP = 0; 
    }while(Read_Dat!=0);    //��״̬�ֲ��ж��Ƿ�ɽ��ж�д���� 

    LCD_RS = 1;       //RS�øߣ���Ϊ������ 
    LCD_EP = 1;          //EP to Hight 
    //Read_Dat = LCD_Data_BUS_In;  // ��������  
    Read_Dat = LCD_Data_BUS_In;  // ��������     
    LCD_EP = 0; 

    return Read_Dat; 
} 
//======================================================================== 
//  ����: unsigned char LCD_StatusRead(void) 
//  ����:  ��LCD �е���ʾ����RAM���ж�һ���ֽڵ���ʾ���� 
//  ����:  ��   
//  ����:  ���������ݣ��Ͱ�λ��Ч��byte �� 
//======================================================================== 
unsigned char LCD_StatusRead(void) 
{ 
    unsigned char Read_Dat=0; 

    LCD_EP = 0;     //EP��RS�õͣ�RW �øߣ���Ϊ��״̬�� 
    LCD_RS = 0; 
    LCD_RW = 1; 
    LCD_Data_BUS_In = 0xff; 
    LCD_EP = 1; 

    //Read_Dat = LCD_Data_BUS_In;  // ��״̬�� 
    Read_Dat = LCD_Data_BUS_In;  // ��״̬�� 

    LCD_EP = 0; 
    return Read_Dat; 
} 

//=======================================================================
//  ����: void LCD_Init(void) 
//  ����: LCD ��ʼ����������������LCD��ʼ����Ҫ���õ����Ĵ������������ 
//     �û����˽⣬����鿴DataSheet ���и����Ĵ��������� 
//  ����:  ��  
//  ����:  �� 
//  ��ע: 
//  �汾: 
//      2007/11/14      First version 
//=======================================================================
// ��ʱ���� 
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

    //LCD������ʹ�õ��Ķ˿ڵĳ�ʼ�� 
    Point = (unsigned char *)LCD_InitialCode;   // ��ȡ��ʼ���������ݵ��׵�ַ 
    LCD_EP = 0; 
    LCD_RS = 0; 
    LCD_RW = 0; 

    for(i=0;i<4;i++) 
    { 
         uiTemp = *Point++; 
         LCD_Data_BUS_Out = uiTemp; 
         LCD_EP = 1;        //EP to Hight 
         LCD_EP = 0;        //EP to Hight 
         TimeDelay(5); //��һ����ʱ�䣬һ��Ҫ�� 4.5ms ���ϾͿ��ԣ�û����ô�ϸ����
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
    while(*string!=0)      //��ʾ�ַ��� 
    { 
        LCD_DataWrite(*string); 
        string++; 
    } 

}

void clearLCD(void)
{
    LCD_RegWrite(0x01);
}
