/**
 * @file sleep.c
 * @brief This is a source file for the sleep module.
 *
 * This source file contains the declaration of the functions
 * for the sleep module, which provides the utilities for
 * putting the calling thread into sleep for the specified
 * amount of time.
 *
 * @author Amirhossein Etaati
 * @date 2023-03-07
 */

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "sleep.h"

void Sleep_ms(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *)NULL);
}

void Sleep_ns(int s, int ns)
{
    struct timespec delay = {s, ns};
    nanosleep(&delay, (struct timespec *)NULL);
}