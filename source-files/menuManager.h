/**
 * @file menuManager.h
 * @brief This is a header file for the menuManager module.
 *
 * This header file contains the definitions of the functions
 * for the menuManager module, which provides the utilities
 * for navigating through the Menu of the BeaglePod
 *
 * @author Mehdi Esmaeilzadeh
 * @date 2023-03-14
 */

#if !defined(_MENUMANAGER_H)
#define _MENUMANAGER_H

/**
 * Enum to keep track of current Menu to display
 */
typedef enum
{
  MAIN_MENU,
  SONGS_MENU,
  BLUETOOTH_MENU,
  BTSCAN_MENU,
  SETTINGS_MENU,
  NUM_MENUS
} MENU;

/**
 * Enum to keep track Main Menu Options
 */
typedef enum
{
  SONGS_OPT,
  BLUETOOTH_OPT,
  SETTINGS_OPT,
  POWEROFF_OPT,
  NUM_MAIN_OPTIONS
} MAIN_OPTIONS;

/**
 * Enum to keep track of Bluetooth menu options
 */
typedef enum
{
  BTMENU_CONNECT,
  BTMENU_DISCONNECT,
  NUM_BT_OPTIONS
} BLUETOOTH_OPTIONS;

#include "songManager.h"

// Initialize all the modules used in the menu
void MenuManager_init(void);

// Clean Up all the modules used in the menu
void MenuManager_cleanup(void);

// Returns the current song playing by the user
song_info *MenuManager_GetCurrentSongPlaying();

#endif // _MENUMANAGER_H
