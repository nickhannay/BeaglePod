/**
 * @file joystick.c
 * @brief This is a source file for the Joystick module.
 *
 * This source file contains the declaration of the functions
 * for the Joystick module, which provides the utilities
 * for setting up joystick and handling the joystick
 * movement when pressed.
 *
 * @author Mehdi Esmaeilzadeh
 * @date 2023-03-13
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <alsa/asoundlib.h>

#include "joystick.h"
#include "gpio.h"

static void assertDirectionOk(enum eJoystickDirections direction);
static void assertDirectionOk(enum eJoystickDirections direction);

struct DirectionInfo
{
    const char *name;
    const int portNumber;
};

// Data on all different directions which we support
const struct DirectionInfo directions[] = {
    {
        "Up",
        26,
    },
    {
        "Down",
        46,
    },
    {
        "Left",
        65,
    },
    {
        "Right",
        47,
    },
    {
        "Center",
        27,
    },
};

//------------------------------------------------
//////////////// Public Functions ////////////////
//------------------------------------------------

void Joystick_init()
{
    for (int i = 0; i < (JOYSTICK_MAX_NUMBER_DIRECTIONS - 1); i++)
    {
        // printf("... todo: actually open file for GPIO #%d\n", directions[i].portNumber);
        GPIO_SetPinDirection(directions[i].portNumber, "in");
    }
}

void Joystick_cleanup()
{
    // Nothing to clean up
    return;
}

bool Joystick_isDirectionPressed(enum eJoystickDirections direction)
{
    assertDirectionOk(direction);

    int length = strlen(GPIO_FILE_VALUE) + 1;
    char filepath[length];
    sprintf(filepath, GPIO_FILE_VALUE, directions[direction].portNumber);

    FILE *pFile = fopen(filepath, "r");
    if (pFile == NULL)
    {
        printf("ERROR: Unable to open file (%s) for read\n", filepath);
        exit(-1);
    }
    // Read string (line)
    const int MAX_LENGTH = 1024;
    char buff[MAX_LENGTH];
    fgets(buff, MAX_LENGTH, pFile);
    fclose(pFile);
    if (buff[0] == '1')
    {
        return false;
    }
    else
    {
        return true;
    }
}

enum eJoystickDirections Joystick_process_direction()
{
    if (Joystick_isDirectionPressed(JOYSTICK_UP))
    {
        return JOYSTICK_UP;
    }
    else if (Joystick_isDirectionPressed(JOYSTICK_DOWN))
    {
        return JOYSTICK_DOWN;
    }
    else if (Joystick_isDirectionPressed(JOYSTICK_RIGHT))
    {
        return JOYSTICK_RIGHT;
    }
    else if (Joystick_isDirectionPressed(JOYSTICK_LEFT))
    {
        return JOYSTICK_LEFT;
    }
    else if (Joystick_isDirectionPressed(JOYSTICK_CENTER))
    {
        return JOYSTICK_CENTER;
    }
    else
    {
        return JOYSTICK_NONE;
    }
}

const char *Joystick_getDirectionName(enum eJoystickDirections direction)
{
    assertDirectionOk(direction);

    return directions[direction].name;
}

//------------------------------------------------
/////////////// Private Functions ////////////////
//------------------------------------------------

static void assertDirectionOk(enum eJoystickDirections direction)
{
    assert(direction >= 0 && direction < (JOYSTICK_MAX_NUMBER_DIRECTIONS - 1));
}