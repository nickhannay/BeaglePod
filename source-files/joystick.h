/**
 * @file joystick.c
 * @brief This is a source file for the Joystick module.
 *
 * This source file contains the definition of the functions
 * for the Joystick module, which provides the utilities
 * for setting up joystick and handling the joystick
 * movement when pressed.
 *
 * @author Mehdi Esmaeilzadeh
 * @date 2023-03-13
 */

#ifndef _JOYSTICK_H
#define _JOYSTICK_H

#define GPIO_FILE_DIRECTION "/sys/class/gpio/gpio%d/direction"
#define GPIO_FILE_VALUE "/sys/class/gpio/gpio%d/value"

enum eJoystickDirections
{
    JOYSTICK_UP = 0,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_CENTER,
    JOYSTICK_NONE,
    JOYSTICK_MAX_NUMBER_DIRECTIONS // Get the number of directions via the enum
};

// Initialize the Joystick module
void Joystick_init();
// Remove any dynamic memory or open file
void Joystick_cleanup();
// Returns the direction hit by the user
enum eJoystickDirections Joystick_process_direction();
// Is the given direction pressed by the user
bool Joystick_isDirectionPressed(enum eJoystickDirections direction);
// Returns "Up", "Down", "Left", "Right" as a char *
const char *Joystick_getDirectionName(enum eJoystickDirections direction);

#endif // _JOYSTICK_H