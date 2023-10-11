/**
 * @file gpio.c
 * @brief This is a source file for the gpio module.
 *
 * This source file contains the declare of the functions
 * for the gpio module, which provides the utilities
 * for enabling and configuring the GPIO pins of the BeagleBone Green.
 *
 * @author Amirhossein Etaati
 * @date 2023-03-07
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "gpio.h"
#include "sleep.h"

// To check if the module functions' caller has called init first
static bool is_module_initialized = false;

// Location of the file to export pins
#define EXPORT_PATH "/sys/class/gpio/export"

// Maximum length of a file path
// Used for exporting pins and setting their direction and value
#define MAX_PATH_LEN 1024

// Pin information
#define DIGIT_GPIO_PATH "/sys/class/gpio/gpio"
#define DIR_PATH "/direction"
#define VAL_PATH "/value"

// Private functions definitions
static void set_pin_direction(const int *pins, int pin_size, const char *pin_direction);
static void export_pins(const int *pins, int pin_size);
static void enable_buses(const char **buses, int bus_size);
static void run_command(const char *command);

//------------------------------------------------
//////////////// Public Functions ////////////////
//------------------------------------------------

void GPIO_init(const char **buses_config_commands, int bus_size, const int *pins, int pin_size, char *pin_dir)
{
    // handle errors
    if (buses_config_commands != NULL && bus_size != 0)
    {
        // enable linux support for the required buses
        enable_buses(buses_config_commands, bus_size);
    }
    if (pins != NULL || pin_size != 0)
    {
        // export the pins
        export_pins(pins, pin_size);
    }
    if (pin_dir != NULL)
    {
        // set pin directions
        set_pin_direction(pins, pin_size, pin_dir);
    }

    is_module_initialized = true;
}

void GPIO_SetPinValue(int pin_number, const char *value)
{
    if (!value)
    {
        return;
    }
    // concatenate to get pin's direction file location
    char value_path[MAX_PATH_LEN];
    memset(value_path, 0, MAX_PATH_LEN * sizeof(char));
    sprintf(value_path, "%s%d%s", DIGIT_GPIO_PATH, pin_number, VAL_PATH);

    // open pin file for write access
    FILE *exportFile = fopen(value_path, "w");
    if (exportFile == NULL)
    {
        printf("GPIO - ERROR: Unable to open export file (GPIO_SetPinValue). %d\n", pin_number);
        exit(1);
    }

    // write "value" to file
    fprintf(exportFile, "%s", value);

    fclose(exportFile);
}

void GPIO_SetPinDirection(int pin_number, const char *pin_direction)
{
    if (!pin_direction)
    {
        return;
    }
    // concatenate to get pin's direction file location
    char direction_path[MAX_PATH_LEN];
    memset(direction_path, 0, MAX_PATH_LEN * sizeof(char));
    sprintf(direction_path, "%s%d%s", DIGIT_GPIO_PATH, pin_number, DIR_PATH);

    // open pin file for write access
    FILE *exportFile = fopen(direction_path, "w");
    if (exportFile == NULL)
    {
        printf("GPIO - ERROR: Unable to open export file (GPIO_SetPinDirection). %d\n", pin_number);
        exit(1);
    }

    // write pin_direction to file
    fprintf(exportFile, "%s", pin_direction);

    fclose(exportFile);
}

void GPIO_writeFile(const char *file_path, char *value)
{
    if (!file_path || !value)
    {
        return;
    }

    // open file for write access
    FILE *exportFile = fopen(file_path, "w");
    if (exportFile == NULL)
    {
        printf("GPIO - ERROR: Unable to open export file (GPIO_writeFile).\n");
        exit(1);
    }

    // write DIR to file
    fprintf(exportFile, "%s", value);

    fclose(exportFile);
}

void GPIO_cleanup(int *pins, int size)
{

    assert(is_module_initialized);
    // does nothing

    is_module_initialized = false;
}

//------------------------------------------------
/////////////// Private Functions ////////////////
//------------------------------------------------

// Helper function to run linux command
// Note: adapted from assignment one description
static void run_command(const char *command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");

    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe))
    {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
    }
    // printf("--> %s", buffer); // Uncomment for debugging
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0)
    {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

// Helper function to enable buses for i2c
// Enables linux support for the bus "/dev/i2c-1" by configuring the data and clock pins (SDA and SCL)
static void enable_buses(const char **buses, int bus_size)
{
    for (size_t i = 0; i < bus_size; i++)
    {
        run_command(buses[i]);
    }
}

// Helper function to export "pin_size" of pins in "pins"
static void export_pins(const int *pins, int pin_size)
{
    for (size_t i = 0; i < pin_size; i++)
    {
        // concatenate to get pin's file location
        char direction_path[MAX_PATH_LEN];
        sprintf(direction_path, "%s%d", DIGIT_GPIO_PATH, pins[i]);

        // check to see if the pin is already exported
        if (access(direction_path, F_OK) == 0)
        {
            // file exists - don't need to import it
            continue;
        }

        // open pin file for write access
        FILE *exportFile = fopen(EXPORT_PATH, "w");
        if (exportFile == NULL)
        {
            printf("GPIO ERROR: Unable to open export file (export_pins). Pin#=%d\n", pins[i]);
            exit(1);
        }

        fprintf(exportFile, "%d", pins[i]);

        fclose(exportFile);
    }

    // wait for ~300ms before use
    Sleep_ms(300);
}

// Helper function to set pins' directions to "pin_dir"
static void set_pin_direction(const int *pins, int pin_size, const char *pin_direction)
{
    for (size_t i = 0; i < pin_size; i++)
    {
        GPIO_SetPinDirection(pins[i], pin_direction);
    }
}