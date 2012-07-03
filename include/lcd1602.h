/*

    Description:
        Header file for lcd1602.

    Author:
        Duowen Tian

    Version: 
        v0.0.1 by Duowen Tian

*/

#ifndef _LCD1602_H_
#define _LCD1602_H_

// Declaration of functions.
extern void TimeDelay(int Time);
extern void LCD_Init(void);
extern void onLCD(unsigned char y, unsigned char x, unsigned char * string);
extern void clearLCD(void);

#endif
