/**
 * @file audio_player.c
 * @brief This is a source file for the Audio Player module.
 *
 * This source file contains the declaration of the functions
 * for the Audio Player module, which provides the utilities
 * for reading, storing and playing audio (.WAV) files
 *
 * @author Nick Hannay
 * @date 2023-03-13
 */

#include <time.h>
#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <alloca.h>

#include "songManager.h"
#include "audio_player.h"

// Global Variables
static snd_pcm_t *handle;
static unsigned long playbackBufferSize = 0;
static short *playbackBuffer = NULL;
static int volume = 0;

// Private functions definitions
static int runCommand(char *command);
static void *playbackThread(void *arg);
static int getSinkIndexes(int *sink_indexes);
static void fillPlaybackBuffer(short *buff, int size);

typedef struct
{
	// a pointer to the raw PCM data
	wavedata_t *pSound;

	// The offset into the pData of pSound. Indicates how much of the
	// sound has already been played (and hence where to start playing next).
	int location;
} playbackSong_t;

// Playback threading
void *playbackThread(void *arg);
static bool stopping = false;
static pthread_t playbackThreadId;
static pthread_mutex_t audioMutex = PTHREAD_MUTEX_INITIALIZER;
static playbackSong_t current_sound;
static bool SONG_PLAYED = false;

//------------------------------------------------
//////////////// Public Functions ////////////////
//------------------------------------------------

void AudioPlayer_init(void)
{

	AudioPlayer_setVolume(DEFAULT_VOLUME);

	// Open the PCM output
	int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0)
	{
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Configure parameters of PCM output
	err = snd_pcm_set_params(handle,
							 SND_PCM_FORMAT_S16_LE,
							 SND_PCM_ACCESS_RW_INTERLEAVED,
							 NUM_CHANNELS,
							 SAMPLE_RATE,
							 1,		 // Allow software resampling
							 50000); // 0.05 seconds per buffer
	if (err < 0)
	{
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Allocate this software's playback buffer to be the same size as the
	// the hardware's playback buffers for efficient data transfers.
	// ..get info on the hardware buffers:
	unsigned long unusedBufferSize = 0;
	snd_pcm_get_params(handle, &unusedBufferSize, &playbackBufferSize);
	playbackBufferSize = playbackBufferSize;
	// ..allocate playback buffer:
	playbackBuffer = malloc(playbackBufferSize * sizeof(*playbackBuffer));

	// Launch playback thread:
	pthread_create(&playbackThreadId, NULL, playbackThread, NULL);
}

// Client code must call AudioMixer_freeWaveFileData to free dynamically allocated data.
void AudioPlayer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound)
{
	sleep(1);
	assert(pSound);
	pthread_mutex_lock(&audioMutex);
	{
		// The PCM data in a wave file starts after the header:
		const int PCM_DATA_OFFSET = 44;

		// Open the wave file
		FILE *file = fopen(fileName, "r");
		if (file == NULL)
		{
			fprintf(stderr, "ERROR: Unable to open file <%s>.\n", fileName);
			exit(EXIT_FAILURE);
		}
		printf("getting file size\n");
		// Get file size
		fseek(file, 0, SEEK_END);
		int sizeInBytes = ftell(file) - PCM_DATA_OFFSET;
		pSound->numSamples = sizeInBytes / (SAMPLE_SIZE);

		// Search to the start of the data in the file
		fseek(file, PCM_DATA_OFFSET, SEEK_SET);

		// Allocate space to hold all PCM data
		pSound->pData = malloc(sizeInBytes);
		if (pSound->pData == 0)
		{
			fprintf(stderr, "ERROR: Unable to allocate %d bytes for file %s.\n",
					sizeInBytes, fileName);
			exit(EXIT_FAILURE);
		}
		printf("begin reading file\n");
		clock_t begin = clock();
		// Read PCM data from wave file into memory
		int samplesRead = fread(pSound->pData, SAMPLE_SIZE, pSound->numSamples, file);
		if (samplesRead != pSound->numSamples)
		{
			fprintf(stderr, "ERROR: Unable to read %d samples from file %s (read %d).\n",
					pSound->numSamples, fileName, samplesRead);
			exit(EXIT_FAILURE);
		}
		clock_t end = clock();
		printf("finished reading, it took: %fs\n", (double)(end - begin) / CLOCKS_PER_SEC);
		fclose(file);
	}
	pthread_mutex_unlock(&audioMutex);
}

void AudioPlayer_freeWaveFileData(wavedata_t *pSound)
{
	pSound->numSamples = 0;
	free(pSound->pData);
	pSound->pData = NULL;
}

void AudioPlayer_playWAV(wavedata_t *pSound)
{
	// Ensure we are only being asked to play "good" sounds:
	assert(pSound->numSamples > 0);
	assert(pSound->pData);
	pthread_mutex_lock(&audioMutex);
	{
		// sample to list of sound bites
		current_sound.pSound = pSound;
		current_sound.location = 0;
		pthread_mutex_unlock(&audioMutex);
		return;
	}
	pthread_mutex_unlock(&audioMutex);

	fprintf(stderr, "Failed to update current song\n");
}

void AudioPlayer_cleanup(void)
{
	printf("Stopping audio...\n");

	// Stop the PCM generation thread
	stopping = true;
	pthread_join(playbackThreadId, NULL);

	// Shutdown the PCM output, allowing any pending sound to play out (drain)
	snd_pcm_drain(handle);
	snd_pcm_close(handle);

	// Free playback buffer
	// (note that any wave files read into wavedata_t records must be freed
	//  in addition to this by calling AudioMixer_freeWaveFileData() on that struct.)
	free(playbackBuffer);
	playbackBuffer = NULL;

	printf("Done stopping audio...\n");
	fflush(stdout);
}

int AudioPlayer_getVolume()
{
	// Return the cached volume; good enough unless someone is changing
	// the volume through other means and the cached value is out of date.
	int vol = volume;
	return vol;
}

void AudioPlayer_setVolume(double newVolume)
{
	// pthread_mutex_lock(&audioMutex);
	{
		int *sinks = malloc(2 * sizeof(int));
		int valid = getSinkIndexes(sinks);
		char vol[16] = {0};
		snprintf(vol, sizeof(vol), "%f", newVolume);
		char sink_index[16] = {0};

		snprintf(sink_index, sizeof(sink_index), "%d ", *(sinks + valid - 1));

		char *tmp = "pactl set-sink-volume ";
		char *command = calloc(strlen(tmp) + strlen(sink_index) + strlen(vol) + 1, sizeof(char));
		command = strcat(strcat(strcat(command, tmp), sink_index), vol);

		runCommand(command);
		volume = (int)(newVolume * 100);

		free(sinks);
		sinks = NULL;
		free(command);
		command = NULL;
	}
	// pthread_mutex_unlock(&audioMutex);
}

//------------------------------------------------
/////////////// Private Functions ////////////////
//------------------------------------------------

static int runCommand(char *command)
{
	FILE *pipe = popen(command, "r");

	char buffer[1024];
	while (!feof(pipe) && !ferror(pipe))
	{
		if (fgets(buffer, sizeof(buffer), pipe))
		{
			break;
		}
	}

	int exit_code = WEXITSTATUS(pclose(pipe));
	if (exit_code != 0)
	{
		return (-1);
	}

	return (0);
}

static int getSinkIndexes(int *sink_indexes)
{
	FILE *pipe = popen("pactl list short sinks", "r");
	int valid = 0;

	char buffer[1024] = {0};
	for (int i = 0; i < 2; i++)
	{
		if (fgets(buffer, sizeof(buffer), pipe) == NULL)
		{
			break;
		}
		valid++;
		*(sink_indexes + i) = atoi(buffer);
	}

	int exit_code = WEXITSTATUS(pclose(pipe));
	if (exit_code != 0)
	{
		return (-1);
	}

	return (valid);
}

// Fill the `buff` array with new PCM values to output.
//    `buff`: buffer to fill with new PCM data from sound bites.
//    `size`: the number of values to store into playbackBuffer
static void fillPlaybackBuffer(short *buff, int size)
{
	// discard old pcm data
	memset(buff, 0, size * SAMPLE_SIZE);

	pthread_mutex_lock(&audioMutex);
	{

		wavedata_t *sound_data = current_sound.pSound;
		if (sound_data != NULL && (sound_data->numSamples - current_sound.location) > 0)
		{
			printf("sound not null\n");
			SONG_PLAYED = true;
			// copy into playback buff
			int total_samples = sound_data->numSamples;
			int location = current_sound.location;
			int samples_left = (total_samples - location);
			short *start_copy = sound_data->pData + location;
			short left_val, right_val;

			if (samples_left > size)
			{
				samples_left = size;
			}

			for (int i = 0; i < samples_left - 1; i += NUM_CHANNELS)
			{

				// get left sample
				if (*(start_copy + i) > SHRT_MAX)
				{
					left_val = SHRT_MAX;
				}
				else if (*(start_copy + i) < SHRT_MIN)
				{
					left_val = SHRT_MIN;
				}
				else
				{
					left_val = *(start_copy + i);
				}
				*(buff + i) = left_val;

				// get right sample
				if (*(start_copy + i + 1) > SHRT_MAX)
				{
					right_val = SHRT_MAX;
				}
				else if (*(start_copy + i + 1) < SHRT_MIN)
				{
					right_val = SHRT_MIN;
				}
				else
				{
					right_val = *(start_copy + i + 1);
				}
				*(buff + i + 1) = right_val;
			}
			current_sound.location += samples_left;
		}
		else if (SONG_PLAYED)
		{
			pthread_mutex_unlock(&audioMutex);
			songManager_AutoPlayNext();
		}
	}
	pthread_mutex_unlock(&audioMutex);
}

static void *playbackThread(void *arg)
{

	while (!stopping)
	{
		// Generate next block of audio
		fillPlaybackBuffer(playbackBuffer, playbackBufferSize);

		// Output the audio
		snd_pcm_sframes_t frames = snd_pcm_writei(handle,
												  playbackBuffer, playbackBufferSize / NUM_CHANNELS);

		// Check for (and handle) possible error conditions on output
		if (frames < 0)
		{
			fprintf(stderr, "AudioPlayer: writei() returned %li\n", frames);
			frames = snd_pcm_recover(handle, frames, 1);
		}
		if (frames < 0)
		{
			fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n",
					frames);
			exit(EXIT_FAILURE);
		}
		if (frames > 0 && frames < playbackBufferSize / NUM_CHANNELS)
		{
			printf("Short write (expected %li, wrote %li)\n",
				   playbackBufferSize, frames);
		}
	}

	return NULL;
}
