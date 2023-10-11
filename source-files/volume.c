/**
 * @file volume.c
 * @brief This is a source file for the volume module.
 *
 * This source file contains the definitions of the functions
 * for the volume module, which provides the utilities for
 * changing the current volume the playing songs.
 *
 * @author Nick Hannay
 * @date 2023-03-15
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

#include "sleep.h"
#include "audio_player.h"
#include "volume.h"

#define A2D_FILE_POT "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_MAX_READING 4095


void* potentiometerThread(void* arg);
static bool stopping = false;
static pthread_t potentiometerThreadId;


void Potentiometer_init(void){

    pthread_create(&potentiometerThreadId, NULL, potentiometerThread, NULL);
}

void Potentiometer_cleanup(void){   
    stopping = true;
    pthread_join(potentiometerThreadId, NULL);
}

// Returns the A2D reading of the voltage input 'input'
// Returns value in range 0 - 4095
int Potentiometer_getA2DReading(){
    
    char * file_name = A2D_FILE_POT;
    
    FILE* f = fopen(file_name, "r");
    if(!f){
        perror(file_name);
        printf("\n");
        exit(-1);
    }

    int a2d_reading = 0;
    int items_read = fscanf(f, "%d", &a2d_reading);
    if(items_read <= 0){
        fprintf(stderr, "Unable to read voltage from file\n");
        exit(-1);
    }

    fclose(f); 
    return a2d_reading;
}



void* potentiometerThread(void* args){

    while(!stopping){
        // read pot
        int reading = Potentiometer_getA2DReading();
        double volume = reading / (double) A2D_MAX_READING;
        AudioPlayer_setVolume(volume);

        
        Sleep_ms(50);

    }
    return NULL;
}