/*

    Description:
        Header file for ds1302.

    Author:
        Duowen Tian

    Version: 
        v0.0.1 by Duowen Tian

*/

#ifndef _DS1302_H_
#define _DS1302_H_

// Define control words.
#define WR_PROTECT 0x8e
#define WR_CHARGE 0x90

#define WR_SEC 0x80
#define WR_MIN 0x82
#define WR_HOUR 0x84
#define WR_DATE 0x86
#define WR_MONTH 0x88
#define WR_YEAR 0x8c
#define WR_DAY 0x8a // 1-7

#define RD_SEC 0x81
#define RD_MIN 0x83
#define RD_HOUR 0x85
#define RD_DATE 0x87
#define RD_MONTH 0x89
#define RD_YEAR 0x8d
#define RD_DAY 0x8b // 1-7

// end of control words.

// Define data structure.

struct Time
{

    unsigned char year;
    unsigned char month;
    unsigned char date;
    unsigned char day;

    unsigned char hour;
    unsigned char min;
    unsigned char sec;

};

// end of data structure.

// Declaration of functions.

extern void setTime(struct Time time);
extern struct Time getTime(void);
extern void initTimer(void);

// end of declaration.

#endif
