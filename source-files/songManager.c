/**
 * @file songManager.c
 * @brief This is a source file for the songManager module.
 *
 * This source file contains the declaration of the functions
 * for the songManager module, which provides the utilities for
 * for playing, stopping and traversing in the list of songs.
 *
 * @author Mehdi Esmaeilzadeh
 * @date 2023-03-10
 */

#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "songManager.h"
#include "doublyLinkedList.h"

#include "lcd_4line.h"

static song_info *current_song_playing = NULL;

static SONG_CURSOR_LINE previous_song_cursor = CURSOR_LINE_NOT_SET;

// Song manager for display
static int previous_song_start_from = -1;
static int CURRENT_AUTOPLAY_SONG = 0;

/********************************PRIVATE FUNCTIONS***********************************************************/
// static song_info *create_song_struct(char *name, char *album, char *path);
static void playSong(wavedata_t *song);
static void displaySongs(SONG_CURSOR_LINE current_song, int from_song_number);
// static bool previously_displayed(SONG_CURSOR_LINE current_song, int from_song_number);
static void setSongs(SONG_CURSOR_LINE current_song, char *song1, char *song2, char *song3, char *song4);
static SONG_CURSOR_LINE getsongCursor(int current_song_number);
static int getfromSongForDisplay(int current_song_number);
static int getCurrentSongNumber();

// static void moveCursorNextPage();
// static void moveCursorPreviousPage();
// static void clean_passed_song(song_info* song);

// static void moveCursorNextPage()
// {
//     doublyLinkedList_iteratorNext(); // 2
//     doublyLinkedList_iteratorNext(); // 3
//     doublyLinkedList_iteratorNext(); // 4
//     doublyLinkedList_iteratorNext(); // 5
// }
// static void moveCursorPreviousPage()
// {
//     doublyLinkedList_iteratorPrev(); // 4
//     doublyLinkedList_iteratorPrev(); // 3
//     doublyLinkedList_iteratorPrev(); // 2
//     doublyLinkedList_iteratorPrev(); // 1
// }

static void setSongs(SONG_CURSOR_LINE current_song, char *song1, char *song2, char *song3, char *song4)
{
    // LCD_clear();
    switch (current_song)
    {
    case CURSOR_LINE_ONE:
        // draw arrow on line 1
        LCD_clear();
        LCD_writeChar(LCD_RIGHT_ARROW);
        LCD_writeString(song1);
        LCD_writeStringAtLine(song2, LCD_LINE2);
        LCD_writeStringAtLine(song3, LCD_LINE3);
        LCD_writeStringAtLine(song4, LCD_LINE4);

        break;
    case CURSOR_LINE_TWO:
        // draw arrow line 2

        LCD_clear();
        LCD_writeStringAtLine(song1, LCD_LINE1);
        LCD_writeStringAtLine("", LCD_LINE2);
        LCD_writeChar(LCD_RIGHT_ARROW);
        LCD_writeString(song2);
        LCD_writeStringAtLine(song3, LCD_LINE3);
        LCD_writeStringAtLine(song4, LCD_LINE4);

        break;
    case CURSOR_LINE_THREE:
        // draw arrow line 3
        LCD_clear();
        LCD_writeString(song1);
        LCD_writeStringAtLine(song2, LCD_LINE2);
        LCD_writeStringAtLine("", LCD_LINE3); // set cursor
        LCD_writeChar(LCD_RIGHT_ARROW);
        LCD_writeString(song3);
        LCD_writeStringAtLine(song4, LCD_LINE4);
        break;
    case CURSOR_LINE_FOUR:
        // draw arrow on line 4
        LCD_clear();
        LCD_writeString(song1);
        LCD_writeStringAtLine(song2, LCD_LINE2);
        LCD_writeStringAtLine(song3, LCD_LINE3);
        LCD_writeStringAtLine("", LCD_LINE4);
        LCD_writeChar(LCD_RIGHT_ARROW); // set cursor
        LCD_writeString(song4);
        break;
    default:
        break;
    }
}

static int getCurrentSongNumber()
{
    return doublyLinkedList_getCurrentIdx() + 1;
}

static void playSong(wavedata_t *song)
{
    AudioPlayer_playWAV(song);
}
// static void clean_passed_song(song_info* song) {
//     if(song != NULL) {
//         // if(song->album != NULL) {
//         //     free(song->album);
//         // }
//         // if(song->author_name != NULL) {
//         //     free(song->author_name);
//         // }
//         // if(song->song_path != NULL) {
//         //     free(song->song_path);
//         // }
//         free(song);
//     }
// }

song_info *create_song_struct(char *name, char *album, char *path, char *song_name_local)
{
    song_info *song = malloc(sizeof(*song));

    printf("aritist: <%s>\n", name);
    printf("album: <%s>\n", album);
    printf("path: <%s>\n", path);
    printf("song name: <%s>\n", song_name_local);

    song->author_name = (char *)malloc(strlen(name) + 1);
    song->album = (char *)malloc(strlen(album) + 1);
    song->song_path = (char *)malloc(strlen(path) + 1);
    song->song_name = (char *)malloc(strlen(song_name_local) + 1);

    strcpy(song->author_name, name);
    strcpy(song->album, album);
    strcpy(song->song_path, path);
    strcpy(song->song_name, song_name_local);

    song->pSong_DWave = malloc(sizeof(*song->pSong_DWave));
    AudioPlayer_readWaveFileIntoMemory(song->song_path, song->pSong_DWave);

    return song;
}

// Returns where the song cursor is located at -- Cursor can be #1, #2, #3, #4
static SONG_CURSOR_LINE getsongCursor(int current_song_number)
{
    int song_cursor = current_song_number;
    if (song_cursor > 4)
    {
        song_cursor -= 4 * (current_song_number / 4);
    }

    if (song_cursor == 1)
    {
        return CURSOR_LINE_ONE;
    }
    else if (song_cursor == 2)
    {
        return CURSOR_LINE_TWO;
    }
    else if (song_cursor == 3)
    {
        return CURSOR_LINE_THREE;
    }
    else if (song_cursor == 4)
    {
        return CURSOR_LINE_FOUR;
    }

    return CURSOR_LINE_NOT_SET;
}
/* Returns the first song that it has to display based on the current song #*/
static int getfromSongForDisplay(int current_song_number)
{
    int from_song = -1;
    // Song # 1, 5 , 9, ... so on
    if (current_song_number % 4 == 1)
    {
        from_song = current_song_number;
    }
    // Song # 4, 8, 12, and so on
    else if (current_song_number % 4 == 0)
    {
        from_song = current_song_number - 3;
    }
    // Any other song # Not listed above
    else if (current_song_number % 4 != 0)
    {
        from_song = current_song_number - ((current_song_number % 4) - 1);
    }
    return from_song;
}
// static bool previously_displayed(SONG_CURSOR_LINE current_song, int from_song_number) {
//     if(current_song == previous_song_cursor && previous_song_start_from == from_song_number) {
//         return true;
//     }
//     if(previous_song_cursor != CURSOR_LINE_NOT_SET && previous_song_start_from != -1) {
//         LCD_clear();
//     }
//     return false;
// }
// Displays all the songs
static void displaySongs(SONG_CURSOR_LINE current_song, int from_song_number)
{
    // if(previously_displayed(current_song, from_song_number)) {
    //     return;
    // }
    song_info *temp1;
    song_info *temp2;
    song_info *temp3;
    song_info *temp4;

    temp1 = doublyLinkedList_getCurrentIteratorElement();
    if (doublyLinkedList_iteratorNext())
    {
        temp2 = doublyLinkedList_getCurrentIteratorElement();
    }
    else
    {
        temp2 = NULL;
    }
    if (doublyLinkedList_iteratorNext())
    {
        temp3 = doublyLinkedList_getCurrentIteratorElement();
    }
    else
    {
        temp3 = NULL;
    }
    if (doublyLinkedList_iteratorNext())
    {
        temp4 = doublyLinkedList_getCurrentIteratorElement();
    }
    else
    {
        temp4 = NULL;
    }
    // char songtemp1[strlen(temp1->author_name) + strlen(temp1->album) + 1];
    // char songtemp2[strlen(temp2->author_name) + strlen(temp2->album) + 1];
    // char songtemp3[strlen(temp3->author_name) + strlen(temp3->album) + 1];
    // char songtemp4[strlen(temp4->author_name) + strlen(temp4->album) + 1];

    char *songtemp1 = NULL;
    char *songtemp2 = NULL;
    char *songtemp3 = NULL;
    char *songtemp4 = NULL;
    if (temp1 != NULL)
    {
        songtemp1 = (char *)malloc(strlen(temp1->author_name) + 1);
    }
    else
    {
        songtemp1 = (char *)malloc(3);
    }
    if (temp2 != NULL)
    {
        songtemp2 = (char *)malloc(strlen(temp2->author_name) + 1);
    }
    else
    {
        songtemp2 = (char *)malloc(3);
    }
    if (temp3 != NULL)
    {
        songtemp3 = (char *)malloc(strlen(temp3->author_name) + 1);
    }
    else
    {
        songtemp3 = (char *)malloc(3);
    }
    if (temp4 != NULL)
    {
        songtemp4 = (char *)malloc(strlen(temp4->author_name) + 1);
    }
    else
    {
        songtemp4 = (char *)malloc(3);
    }

    //     char songtemp1[strlen(temp1->author_name) + 1];
    //     char songtemp2[strlen(temp2->author_name) + 1];
    //     char songtemp3[strlen(temp3->author_name) + 1];
    //     char songtemp4[strlen(temp4->author_name) + 1];

    if (temp1 != NULL)
    {
        strcpy(songtemp1, temp1->author_name);
        // strcat(songtemp1, temp1->album);
    }
    else
    {
        char *cpy = "  ";
        strcpy(songtemp1, cpy);
    }

    if (temp2 != NULL)
    {
        strcpy(songtemp2, temp2->author_name);
        // strcat(songtemp2, temp2->album);
    }
    else
    {
        char *cpy = "  ";
        strcpy(songtemp2, cpy);
    }
    if (temp3 != NULL)
    {
        strcpy(songtemp3, temp3->author_name);
        //  strcat(songtemp3, temp3->album);
    }
    else
    {
        char *cpy = "  ";
        strcpy(songtemp3, cpy);
    }
    if (temp4 != NULL)
    {
        strcpy(songtemp4, temp4->author_name);
        // strcat(songtemp4, temp4->album);
    }
    else
    {
        char *cpy = "  ";
        strcpy(songtemp4, cpy);
    }

    setSongs(current_song, songtemp1, songtemp2, songtemp3, songtemp4);
    previous_song_cursor = current_song;
    previous_song_start_from = from_song_number;
    free(songtemp1);
    free(songtemp2);
    free(songtemp3);
    free(songtemp4);
    doublyLinkedList_setIteratorStartPosition();

    // iterate_through_all_songs();
}

/***************************************PUBLIC FUNCTIONS****************************************************************/
void songManager_init()
{
    doublyLinkedList_init();

    /**** TESTING********/

    // Adds 5 song to the list
    /*char *song1_p = "songs/moves.wav";
    char *song2_p = "songs/hair.wav";
    char *song3_p = "songs/moves.wav";
    char *song4_p = "songs/som-liveletlive.wav";
    char *song5_p = "songs/Wild Ones (feat. Sia).wav";*/
    /*char *song1_name = "Moves";
    char *song2_name = "HairShop";
    char *song3_name = "Author 3";
    char *song4_name = "Author 4";
    char *song5_name = "Author 5";
    char *song1_album = "Dummy 1";
    char *song2_album = "Dummy 2";
    char *song3_album = "Dummy 3";
    char *song4_album = "Dummy 4";
    char *song5_album = "Dummy 5";
    song_info *song1 = create_song_struct(song1_name, song1_album,"songs/moves.wav", song1_p);
    songManager_addSongFront(song1);
    song_info *song2 = create_song_struct(song2_name, song2_album, "songs/hair.wav",song2_p);
    songManager_addSongBack(song2);

    song_info *song3 = create_song_struct(song3_name, song3_album, song3_p);
    songManager_addSongBack(song3);
    song_info *song4 = create_song_struct(song4_name, song4_album, song4_p);
    songManager_addSongBack(song4);
    song_info *song5 = create_song_struct(song5_name, song5_album, song5_p);
    songManager_addSongFront(song1);
    songManager_addSongBack(song2);
    songManager_addSongBack(song3);
    songManager_addSongBack(song4);
    songManager_addSongBack(song5);
    */
}

// size_t songManager_currentNumberSongs()
// {
//     int size = 0;
//     struct Node *temp = doublyLinkedList_getHead();
//     while (temp != NULL)
//     {
//         size++;
//         temp = temp->next;
//     }
//     return size;
// }

void songManager_playSong()
{
    song_info *temp = doublyLinkedList_getCurrentElement();
    CURRENT_AUTOPLAY_SONG = doublyLinkedList_getCurrentIdx();
    if (temp == NULL)
    {
        printf("Song does not exist\n");
    }
    else
    {
        current_song_playing = temp;
        playSong(current_song_playing->pSong_DWave);
    }
}

void songManager_AutoPlayNext(void)
{
    CURRENT_AUTOPLAY_SONG++;
    song_info *song = (song_info *)doublyLinkedList_getElementAtIndex(CURRENT_AUTOPLAY_SONG);
    playSong(song->pSong_DWave);
}

void songManager_addSongFront(song_info *song)
{
    doublyLinkedList_prependItem(song, sizeof(*song));
}
void songManager_addSongBack(song_info *song)
{
    doublyLinkedList_appendItem(song, sizeof(*song));
}

void songManager_displaySongs()
{
    if (!doublyLinkedList_getSize())
    {
        LCD_clear();
        LCD_writeStringAtLine("Empty song library", LCD_LINE1);
        return;
    }
    int current_song_number = getCurrentSongNumber();
    int from_song = getfromSongForDisplay(current_song_number);
    doublyLinkedList_setIterator(from_song - 1);

    SONG_CURSOR_LINE song_cursor = getsongCursor(current_song_number);
    displaySongs(song_cursor, from_song);
    printf("finished displaying, current_song_number: %d, song_cursor: %d from song: %d\n", current_song_number, song_cursor, from_song);
}

void songManager_reset()
{
    // current_song_number = 1;
    doublyLinkedList_setCurrent(0);
    doublyLinkedList_setIteratorStartPosition();
    previous_song_cursor = CURSOR_LINE_NOT_SET;
    previous_song_start_from = -1;
}
void songManager_moveCursorDown()
{
    doublyLinkedList_next();
    songManager_displaySongs();
}

void songManager_moveCursorUp()
{
    doublyLinkedList_prev();
    songManager_displaySongs();
}

void songManager_deleteSong(int index)
{
    doublyLinkedList_delete(index);
    songManager_displaySongs();
}

// Gets the data of the "current"
song_info *songManager_getCurrentSongPlaying(void)
{
    return current_song_playing;
}

// Frees the memory for all nodes, the data, and the List struct
void songManager_cleanup(void)
{
    doublyLinkedList_cleanup();
}
