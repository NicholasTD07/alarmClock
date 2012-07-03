/*

    Description:
        C file for main programme.

    Author:
        Duowen Tian

    Version: 
        v0.0.1 by Duowen Tian

*/

#include <REG52.H>
#include <stdio.h>
#include <string.h>

#include "../include/system.h"
#include "../include/ds1302.h"
#include "../include/lcd1602.h"

// define constants.
#define TIME0 20*1000 // us
#define TIMS (65536 - TIME0)

// interrupt
void timer0_ISR(void);

// initialize.
void initSys(void);
void initTimer0(void);
struct MenuInfo getMenuInfo(void);
struct SystemInfo getSysInfo(void);

// control.
void keyScan(void);
void contentControl(void);
void displayControl(void);

// display.
void timeOnLCD(void);
void counterOnLCD(void);
void alarmOnLCD(void);

// helper.
unsigned char maxDays(void);
unsigned char dayInWeek(void);
unsigned char * toString(unsigned char);
unsigned char * whichDay(unsigned char dayInDight);

// end of declaration.


// data.
bool menuKeyPressed, selectKeyPressed, cancelKeyPressed;
bool menuKeyTemp, selectKeyTemp, cancelKeyTemp;
struct SystemInfo sysInfo;


char code dx516[3] _at_ 0x003b; 
void main (void)
{

    sysInfo = getSysInfo();
 
    initSys();
 
    TimeDelay(1000);

    led=1; 
    clearLCD();
 
    TimeDelay(2000);

    while(1)  
    { 

        sysInfo.now = getTime();

        //led = ~led;

 	    TimeDelay(400); 


        keyScan();
        contentControl();
        displayControl();

    } 
 
}


unsigned char timerCount = 0;
void timer0_ISR(void) interrupt 1 using 1
{

    TL0 = TIMS;
    TH0 = TIMS >> 8;

    // Keys
    if (timerCount%5)
    {
        //if (menuKey == 0)
        if (menuKeyTemp & ~menuKey | menuKeyPressed & (menuKeyTemp^ (~menuKey)))
        {
            menuKeyPressed = true;
        }
        //if (selectKey == 0)
        if (selectKeyTemp & ~selectKey | selectKeyPressed & (selectKeyTemp^(~selectKey)))
        {
            selectKeyPressed = true;
        }
        //if (cancelKey == 0)
        if (cancelKeyTemp & ~cancelKey | cancelKeyPressed & (cancelKeyTemp^(~cancelKey)))
        {
            cancelKeyPressed = true;
        }
    }

    // save temps.
    menuKeyTemp = ~menuKey;
    selectKeyTemp = ~selectKey;
    cancelKeyTemp = ~cancelKey;

    // led
    if (timerCount == 20)
    {

        led = ~led;
        timerCount = 0;
    }
    else
    {
        timerCount++;
    }

}

void initSys(void)
{
    
    LCD_Init();

    onLCD(0, 3, "Hello World !!!!");
    onLCD(1, 1, "Hello Again !!!!");

    initTimer();

    initTimer0();

}


void initTimer0(void)
{

    TMOD = 0x01;
    TL0 = TIMS;
    TH0 = TIMS >> 8;
    TR0 = 1;
    ET0 = 1;
    EA = 1;
}

void keyScan(void)
{

    if (sysInfo.menuInfo.onWhat != 0)
    {
        //if (menuKey == 0)
        if (menuKeyPressed)
        {
            sysInfo.menuInfo.confirm = true;

            menuKeyPressed = false;
        }

        //if (selectKey == 0)
        if (selectKeyPressed)
        {
            if (sysInfo.menuInfo.selected) // selected on something.
            {
                sysInfo.menuInfo.add = true;
                // when selected, select key works as add key.
            }
            else // not selected
            {

                switch (sysInfo.menuInfo.mode)
                {
                    case 0:

                        if (sysInfo.menuInfo.onWhat == sysInfo.menuInfo.timeMax)
                        {
                            sysInfo.menuInfo.onWhat = sysInfo.menuInfo.onMin;
                        }
                        else
                        {
                            sysInfo.menuInfo.onWhat++;
                        }

                        break;

                    case 1:

                        break;

                    case 2:

                        if (sysInfo.menuInfo.onWhat == sysInfo.menuInfo.alarmMax)
                        {
                            sysInfo.menuInfo.onWhat = sysInfo.menuInfo.onMin;
                        }
                        else
                        {
                            sysInfo.menuInfo.onWhat++;
                        }

                        break;

                }

            }

            selectKeyPressed = false;
        } // end of selectKey is pressed.

        //if (cancelKey == 0)
        if (cancelKeyPressed)
        {
            sysInfo.menuInfo.cancel = true;

            cancelKeyPressed = false;
        }
    }
    else // onWhat = 0
    {
        //if (menuKey == 0)
        if (menuKeyPressed)
        {
            if (sysInfo.menuInfo.mode == sysInfo.menuInfo.modeMax)
            {
                sysInfo.menuInfo.mode = sysInfo.menuInfo.modeMin;
            }
            else
            {
                sysInfo.menuInfo.mode++;
            }

            menuKeyPressed = false;
        }

        //if (selectKey == 0) // Pressed select key when on nothing.
        if (selectKeyPressed)
        {
            switch (sysInfo.menuInfo.mode)
            {
                case 0: // clock mode.
                    
                    if (sysInfo.menuInfo.onWhat==0)
                    {
                        sysInfo.newTime = sysInfo.now;
                    }

                    sysInfo.menuInfo.onWhat++;

                    break;

                case 1: // mode 1, counter mode.

                    if (!sysInfo.counterStarted) // not started
                    {
                        sysInfo.startTime = getTime();
                        sysInfo.counterStarted = true; // start!
                    }
                    else // already started, stop.
                    {
                        sysInfo.counterStarted = false;
                    }
                    break;

                case 2:

                    sysInfo.menuInfo.onWhat++;

                    break;
            }

            selectKeyPressed = false;
        }
    }
}


void contentControl(void)
{

    // After selected an item, the value of it can be changed.
    
    switch (sysInfo.menuInfo.mode)
    {
        case 0: // mode 0, clock mode.

            if (!sysInfo.menuInfo.selected) // not selected.
            {
                if (sysInfo.menuInfo.confirm)
                {
                    //sysInfo.newTime = sysInfo.now;
                    sysInfo.menuInfo.selected = true;

                    sysInfo.menuInfo.confirm = false;
                }

                if (sysInfo.menuInfo.cancel)
                {
                    sysInfo.menuInfo.onWhat = 0;

                    sysInfo.menuInfo.cancel= false;
                }

            }
            else // selected.
            {
                if (sysInfo.menuInfo.confirm)
                {

                    sysInfo.newTime.sec = sysInfo.now.sec;
                    sysInfo.newTime.day = dayInWeek();

                    setTime(sysInfo.newTime);

                    sysInfo.menuInfo.selected = false;
                    sysInfo.menuInfo.confirm = false;

                }

                if (sysInfo.menuInfo.cancel)
                { // anything else to do???
                    sysInfo.menuInfo.selected = false;
                    sysInfo.menuInfo.cancel = false;
                }

                switch (sysInfo.menuInfo.onWhat)
                {
                    case 0: break;

                    case 1: // year

                        if (sysInfo.menuInfo.add)
                        {
                            if (sysInfo.newTime.year > 98)
                            {
                                sysInfo.newTime.year = 0;
                            }
                            else
                            {
                                sysInfo.newTime.year++;
                            }

                            sysInfo.menuInfo.add = false;
                        }

                        break;

                    case 2 : // month

                        if (sysInfo.menuInfo.add)
                        {
                            if (sysInfo.newTime.month > 11)
                            {
                                sysInfo.newTime.year++;
                                sysInfo.newTime.month = 1;
                            }
                            else
                            {
                                sysInfo.newTime.month++;
                            }

                            sysInfo.menuInfo.add = false;
                        }

                        break;

                    case 3 : // date

                        if (sysInfo.menuInfo.add)
                        {
                            if (sysInfo.newTime.date > (maxDays() - 1) ) // !!! Feb
                            {
                                sysInfo.newTime.date = 1;

                                if (sysInfo.newTime.month<11)
                                {
                                    sysInfo.newTime.month++;
                                }
                                else
                                {
                                    sysInfo.newTime.year++;
                                    sysInfo.newTime.month = 1;
                                }
                            }
                            else
                            {
                                sysInfo.newTime.date++;
                            }

                            sysInfo.menuInfo.add = false;
                        }

                        break;

                    case 4 : // hour

                        if (sysInfo.menuInfo.add)
                        {
                            if (sysInfo.newTime.hour > 23)
                            {
                                sysInfo.newTime.hour = 0;
                            }
                            else
                            {
                                sysInfo.newTime.hour++;
                            }

                            sysInfo.menuInfo.add = false;
                        }

                        break;

                    case 5 : // min

                        if (sysInfo.menuInfo.add)
                        {
                            if (sysInfo.newTime.min > 59)
                            {
                                sysInfo.newTime.min = 0;
                            }
                            else
                            {
                                sysInfo.newTime.min++;
                            }

                            sysInfo.menuInfo.add = false;
                        }

                        break;

                }
            }

            break;

        case 1: // mode 1, counter mode.

            break;

        case 2: // mode 2, alarm mode.

            if (!sysInfo.menuInfo.selected) // not selected.
            {
                if (sysInfo.menuInfo.confirm)
                {
                    //sysInfo.newTime = sysInfo.now;
                    sysInfo.menuInfo.selected = true;

                    sysInfo.menuInfo.confirm = false;
                }

                if (sysInfo.menuInfo.cancel)
                {
                    sysInfo.menuInfo.onWhat = 0;

                    sysInfo.menuInfo.cancel= false;
                }

            }
            else // selected.
            {
                if (sysInfo.menuInfo.confirm)
                {

                    sysInfo.menuInfo.selected = false;
                    sysInfo.menuInfo.confirm = false;

                }

                if (sysInfo.menuInfo.cancel)
                { // anything else to do???
                    sysInfo.menuInfo.selected = false;
                    sysInfo.menuInfo.cancel = false;
                }

                switch (sysInfo.menuInfo.onWhat)
                {
                    case 0: break;

                    case 1: // ON or OFF?

                        if (sysInfo.menuInfo.add)
                        {

                            if (sysInfo.alarm) // alarm is set.
                            {
                                sysInfo.alarm = false;
                            }
                            else // alarm is cleared.
                            {
                                sysInfo.alarm = true;
                            }

                            sysInfo.menuInfo.add = false;

                        }

                        break;

                    case 2 : // hour

                        if (sysInfo.menuInfo.add)
                        {

                            if (sysInfo.alarmClock.hour > 23)
                            {
                                sysInfo.alarmClock.hour = 0;
                            }
                            else
                            {
                                sysInfo.alarmClock.hour++;
                            }

                            sysInfo.menuInfo.add = false;

                        }

                        break;

                    case 3 : // min

                        if (sysInfo.menuInfo.add)
                        {

                            if (sysInfo.alarmClock.min > 59)
                            {
                                sysInfo.alarmClock.min = 0;
                            }
                            else
                            {
                                sysInfo.alarmClock.min ++;
                            }

                            sysInfo.menuInfo.add = false;

                        }

                        break;

                }
            }

            break;
    }

}


void displayControl(void)
{

    switch (sysInfo.menuInfo.mode)
    {
        case 0: // time mode.

            clearLCD();
            timeOnLCD();

            break;

        case 1: // counter mode.

            clearLCD();
            counterOnLCD();

            break;

        case 2: // alarm mode.

            clearLCD();
            alarmOnLCD();

            break;
    }

}


void timeOnLCD(void)
{

    unsigned char timeLine = 1, clockLine = 0;
    unsigned char timeStart = 1, clockStart = 8;

    struct Time time, newTime;

    time = sysInfo.now;

    if (sysInfo.menuInfo.onWhat == 0)
    {
        // time line.
        onLCD(timeLine, timeStart, whichDay(time.day));

        timeStart++;
        timeStart++;
        timeStart++;
        timeStart++;
        timeStart++;

        onLCD(timeLine, timeStart, "20");

        timeStart++;
        timeStart++;

        onLCD(timeLine, timeStart, toString(time.year));

        timeStart++;
        timeStart++;

        onLCD(timeLine, timeStart++, "-");

        onLCD(timeLine, timeStart, toString(time.month));

        timeStart++;
        timeStart++;

        onLCD(timeLine, timeStart++, "-");

        onLCD(timeLine, timeStart, toString(time.date));

        timeStart++;
        timeStart++;

        onLCD(timeLine, timeStart, " ");

        // clock line.
        onLCD(clockLine, clockStart, toString(time.hour));

        clockStart++;
        clockStart++;

        onLCD(clockLine, clockStart++, ":");

        onLCD(clockLine, clockStart, toString(time.min));

        clockStart++;
        clockStart++;

        onLCD(clockLine, clockStart++, ":");

        onLCD(clockLine, clockStart, toString(time.sec));

        clockStart++;
        clockStart++;

        onLCD(clockLine, clockStart, " ");
    }
    else // onWhat != 0. setting mode.
    {

        newTime = sysInfo.newTime;
        
        if (!sysInfo.menuInfo.selected) // not selected. All display, only onWhat blinks.
        {

            // time line, second line.
            onLCD(timeLine, 6, "20"); // 6, 7
            onLCD(timeLine, 8, toString(newTime.year)); // 8, 9
            onLCD(timeLine, 10, "-"); // 10
            onLCD(timeLine, 11, toString(newTime.month)); // 11, 12
            onLCD(timeLine, 13, "-"); // 13
            onLCD(timeLine, 14, toString(newTime.date)); // 14, 15

            //clock line, first line.
            onLCD(clockLine, 8, toString(newTime.hour)); // 8, 9
            onLCD(clockLine, 10, ":"); // 10
            onLCD(clockLine, 11, toString(newTime.min)); // 11, 12
            onLCD(clockLine, 13, ":"); // 13
            onLCD(clockLine, 14, toString(time.sec)); // 14, 15


            if (sysInfo.menuInfo.blank) // blank is true.
            {
                sysInfo.menuInfo.blank = false;

                switch(sysInfo.menuInfo.onWhat)
                {
                    case 1: // year

                        onLCD(timeLine, 6, "    ");
                        break;

                    case 2: // month

                        onLCD(timeLine, 11, "  ");
                        break;

                    case 3: // date

                        onLCD(timeLine, 14, "  ");
                        break;

                    case 4: // hour

                        onLCD(clockLine, 8, "  ");
                        break;

                    case 5: // min

                        onLCD(clockLine, 11, "  ");
                        break;

                }
            }
            else // blank is false.
            {
                sysInfo.menuInfo.blank = true;
            }

        }
        else // selected. only display and blink the selected part.
        {

            clearLCD();

            if (sysInfo.menuInfo.blank) // blank is true.
            {
                sysInfo.menuInfo.blank = false;

                switch(sysInfo.menuInfo.onWhat)
                {
                    case 1: // year

                        onLCD(timeLine, 6, "    "); // 6, 7, 8, 9
                        onLCD(timeLine, 10, "-"); // 10
                        onLCD(timeLine, 11, toString(newTime.month)); // 11, 12
                        onLCD(timeLine, 13, " "); // 13
                        break;

                    case 2: // month

                        onLCD(timeLine, 11, "  ");
                        onLCD(timeLine, 13, "-"); // 13
                        onLCD(timeLine, 14, toString(newTime.date)); // 15, 16
                        onLCD(timeLine, 16, " "); // 16
                        break;

                    case 3: // date

                        onLCD(timeLine, 11, toString(newTime.month)); // 11, 12
                        onLCD(timeLine, 13, "-"); // 13
                        onLCD(timeLine, 14, "  ");
                        break;

                    case 4: // hour

                        onLCD(clockLine, 8, "  "); // 8, 9
                        onLCD(clockLine, 10, ":"); // 10
                        onLCD(clockLine, 11, toString(newTime.min)); // 11, 12
                        onLCD(clockLine, 13, "  ");
                        break;

                    case 5: // min

                        onLCD(clockLine, 8, toString(newTime.hour)); // 8, 9
                        onLCD(clockLine, 10, ":"); // 10
                        onLCD(clockLine, 11, "  "); // 11, 12
                        break;

                }
            }
            else // blank is false.
            {
                sysInfo.menuInfo.blank = true;

                switch(sysInfo.menuInfo.onWhat)
                {
                    case 1: // year

                        onLCD(timeLine, 6, "20");
                        onLCD(timeLine, 8, toString(newTime.year));
                        onLCD(timeLine, 10, "-"); // 10
                        onLCD(timeLine, 11, toString(newTime.month));
                        break;

                    case 2: // month

                        onLCD(timeLine, 11, toString(newTime.month));
                        onLCD(timeLine, 13, "-"); // 13
                        onLCD(timeLine, 14, toString(newTime.date));
                        break;

                    case 3: // date

                        onLCD(timeLine, 11, toString(newTime.month)); // 11, 12
                        onLCD(timeLine, 13, "-"); // 13
                        onLCD(timeLine, 14, toString(newTime.date));
                        break;

                    case 4: // hour

                        onLCD(clockLine, 8, toString(newTime.hour));
                        onLCD(clockLine, 10, ":"); // 10
                        onLCD(clockLine, 11, toString(newTime.min)); // 11, 12
                        break;

                    case 5: // min

                        onLCD(clockLine, 8, toString(newTime.hour)); // 8, 9
                        onLCD(clockLine, 10, ":"); // 10
                        onLCD(clockLine, 11, toString(newTime.min));
                        break;
                }

            }

        }

    }

} // end of timeOnLCD.


void counterOnLCD(void)
{

    unsigned char timeLine = 0, counterLine = 1;
    unsigned char timeStart = 0, counterStart = 5;

    xdata struct Time time;

    time = sysInfo.now;

    // time line.
    // date
    onLCD(timeLine, timeStart, toString(time.month)); // MM, 0, 1

    timeStart++;
    timeStart++;

    onLCD(timeLine, timeStart++, "-"); // MM-, 2

    onLCD(timeLine, timeStart, toString(time.date)); // MM-DD, 3, 4
    
    timeStart++;
    timeStart++;

    onLCD(timeLine, timeStart, "   "); // 5, 6, 7

    timeStart = 8;

    onLCD(timeLine, timeStart, toString(time.hour)); // HH

    timeStart++;
    timeStart++;

    onLCD(timeLine, timeStart++, ":"); // HH:

    onLCD(timeLine, timeStart, toString(time.min)); // HH:MM

    timeStart++;
    timeStart++;

    onLCD(timeLine, timeStart++, ":"); // HH:MM:

    onLCD(timeLine, timeStart, toString(time.sec)); // HH:MM:SS

    timeStart++;
    timeStart++;

    onLCD(timeLine, timeStart, " "); // 15, the end of the line.

    // counter line.
    if (sysInfo.counterStarted) // counter started.
    {
        struct Time startTime;
        signed char pastDay, pastHour, pastMin, pastSec;

        startTime = sysInfo.startTime;

        pastDay = time.date - startTime.date;
        pastHour = time.hour - startTime.hour;
        pastMin = time.min - startTime.min;
        pastSec = time.sec - startTime.sec;

        if (pastDay < 0)
        {
            pastDay += 30;
        }

        if (pastHour < 0)
        {
            pastHour += 24;
            pastDay--;
        }

        if (pastMin < 0)
        {
            pastMin += 60;
            pastHour--;
        }

        if (pastSec < 0)
        {
            pastSec += 60;
            pastMin--; // take one minute to do the sub action.
        }


        onLCD(counterLine, counterStart, toString(pastDay)); // 5,6

        counterStart++;
        counterStart++;

        onLCD(counterLine, counterStart++, ":"); // 7

        onLCD(counterLine, counterStart, toString(pastHour)); // 8,9

        counterStart++;
        counterStart++;

        onLCD(counterLine, counterStart++, ":"); // 10

        onLCD(counterLine, counterStart, toString(pastMin)); // 11,12

        counterStart++;
        counterStart++;

        onLCD(counterLine, counterStart++, ":"); // 13

        onLCD(counterLine, counterStart, toString(pastSec)); // 14, 15

        counterStart++;
        counterStart++;

    } // end of started clock.
    else // counter is not started yet.
    {
        onLCD(counterLine, counterStart, "DD"); // 5,6

        counterStart++;
        counterStart++;

        onLCD(counterLine, counterStart++, ":"); // 7

        onLCD(counterLine, counterStart, "HH"); // 8,9

        counterStart++;
        counterStart++;

        onLCD(counterLine, counterStart++, ":"); // 10

        onLCD(counterLine, counterStart, "MM"); // 11,12

        counterStart++;
        counterStart++;

        onLCD(counterLine, counterStart++, ":"); // 13

        onLCD(counterLine, counterStart, "SS"); // 14, 15

        counterStart++;
        counterStart++;

    } // end of not started clock.

} // end of timeOnLCD.


void alarmOnLCD(void)
{
    unsigned char hour, min;

    hour = sysInfo.alarmClock.hour;
    min = sysInfo.alarmClock.min;

    if (sysInfo.menuInfo.onWhat == 0)
    {

        // first line.
        onLCD(0, 0, "Alarm"); // 0-4.
        onLCD(0, 11, toString(hour)); // 11, 12
        onLCD(0, 13, ":");
        onLCD(0, 14, toString(min));

        // second line.
        if (sysInfo.alarm) // alarm is on.
        {
            onLCD(1, 1, "ON");
        }
        else
        {
            onLCD(1, 1, "OFF");
        }

    }
    else // onWhat != 0. setting mode.
    {

        if (!sysInfo.menuInfo.selected) // not seleted, only onWhat blinks.
        {

            // first line.
            onLCD(0, 0, "Alarm"); // 0-4.
            onLCD(0, 11, toString(hour)); // 11, 12
            onLCD(0, 13, ":");
            onLCD(0, 14, toString(min));

            // second line.
            if (sysInfo.alarm) // alarm is on.
            {
                onLCD(1, 1, "ON");
            }
            else
            {
                onLCD(1, 1, "OFF");
            }

            if (sysInfo.menuInfo.blank) // display blank.
            {
                sysInfo.menuInfo.blank = false;

                switch (sysInfo.menuInfo.onWhat)
                {

                    case 1:

                        onLCD(1, 1, "   "); // ON
                        break;

                    case 2:

                        onLCD(0, 11, "  "); // HH
                        onLCD(0, 13, ":");
                        onLCD(0, 14, toString(min));
                        break;

                    case 3:

                        onLCD(0, 11, toString(hour)); // 11, 12
                        onLCD(0, 13, ":");
                        onLCD(0, 14, "  "); // MM
                        break;

                }
            }
            else // display the content.
            {
                sysInfo.menuInfo.blank = true;

                switch (sysInfo.menuInfo.onWhat)
                {

                    case 1:

                        if (sysInfo.alarm) // alarm is on.
                        {
                            onLCD(1, 1, "ON");
                        }
                        else
                        {
                            onLCD(1, 1, "OFF");
                        }

                        break;

                    case 2:

                        onLCD(0, 11, toString(hour)); // 11, 12
                        onLCD(0, 13, ":");
                        onLCD(0, 14, toString(min));
                        break;

                    case 3:

                        onLCD(0, 11, toString(hour)); // 11, 12
                        onLCD(0, 13, ":");
                        onLCD(0, 14, toString(min)); // 11, 12
                        break;

                }

            }

        } // end of not selected.
        else // selected on something.
        {

            clearLCD();

            if (sysInfo.menuInfo.blank) // blank is true.
            {

                sysInfo.menuInfo.blank = false;

                switch (sysInfo.menuInfo.onWhat)
                {

                    case 1:

                        onLCD(1, 1, "   ");
                        break;

                    case 2:

                        onLCD(0, 11, "  ");
                        break;

                    case 3:

                        onLCD(0, 14, "  ");
                        break;
                }

            }
            else // blank is false.
            {

                sysInfo.menuInfo.blank = true;

                switch (sysInfo.menuInfo.onWhat)
                {

                    case 1:

                        if (sysInfo.alarm) // alarm is on.
                        {
                            onLCD(1, 1, "ON");
                        }
                        else
                        {
                            onLCD(1, 1, "OFF");
                        }

                        break;

                    case 2:

                        onLCD(0, 11, toString(hour)); // 11, 12
                        onLCD(0, 13, " "); // 11, 12
                        break;

                    case 3:

                        onLCD(0, 14, toString(min)); // 11, 12
                        break;

                }

            }
        }

    } // end of on something, setting mode.

}


unsigned char maxDays(void)
{
    unsigned char year, month;

    year = sysInfo.newTime.year;
    month = sysInfo.newTime.month;

    switch(month)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:

            return (unsigned char) 31;
            break;

        case 4:
        case 6:
        case 9:
        case 11:

            return (unsigned char) 30;
            break;

        case 2:

            if ((year%4==0) && (year%100!=0))
            {
                return (unsigned char) 29;
            }
            else
            {
                return (unsigned char) 28;
            }

            break;

        default:

            return 30; // !!! to avoid warnning.
            break;
    }
}


unsigned char dayInWeek(void)
{
    int y, month, date;

    y = sysInfo.newTime.year + 2000;
    month = sysInfo.newTime.month;
    date = sysInfo.newTime.date;

    if (month==1||month==2)
    {
        month += 12;
        y--;
    }

    return (unsigned char)(((date+2*month+3*(month+1)/5+y+y/4-y/100+y/400)%7) + 1 );

}


unsigned char * toString(unsigned char number)
{

    unsigned char string[2];

    if (number < 10)
    {
        string[0] = '0';
        string[1] = number + 0x30;
    }
    else
    {
        string[0] = number/10 + 0x30;
        string[1] = number%10 + 0x30;
    }

    return string;
} // end of toString.


unsigned char * whichDay(unsigned char dayInDight)
{

    unsigned char * string ;

    switch (dayInDight)
    {
        case 1: string = "Mon. "; break;
        case 2: string = "Tue. "; break;
        case 3: string = "Wed. "; break;
        case 4: string = "Thus."; break;
        case 5: string = "Fri. "; break;
        case 6: string = "Sat. "; break;
        case 7: string = "Sun. "; break;
        default : string = "     "; break;
    }

    return string;

} // end of whichDay.


struct SystemInfo getSysInfo(void)
{
    struct SystemInfo sysInfo;

    sysInfo.menuInfo = getMenuInfo();

    sysInfo.counterStarted = false;
    sysInfo.alarm = false;

    return sysInfo;
}


struct MenuInfo getMenuInfo(void)
{
    struct MenuInfo menuInfo;

    menuInfo.modeMin = 0;
    menuInfo.modeMax = 2;

    menuInfo.onMin = 0;
    menuInfo.alarmMax = 3; // onWhat in alarm mode.
    menuInfo.timeMax = 5; // onWhat in time mode.

    menuInfo.mode = 0;
    menuInfo.onWhat = 0;

    menuInfo.confirm = false; // key1
    menuInfo.add = false; // key2
    menuInfo.cancel = false; // key3

    //menuInfo.blink = false;
    menuInfo.blank = false;
    menuInfo.selected = false;

    return menuInfo;
}


