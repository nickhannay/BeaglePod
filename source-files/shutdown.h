/**
 * @file shutDown.h
 * @brief This is a header file for the shutDown module.
 *
 * This header file contains the definitions of the functions
 * for the shutDown module, which provides the utilities
 * for terminating the program cleanly.
 *
 * @author Mehdi Esmaeilzadeh
 * @date 2023-03-10
 */

#ifndef _SHUTDOWN_H_
#define _SHUTDOWN_H_
// shutdown.h
// Module that is responsible for shutting the program down and makes sure that main first lets
// all modules to clean up and then finish the execution

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

// Initialize the condition and mutexes
void Shutdown_init(void);
// It will make the main execution to wait for all the modules to shutdown
void Shutdown_waitForShutdown(void);
// It will tell other modules that is it time to stop the execution and clean up
bool Shutdown_isShutdown(void);
// The shutdown has happend it will wake up the main thread to finish the execution and clean up
void Shutdown_triggerForShutdown(void);

#endif