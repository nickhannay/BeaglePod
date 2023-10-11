/**
 * @file mp3ToWav.h
 * @brief This is a header file for the mp3ToWav module.
 * Note: This Module is not used - Found a more efficient way to convert using the ffmpeg javascript library
 *
 * This source file contains the definitions of the functions
 * for the mp3ToWav module, which provides the utilities
 * for converting an MP3 file to WAV format
 *
 * @author Amirhossein Etaati
 * @date 2023-03-07
 */

#if !defined(MP3TOWAV_H)
#define MP3TOWAV_H

void mp3ToWave_init(void);

void mp3ToWave_convert(char *mp3_path, char *wav_path);

void mp3ToWave_cleanup(void);

#endif // MP3TOWAV_H
