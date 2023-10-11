/**
 * @file sleep.c
 * @brief This is a header file for the sleep module.
 *
 * This header file contains the declaration of the functions
 * for the sleep module, which provides the utilities for
 * putting the calling thread into sleep for the specified
 * amount of time.
 *
 * @author Amirhossein Etaati
 * @date 2023-03-07
 */

/*
Purpose: Declaration of the Sleep module to handle required waitings
*/

#if !defined(_SLEEP_H)
#define _SLEEP_H

// Puts thread to sleep for 'delayInMs' milliseconds
void Sleep_ms(long long delayInMs);

// Puts thread to sleep for 's' seconds and  'ns' nanoseconds
// nanosleep wrapper function - accepts seconds and nanoseconds to construct delay
// Note: Adapted from the 16x2CharacterLCDThroughGPIO student guide
// URL: https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/links/files/2022-student-howtos/16x2CharacterLCDThroughGPIO.pdf
void Sleep_ns(int s, int ns);

#endif // _SLEEP_H
