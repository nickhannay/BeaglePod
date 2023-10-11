/**
 * @file beaglePod.c
 * @brief This is a source file for the main program
 *
 * This source file call the module for each of the features
 * to initialize, run and clean up the program.
 *
 * @author Amirhossein Etaati
 * @date 2023-03-01
 */

#include "sleep.h"
#include "shutdown.h"
#include "bluetooth.h"
#include "audio_player.h"
#include "gpio.h"
#include "joystick.h"
#include "menuManager.h"
#include "volume.h"
#include "lcd_4line.h"
#include "network.h"
#include "songManager.h"

int main(int argc, char const *argv[])
{

    AudioPlayer_init();
    Potentiometer_init();
    MenuManager_init();
    Network_init();
    songManager_init();

    Shutdown_init();
    Shutdown_waitForShutdown();

    songManager_cleanup();
    Network_cleanup();
    MenuManager_cleanup();
    Potentiometer_cleanup();
    AudioPlayer_cleanup();

    return 0;
}
