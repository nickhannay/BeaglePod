/**
 * @file audio_player.h
 * @brief This is a header file for the Audio Player module.
 *
 * This header file contains the definitions of the functions
 * for the Audio Player module, which provides the utilities
 * for reading, storing and playing audio (.WAV) files
 *
 * @author Nick Hannay
 * @date 2023-03-13
 */

#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#define AUDIO_PLAYER_MAX_VOLUME 100
#define AUDIO_PLAYER_MIN_VOLUME 0

#define DEFAULT_VOLUME 0.8
#define SAMPLE_RATE 48000
#define NUM_CHANNELS 2 // sample rate

// size of each PCM sample from the audio file.
// each frame sent to the PCM device
// will consist of NUM_CHANNELS samples
#define SAMPLE_SIZE (sizeof(short))

typedef struct
{
	int numSamples;
	short *pData;
} wavedata_t;

// init() must be called before any other functions,
void AudioPlayer_init(void);

// cleanup() must be called last to stop playback threads and free memory.
void AudioPlayer_cleanup(void);

// Read the contents of a wave file into the pSound structure. Note that
// the pData pointer in this structure will be dynamically allocated in
// readWaveFileIntoMemory(), and is freed by calling freeWaveFileData().
void AudioPlayer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound);
void AudioPlayer_freeWaveFileData(wavedata_t *pSound);

// Queue up another sound bite to play as soon as possible.
void AudioPlayer_playWAV(wavedata_t *pSound);

// Get/set the volume.
// setVolume() function posted by StackOverflow user "trenki" at:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
int AudioPlayer_getVolume(void);
void AudioPlayer_setVolume(double newVolume);

#endif