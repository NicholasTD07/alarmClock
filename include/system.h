/*

    Description:
        Header file for STC89C52RC/RD.

    Author:
        Duowen Tian

    Version: 
        v0.0.1 by Duowen Tian

*/

#ifndef  __SYSTEM_H__
#define __SYSTEM_H__

#include "../include/ds1302.h"

// Define I/O ports.

sbit led = P1^0;

// keys
sbit menuKey = P1^1;
sbit selectKey = P1^2;
sbit cancelKey = P1^3;

// end of I/O ports' definition.


// Define data structure.

typedef enum { false = 0, true = !false } bool;


struct MenuInfo
{

    unsigned char modeMin;
    unsigned char modeMax;

    unsigned char onMin;
    unsigned char alarmMax; // onWhat in alarm mode.
    unsigned char timeMax; // onWhat in time mode.

    unsigned char mode;
    unsigned char onWhat;

    bool confirm; // key1
    bool add; // key2
    bool cancel; // key3

    //bool blink;
    bool blank;
    bool selected;

};


struct SystemInfo
{

    struct MenuInfo menuInfo;

    struct Time alarmClock;
    struct Time startTime; // for counter
    struct Time newTime;

    struct Time now;

    bool counterStarted;
    bool alarm;

};

// end of definition of data structure.

#endif
