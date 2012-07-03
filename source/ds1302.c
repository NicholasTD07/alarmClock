/*

    Description:
        C file for ds1302.

    Author:
        Duowen Tian

    Version: 
        v0.0.1 by Duowen Tian

*/

#include <REG52.H>
#include <stdio.h>

#include "../include/ds1302.h"


sbit sclk = P3^3;
sbit timerIO = P3^4;
sbit ce = P3^5;

sbit A7 = ACC^7;

void startSignal(void)
{

    ce = 0;
    sclk = 0;
    ce = 1;

}


void endSignal(void)
{

    sclk = 1; // ?
    ce = 0;

}


// Before using this, must use startSignal FIRST.
// After using this, must use endSignal AFTER.
void sendByte(unsigned char _data)
{

    unsigned char i, temp;
    temp = _data;

    for(i=0; i<8; i++)
    {
        sclk = 0;
        timerIO = (bit)(temp & 0x01);
        temp >>= 1;
        sclk =1;
    } // end of sending data;

} // end of sendByte();


// Before using this, must use startSignal FIRST.
// After using this, must use endSignal AFTER.
unsigned char readByte(void)
{

    unsigned char i, temp;

    for(i=0; i<8; i++)
    {
        A7 = timerIO;
        sclk = 0;
        ACC >>= 1;
        sclk = 1;
    }

    temp = ACC;

    return temp;

}


void writeTimer(unsigned char address, unsigned char _data)
{

    startSignal();

    sendByte(address);
    sendByte(_data);

    endSignal();

} // end of writeTimer;


unsigned char readTimer(unsigned char address)
{

    unsigned char temp, _data;

    startSignal();

    sendByte(address);
    temp = readByte();

    // temp = ACC
    // data = ACC%16 + ACC/16*10;

    endSignal();

    _data = temp / 16 * 10 + temp % 16;
    return _data;

}


unsigned char toBCD(unsigned char dec)
{
    return ( (dec / 10)<<4 | (dec % 10) );
}
void setTime(struct Time time)
 {

    writeTimer(WR_PROTECT, 0x00); // Clear write-protection.

    // Write date.
    writeTimer(WR_YEAR, toBCD(time.year));
    writeTimer(WR_MONTH, toBCD(time.month));
    writeTimer(WR_DATE, toBCD(time.date));
    writeTimer(WR_DAY, toBCD(time.day));

    // Write clock.
    writeTimer(WR_HOUR, toBCD(time.hour));
    writeTimer(WR_MIN, toBCD(time.min));
    writeTimer(WR_SEC, toBCD(time.sec));

    writeTimer(WR_PROTECT, 0x80); // Set write-protection.

} // end of setTime.


struct Time getTime(void)
{

    struct Time time;

    time.year = readTimer(RD_YEAR);
    time.month = readTimer(RD_MONTH);
    time.date = readTimer(RD_DATE);
    time.day = readTimer(RD_DAY);

    time.hour = readTimer(RD_HOUR);
    time.min = readTimer(RD_MIN);
    time.sec = readTimer(RD_SEC);

    return time;
} // end of getTime.


void initTimer(void)
{
    struct Time initTime;

    initTime.year = 12;
    initTime.month = 06;
    initTime.date = 30;
    initTime.day = 06;
    initTime.hour = 11;
    initTime.min = 59;
    initTime.sec = 55;


    writeTimer(WR_PROTECT, 0x00); // Clear write-protection.
    writeTimer(WR_CHARGE, 0x00); // Disable charge function.

    setTime(initTime);

    writeTimer(WR_PROTECT, 0x80); // Set write-protection.

} // end of initTimer

