/**
 * @file shutDown.c
 * @brief This is a source file for the shutDown module.
 *
 * This source file contains the declarations of the functions
 * for the shutDown module, which provides the utilities
 * for terminating the program cleanly.
 *
 * @author Mehdi Esmaeilzadeh
 * @date 2023-03-10
 */

#include "shutdown.h"

static pthread_cond_t cond_main_thread;
static pthread_mutex_t cond_mutex;
static bool shutdown_module;

static pthread_mutex_t shut_down_mutex = PTHREAD_MUTEX_INITIALIZER;

void Shutdown_init(void)
{
    pthread_cond_init(&cond_main_thread, NULL);
    pthread_mutex_init(&cond_mutex, NULL);
    shutdown_module = false;
}

bool Shutdown_isShutdown(void)
{
    bool shut_down_result = false;
    pthread_mutex_lock(&shut_down_mutex);
    shut_down_result = shutdown_module;
    pthread_mutex_unlock(&shut_down_mutex);

    return shut_down_result;
}
void Shutdown_waitForShutdown(void)
{
    pthread_mutex_lock(&cond_mutex);
    pthread_cond_wait(&cond_main_thread, &cond_mutex);
    pthread_mutex_unlock(&cond_mutex);
}

void Shutdown_triggerForShutdown(void)
{
    pthread_cond_broadcast(&cond_main_thread);
    pthread_cond_destroy(&cond_main_thread);
    pthread_mutex_destroy(&cond_mutex);
    shutdown_module = true;
}