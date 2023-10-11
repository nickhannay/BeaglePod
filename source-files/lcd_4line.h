/**
 * @file lcd_4line.h
 * @brief This is a header file for the lcd_4line module.
 *
 * This header file contains the declaration of the functions
 * for the lcd_4line module, which provides the utilities
 * for interacting with the BeablePod mp3 player
 *
 * @author Nick Hannay
 * @date 2023-03-25
 */

#ifndef LCD_H
#define LCD_H

/**
 * I2C I/O expander datasheet: https://www.mouser.ca/datasheet/2/302/PCF8574_PCF8574A-1127673.pdf
 * 2004A LCD module datasheet: https://image.dfrobot.com/image/data/DFR0154/LCD2004%20hd44780%20Datasheet.pdf
 *
 * WIRING INSTRUCTIONS:
 *  LCD    -     BeagleBone
 *  GND    -     P9_02
 *  VDD    -     P9_03
 *  SDA    -     P9_18
 *  SCL    -     P9_17
 *
 *
 *
 * NOTES:
 *  - cursor will wrap around entire screen if continuosly
 *    incremented
 *
 *  - logical layout of lines is:
 *
 *    line 1 = physical line 1 + line 3  (20 bytes + 20 bytes)
 *    line 2 = physical line 2 + line 4  (20 bytes + 20 bytes)
 *
 *   This means that when starting at the first position of line 1
 *   and continuosly incrementing, the cursor will travel (physical lines shown):
 *
 *                  line 1 -> line 3 -> line 2 -> line 4
 *
 *
 * Byte Layout:
 *              b8 - b5 = data
 *              b4 - b0 = (ignored) E R/W R/S
 *
 * example writing 'H':
 * 'H' char code = 0x48 = 01001000
 * using 4 bit mode, the data must be split into nibbles
 *
 * 1) Send the high nibble first (0100):
 *    0100 0101
 *    E = 1 (Initial pulse), R/W = 0 (Write operation), R/S = 1 (Data Operation)
 *
 * 2) send a low pulse (data is read into internal registers on low pulse):
 *       0000              0000
 *    (data bits   only E matters -> set low
 *     ingnored)
 *
 * 3) Send low nibble (1000):
 *    1000 0101
 *    E = 1 (Initial Pulse), R/W = 0 (Write operation), R/S = 1 (Data Operation)
 *
 * 4) send low pulse:
 *    0000 0000
 *
 *
 * DDRAM address for each line"
 *  1 = 0x00
 *  2 = 0x40
 *  3 = 0x14
 *  4 = 0x54
 */

#define LCD_RIGHT_ARROW 0x7E

/**
 * Used to specify the line number
 * to print to
 */
typedef enum
{
    LCD_LINE1,
    LCD_LINE2,
    LCD_LINE3,
    LCD_LINE4,
    NUM_LINES
} LCD_LINE_NUM;

/**
 * Used to change direction
 * of cursor/display
 */
typedef enum
{
    LCD_LEFT,
    LCD_RIGHT,
    NUM_DIRECTIONS
} LCD_DIRECTION;

/**
 * initializes the LCD display to
 * 4-bit, 2 line, 5x8 character mode
 */
void LCD_init(void);

/**
 * Configures the display mode
 */
void LCD_turnOnDisplay(void);

/**
 * closes I2C file descriptor
 */
void LCD_cleanup(void);

/**
 * Sets the cursor direction to RIGHT
 */
void LCD_setCursorDirection(void);

/**
 * Shifts the contents of a line
 *
 * LCD_LINE_NUM: LCD_LINE1, LCD_LINE2, LCD_LINE3, LCD_LINE4
 * LCD_DIRECTION: LCD_LEFT, LCD_RIGHT
 *
 * @param line The LCD_LINE_NUM of the line to be shifted
 * @param dir The LCD_DIRECTION to shift
 */
void LCD_shiftDisplay(LCD_LINE_NUM line, LCD_DIRECTION dir);

/**
 * Clears the specified line number and
 * returns the sets the cursor to the beginning of the line
 *
 * @param line The line number to be cleared
 */
void LCD_clearLine(LCD_LINE_NUM line);

/**
 * Writes a string to the specified line of the LCD screen
 *
 * @param string The string to be output to the LCD screen
 * @param line_num The enum specifying which line to write to [LCD_LINE1, LCD_LINE2, LCD_LINE2, LCD_LINE4]
 */
void LCD_writeStringAtLine(char *string, LCD_LINE_NUM line_num);

/**
 * Writes a string, starting at the current cursor position,
 * to the LCD screen
 *
 * @param string The string to be output to the LCD screen
 */
void LCD_writeString(char *string);

/**
 * Writes a character to the current cursor position (DDRAM ADDR)
 * and increments the cursor (DDRAM ADDR)
 *
 * @param character The character to be written to screen
 */
void LCD_writeChar(unsigned char character);

/**
 * Clears Screen and returns cursor
 * to start
 */
void LCD_clear(void);

#endif