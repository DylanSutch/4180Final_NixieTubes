/**********************************************************************
* 
* ECE 4180 Final Project - Nixie Tube Clock with DS3231 and Bluetooth Control
* Edited from the original demo by Benjamin Ventimiglia
*
***********************************************************************
* Copyright (C) 2015 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
**********************************************************************/

#include "ds3231.h"

#define ESC 0x1B

#define BLUETOOTH

Serial bluemod(p28,p27);

void get_user_input(char* message, uint8_t min, uint8_t max, uint32_t* member);
void get_user_input(char* message, uint8_t min, uint8_t max, bool* member);
void get_bt_user_input(char* message, uint8_t min, uint8_t max, uint32_t* member);
void get_bt_user_input(char* message, uint8_t min, uint8_t max, bool* member);

int main(void)
{
    //rtc object
    Ds3231 rtc(p9, p10); 
    
    time_t epoch_time;
    
    //DS3231 rtc variables
    
    //default, use bit masks in ds3231.h for desired operation
    ds3231_cntl_stat_t rtc_control_status = {0,0}; 
    ds3231_time_t rtc_time;
    ds3231_calendar_t rtc_calendar;
    
    rtc.set_cntl_stat_reg(rtc_control_status);
    
#ifndef BLUETOOTH
    //get day from user
    get_user_input("\nPlease enter day of week, 1 for Sunday (1-7): ", 1,
                    7, &rtc_calendar.day);

    //get day of month from user
    get_user_input("\nPlease enter day of month (1-31): ", 1, 31, 
                    &rtc_calendar.date);

    //get month from user
    get_user_input("\nPlease enter the month, 1 for January (1-12): ", 1, 
                    12, &rtc_calendar.month);

    //get year from user
    get_user_input("\nPlease enter the year (0-99): ",0, 99, 
                    &rtc_calendar.year);
      
    //Get time mode
    get_user_input("\nWhat time mode? 1 for 12hr 0 for 24hr: ", 0, 1, 
                   &rtc_time.mode);  
    
    if(rtc_time.mode)
    {
        //Get AM/PM status
        get_user_input("\nIs it AM or PM? 0 for AM 1 for PM: ", 0, 1, 
                       &rtc_time.am_pm);  
        //Get hour from user
        get_user_input("\nPlease enter the hour (1-12): ", 1, 12, 
                       &rtc_time.hours);
    }
    else
    {
        //Get hour from user
        get_user_input("\nPlease enter the hour (0-23): ", 0, 23, 
                       &rtc_time.hours);
    }
     
    //Get minutes from user
    get_user_input("\nPlease enter the minute (0-59): ", 0, 59, 
                   &rtc_time.minutes);
    
    
    //Get seconds from user
    get_user_input("\nPlease enter the second (0-59): ", 0, 59,     
                   &rtc_time.seconds);

    // If BLUETOOTH is defined, use bt inputs
#else
    //get day from user
    get_bt_user_input("\nPlease enter day of week, 1 for Sunday (1-7): ", 1,
                    7, &rtc_calendar.day);

    //get day of month from user
    get_bt_user_input("\nPlease enter day of month (1-31): ", 1, 31, 
                    &rtc_calendar.date);

    //get month from user
    get_bt_user_input("\nPlease enter the month, 1 for January (1-12): ", 1, 
                    12, &rtc_calendar.month);

    //get year from user
    get_bt_user_input("\nPlease enter the year (0-99): ",0, 99, 
                    &rtc_calendar.year);
      
    //Get time mode
    get_bt_user_input("\nWhat time mode? 1 for 12hr 0 for 24hr: ", 0, 1, 
                   &rtc_time.mode);  
    
    if(rtc_time.mode)
    {
        //Get AM/PM status
        get_bt_user_input("\nIs it AM or PM? 0 for AM 1 for PM: ", 0, 1, 
                       &rtc_time.am_pm);  
        //Get hour from user
        get_bt_user_input("\nPlease enter the hour (1-12): ", 1, 12, 
                       &rtc_time.hours);
    }
    else
    {
        //Get hour from user
        get_bt_user_input("\nPlease enter the hour (0-23): ", 0, 23, 
                       &rtc_time.hours);
    }
     
    //Get minutes from user
    get_bt_user_input("\nPlease enter the minute (0-59): ", 0, 59, 
                   &rtc_time.minutes);
    
    
    //Get seconds from user
    get_bt_user_input("\nPlease enter the second (0-59): ", 0, 59, 
                   &rtc_time.seconds);
#endif
    
    
    //Set the time, uses inverted logic for return value
    if(rtc.set_time(rtc_time))
    {
        printf("\nrtc.set_time failed!!\n");
        exit(0);
    }
    
    //Set the calendar, uses inverted logic for return value
    if(rtc.set_calendar(rtc_calendar))
    {
        printf("\nrtc.set_calendar failed!!\n");
        exit(0);
    }
    
    char buffer[32];
    
    for(;;)
    {   
        printf("%c[2J", ESC); //clear screen
        printf("%c[H", ESC); //move cursor to Home
        
        //new epoch time fx
        epoch_time = rtc.get_epoch();
        
        printf("\nTime as seconds since January 1, 1970 = %d\n", epoch_time);
        
        printf("\nTime as a basic string = %s", ctime(&epoch_time));
 
        strftime(buffer, 32, "%I:%M %p\n", localtime(&epoch_time));
        printf("\nTime as a custom formatted string = %s", buffer);
        
        wait(1.0);
    }//loop 
}


/**********************************************************************
* Function: get_user_input() / get_bt_user_input()
* Parameters: message - user prompt
*             min - minimum value of input
*             max - maximum value of input
*             member - pointer to struct member              
* Returns: none
*
* Description: get time/date input from user from either 
*              serial terminal or bluetooth connection
*
**********************************************************************/
void get_user_input(char* message, uint8_t min, uint8_t max, uint32_t* member)
{
    uint32_t temp;

    do
    {
        printf("\n%s", message);
        
        //for some reason mbed doesn't like a pointer to a member in scanf
        //term.scanf("%d", member); works with gcc on RPi
        scanf("%d", &temp);
        
        *member = temp;
       
        if((*(member)< min) || (*(member) > max))
        {
            printf("\nERROR-RTI");
        }
    }
    while((*(member) < min) || (*(member) > max));
}


void get_user_input(char* message, uint8_t min, uint8_t max, bool* member)
{
    uint32_t temp;

    do
    {
        printf("\n%s", message);
        
        //for some reason mbed doesn't like a pointer to a member in scanf
        //term.scanf("%d", member); works with gcc on RPi
        scanf("%d", &temp);
        
        *member = temp;
       
        if((*(member)< min) || (*(member) > max))
        {
            printf("\nERROR-RTI");
        }
    }
    while((*(member) < min) || (*(member) > max));
}

void get_bt_user_input(char* message, uint8_t min, uint8_t max, uint32_t* member)
{
    uint32_t temp;

    do
    {
        bluemod.printf("\n%s", message);
        
        //for some reason mbed doesn't like a pointer to a member in scanf
        //term.scanf("%d", member); works with gcc on RPi
        bluemod.scanf("%d", &temp);
        
        *member = temp;
       
        if((*(member)< min) || (*(member) > max))
        {
            bluemod.printf("\nERROR-RTI");
        }
    }
    while((*(member) < min) || (*(member) > max));
    
    
   /* while(1) {
        if (bluemod.readable()) {
            if (bluemod.getc()=='!') {
                if (bluemod.getc()=='C') { //color data packet
                    bred = bluemod.getc(); // RGB color values
                    bgreen = bluemod.getc();
                    bblue = bluemod.getc();
                    if (bluemod.getc()==char(~('!' + 'C' + bred + bgreen + bblue))) { //checksum OK?
                        RGBLED_r = bred/255.0; //send new color to RGB LED PWM outputs
                        RGBLED_g = bgreen/255.0;
                        RGBLED_b = bblue/255.0;
                    }
                break;
                }
            }
        }
    }*/
}

void get_bt_user_input(char* message, uint8_t min, uint8_t max, bool* member)
{
    uint32_t temp;

    do
    {
        bluemod.printf("\n%s", message);
        
        //for some reason mbed doesn't like a pointer to a member in scanf
        //term.scanf("%d", member); works with gcc on RPi
        bluemod.scanf("%d", &temp);
        
        *member = temp;
       
        if((*(member)< min) || (*(member) > max))
        {
            bluemod.printf("\nERROR-RTI");
        }
    }
    while((*(member) < min) || (*(member) > max));
}    