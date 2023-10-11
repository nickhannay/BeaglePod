/**
 * @file lcd_4line.c
 * @brief This is a source file for the lcd_4line module.
 *
 * This source file contains the definition of the functions
 * for the lcd_4line module, which provides the utilities
 * for interacting with the BeablePod mp3 player
 *
 * @author Nick Hannay
 * @date 2023-03-25
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>

#include "lcd_4line.h"
#include "sleep.h"

#define I2C_BUS "/dev/i2c-1"
#define LCD_ADDR 0x27

// GLOBALS
static int i2cFd;

// Private Function Declarations
static void setLineNum(LCD_LINE_NUM line_num);
static void I2C_sendByte(unsigned char data);
static void I2C_configPins(void);
static void I2C_configBus(void);
static int runCommand(char *command);

//------------------------------------------------
//////////////// Public Functions ////////////////
//------------------------------------------------

void LCD_init(void)
{

    I2C_configPins();
    I2C_configBus();

    /* -------------------------------------------------------------------- *
     * Initialize the display, using the 4-bit mode initialization sequence *
     * -------------------------------------------------------------------- */
    I2C_sendByte(0b00110100);
    I2C_sendByte(0b00110000);
    Sleep_ns(0, 4100000); // wait 4.1msec
    I2C_sendByte(0b00110100);
    I2C_sendByte(0b00110000);
    Sleep_ns(0, 100000);      // wait 100usec
    I2C_sendByte(0b00110100); //
    I2C_sendByte(0b00110000);
    Sleep_ns(0, 4100000); // wait 4.1msec
    I2C_sendByte(0b00100100);
    I2C_sendByte(0b00100000); // 4 bit mode

    /* -------------------------------------------------------------------- *
     * Set 4-bt, 2 Line, 5x8 char mode                                      *
     * -------------------------------------------------------------------- */
    Sleep_ns(0, 40000); // wait 40usec
    I2C_sendByte(0b00100100);
    I2C_sendByte(0b00100000); // keep 4-bit mode
    I2C_sendByte(0b10000100);
    I2C_sendByte(0b10000000); // D3=2 lines, D2=char5x8

    LCD_setCursorDirection();

    LCD_turnOnDisplay();

    LCD_clear();
}

void LCD_cleanup(void)
{
    LCD_clear();
    LCD_writeStringAtLine("Shutting Down...", LCD_LINE1);
    Sleep_ms(1000);
    LCD_clear();
    close(i2cFd);
}

void LCD_writeStringAtLine(char *string, LCD_LINE_NUM line_num)
{
    setLineNum(line_num);

    LCD_writeString(string);
}

void LCD_shiftDisplay(LCD_LINE_NUM line, LCD_DIRECTION dir)
{
    switch (dir)
    {
    case LCD_LEFT:
        I2C_sendByte(0b00010100);
        I2C_sendByte(0b00000000);
        I2C_sendByte(0b10000100);
        I2C_sendByte(0b00000000);
        break;
    case LCD_RIGHT:
        I2C_sendByte(0b00010100);
        I2C_sendByte(0b00000000);
        I2C_sendByte(0b11000100);
        I2C_sendByte(0b00000000);
        break;
    default:
        // invalid direction
        break;
    }
}

void LCD_writeChar(unsigned char character)
{
    unsigned char full = 0x00;
    unsigned char high = character & 0xF0;
    unsigned char low = (character & 0x0F) << 4;
    full = full | high;
    full = full | 0x05;

    I2C_sendByte(full);
    // printf("sent: %0x\n", full);
    I2C_sendByte(0b00000000);

    full = 0x00;
    full = full | low;
    full = full | 0x05;

    I2C_sendByte(full);
    // printf("sent: %0x\n", full);
    I2C_sendByte(0b00000000);
}

void LCD_clear(void)
{
    I2C_sendByte(0b00000100);
    I2C_sendByte(0b00000000);
    I2C_sendByte(0b00010100);
    I2C_sendByte(0b00000000);
}

void LCD_clearLine(LCD_LINE_NUM line)
{
    // clear
    LCD_writeStringAtLine("                    ", line);

    // rest cursor to beginning of line
    LCD_writeStringAtLine("", line);
}

void LCD_turnOnDisplay(void)
{
    Sleep_ns(0, 40000); // wait 40usec
    I2C_sendByte(0b00000100);
    I2C_sendByte(0b00000000);
    I2C_sendByte(0b11000100);
    I2C_sendByte(0b11000000);
}

void LCD_setCursorDirection(void)
{
    I2C_sendByte(0b00000100);
    I2C_sendByte(0b00000000);
    I2C_sendByte(0b01100100);
    I2C_sendByte(0b01100000); // print left to right
}

void LCD_writeString(char *string)
{
    // add ... to long strings
    if (strlen(string) > 20)
    {
        for (int i = 0; i < 16; i++)
        {
            LCD_writeChar(*string);
            string++;
        }
        for (int i = 0; i < 3; i++)
        {
            LCD_writeChar('.');
        }
    }
    else
    {
        while (*string != '\0')
        {
            LCD_writeChar(*string);
            string++;
        }
    }
}

//------------------------------------------------
//////////////// Private Functions ////////////////
//------------------------------------------------

static void I2C_configPins(void)
{
    // configure I2C1 SDA pin
    while (runCommand("config-pin P9_18 i2c") != 0)
    {
        printf("Attempting to reconfigure P9_18\n");
        Sleep_ms(10);
    }

    // configure I2C1 SDA pin
    while (runCommand("config-pin P9_17 i2c") != 0)
    {
        printf("Attempting to reconfigure P9_17\n");
        Sleep_ms(10);
    }
}

static void I2C_configBus(void)
{
    if ((i2cFd = open(I2C_BUS, O_RDWR)) < 0)
    {
        printf("Error failed to open I2C bus [%s].\n", I2C_BUS);
        exit(-1);
    }
    // set the I2C slave address for all subsequent I2C device transfers
    if (ioctl(i2cFd, I2C_SLAVE, LCD_ADDR) < 0)
    {
        printf("Error failed to set I2C address [%0x].\n", LCD_ADDR);
        exit(-1);
    }
}

static void I2C_sendByte(unsigned char data)
{
    unsigned char byte[1];
    byte[0] = data;
    write(i2cFd, byte, sizeof(byte));
    /* -------------------------------------------------------------------- *
     * Below wait creates 1msec delay, needed by display to catch commands  *
     * -------------------------------------------------------------------- */
    Sleep_ns(0, 1000000);
}

static int runCommand(char *command)
{
    FILE *pipe = popen(command, "r");

    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe))
    {
        if (fgets(buffer, sizeof(buffer), pipe))
        {
            break;
        }
    }

    int exit_code = WEXITSTATUS(pclose(pipe));
    if (exit_code != 0)
    {
        return (-1);
    }

    return (0);
}

static void setLineNum(LCD_LINE_NUM line_num)
{

    switch (line_num)
    {
    case LCD_LINE1:
        // 0x00 : 000 0000
        I2C_sendByte(0b10000100);
        I2C_sendByte(0b00000000);
        I2C_sendByte(0b00000100);
        I2C_sendByte(0b00000000);
        break;
    case LCD_LINE2:
        // 0x40 : 100 0000
        I2C_sendByte(0b11000100);
        I2C_sendByte(0b00000000);
        I2C_sendByte(0b00000100);
        I2C_sendByte(0b00000000);
        break;
    case LCD_LINE3:
        // 0x14 : 001 0100
        I2C_sendByte(0b10010100);
        I2C_sendByte(0b00000000);
        I2C_sendByte(0b01000100);
        I2C_sendByte(0b00000000);
        break;
    case LCD_LINE4:
        // 0x54  : 101 0100
        I2C_sendByte(0b11010100);
        I2C_sendByte(0b00000000);
        I2C_sendByte(0b01000100);
        I2C_sendByte(0b00000000);
        break;
    default:
        // invalid line
        printf("LCD_setLineNum : invalid line num\n");
        break;
    }
}