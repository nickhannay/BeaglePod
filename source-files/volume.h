/**
 * @file volume.h
 * @brief This is a header file for the volume module.
 *
 * This header file contains the definitions of the functions
 * for the volume module, which provides the utilities for
 * changing the current volume the playing songs.
 *
 * @author Nick Hannay
 * @date 2023-03-15
 */

/**
 * This module responsible for reading the potentiometer,
 * and setting the volume accordingly
 */
#ifndef VOLUME_H
#define VOLUME_H

// begins the thread that reads the potentiometer
void Potentiometer_init(void);

// ends the thread that reads the potentiometer
void Potentiometer_cleanup(void);

// Returns the A2D reading of the voltage input 'input'
// Returns value in range 0 - 4095
int Potentiometer_getA2DReading(void);

#endif