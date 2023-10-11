/**
 * @file network.c
 * @brief This is a source file for the network module.
 *
 * This source file contains the declarations of the functions
 * for the network module, which provides the utilities
 * for sending, receiving, and parsing UDP packets.
 *
 * @author Amirhossein Etaati
 * @date 2023-03-10
 */

/*
Subject: CMPT433 (Embedded Systems) - BeablePod Project
Purpose: Declarations of the network module to handle UDP messages se
Use cases: To respond to the web interface requests
Author: Amirhossein Etaati
Date: 2023-03-16
*/

#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h> // for strncmp()
#include <unistd.h> // for close()
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include "songManager.h"

#define MSG_MAX_LEN 1024
#define MSG_ACK "ACK"
#define MSG_ACK_LEN 4
#define PORT 12345
#define MAX_SAMPLES_LINE 20

// #define MAX_HISTORY_STR_SIZE 32768
#define MAX_LINE_STR_SIZE 256

static pthread_t thread_id;

bool is_module_initialized = false;

// to handle commands
enum eWebCommands
{
    COMMAND_ADD_SONG = 0,
    COMMAND_REMOVE_SONG,
    COMMAND_VOLUME_UP,
    COMMAND_VOLUME_DOWN,
    COMMAND_SONG_NEXT,
    COMMAND_SONG_PREVIOUS,
    COMMAND_STOP,
    UNKNOWN_COMMAND,
    COMMAND_TOTAL_COUNT // Total number of available commands ??
};

// parse the received command and return the appropriate eWebCommands enum value
static enum eWebCommands parse_command(char messageRx[MSG_MAX_LEN])
{
    if (strncmp(messageRx, "add_song", strlen("add_song")) == 0)
    {
        printf("adding song\n");
        return COMMAND_ADD_SONG;
    }
    else if (strncmp(messageRx, "remove_song", strlen("remove_song")) == 0)
    {
        return COMMAND_REMOVE_SONG;
    }
    else if (strncmp(messageRx, "volume_up", strlen("volume_up")) == 0)
    {
        return COMMAND_VOLUME_UP;
    }
    else if (strncmp(messageRx, "volume_down", strlen("volume_down")) == 0)
    {
        return COMMAND_VOLUME_DOWN;
    }
    else if (strncmp(messageRx, "song_next", strlen("song_next")) == 0)
    {
        return COMMAND_SONG_NEXT;
    }
    else if (strncmp(messageRx, "song_previous", strlen("song_previous")) == 0)
    {
        return COMMAND_SONG_PREVIOUS;
    }
    else if (strncmp(messageRx, "stop", strlen("stop")) == 0)
    {
        return COMMAND_STOP;
    }
    else
    {
        return UNKNOWN_COMMAND;
    }
}

// run the commands and return the appropriate string to be sent back
// returns NULL rerun is required
// the caller should call free()
static void run_command(enum eWebCommands cur_command, char *message)
{
    printf("current command: %d\n", cur_command);
    // TODO: consider using an array of function pointers to respond to each command
    // The array will be initialized in network_init ??
    // The indices of the array will be the values of eWebCommands enum ??
    // char *result = NULL;
    // result = malloc(MSG_ACK_LEN * sizeof(char));
    // snprintf(result, MSG_ACK_LEN, MSG_ACK);
    if (cur_command == COMMAND_ADD_SONG)
    {
        // TODO call the call songManager module to add the new song
        // TODO: SOS - need the path of the song ??
        char *path;      // 0
        char *song_name; // 1
        char *singer;    // 2
        char *album;     // 3

        int iter = 0;

        while ((message = strtok(NULL, "\n")) != NULL)
        {

            printf("%s\n", message);
            printf("after printing\n");

            if (iter == 0)
            {
                path = malloc(strlen(message) + 1);
                strcpy(path, message);
            }
            else if (iter == 1)
            {
                song_name = malloc(strlen(message) + 1);
                strcpy(song_name, message);
            }
            else if (iter == 3)
            {
                album = malloc(strlen(message) + 1);
                strcpy(album, message);
            }
            else if (iter == 2)
            {
                singer = malloc(strlen(message) + 1);
                strcpy(singer, message);
            }
            iter++;
        }

        printf("after parsing\n");
        // song_info * song = create_song
        //  Create Song stuct
        song_info *song_struct = create_song_struct(singer, album, path, song_name);
        songManager_addSongBack(song_struct);
        // add song into linked list back
        printf("DEBUG: add song\n");

        // return result;
    }
    else if (cur_command == COMMAND_REMOVE_SONG)
    {
        char *song_num = strtok(NULL, "\n");
        int index = atoi(song_num);

        songManager_deleteSong(index);
        // Delete a song number index

        // TODO call the call songManager module to remove the song
        // TODO: SOS - need the path of the song ??
        printf("DEBUG: remove song\n");
        // return result;
    }
    else if (cur_command == COMMAND_VOLUME_UP)
    {
        // TODO call the volume module to increase the volume
        printf("DEBUG: volume up\n");
        // return result;
    }
    else if (cur_command == COMMAND_VOLUME_DOWN)
    {
        // TODO call the volume module to decrease the volume
        printf("DEBUG: volume down\n");
        // return result;
    }
    else if (cur_command == COMMAND_SONG_NEXT)
    {
        // TODO call the song module to go to the next song
        printf("DEBUG: next song\n");
        // return result;
    }
    else if (cur_command == COMMAND_SONG_PREVIOUS)
    {
        // TODO call the song module to the previous song
        printf("DEBUG: previous song\n");
        // return result;
    }
    else if (cur_command == COMMAND_STOP)
    {
        // TODO stop all the other threads and modules
        printf("DEBUG: stop\n");
        // return result;
    }
    else
    {
        printf("DEBUG: unkown command\n");
        // return NULL; // TODO: ??
    }
}
// static int get_size_string(int start,char * str) {
//     int size =0;
//     for (size_t i = start; i < strlen(str); i++)
//     {
//         if (str[i] == '\n')
//         {
//             return size;
//         }
//         size++;
//     }
//     return size;
// }

static void network_logic(int socketDescriptor)
{
    bool network_cond = true;
    while (network_cond)
    {

        // create variables required for receiving
        struct sockaddr_in sinRemote; // set to the sender/clients's address. Use to reply
        unsigned int sin_len = sizeof(sinRemote);
        char messageRx[MSG_MAX_LEN];

        // listen for the commands (blocking)

        // buffer size: maximum length minus one to allow null termination (string data)
        int buffer_size = MSG_MAX_LEN - 1;
        printf("Before recieve \n");
        int bytesRx = recvfrom(socketDescriptor, messageRx, buffer_size, 0, (struct sockaddr *)&sinRemote, &sin_len);
        // Check for errors
        if (bytesRx == -1)
        {
            printf("ERROR: Failed to receive. Data is not available and socket is in nonblocking mode\n");
            exit(-1);
        }
        printf("After recieve \n");

        // handle the received message ///////////////////////////////////////////////////////////////////////

        // make the received message null terminated so string functions work
        messageRx[bytesRx] = 0;

        printf("Recieved Message <%s>\n", messageRx);

        // if command is not 'enter' (!= "\n")

        // Command
        // Path
        //

        enum eWebCommands curr_command = UNKNOWN_COMMAND;
        char *token = NULL;
        if (bytesRx > 1)
        {

            token = strtok(messageRx, "\n");
            curr_command = parse_command(token);
        }

        run_command(curr_command, token);

        // extract the command from the message received

        // compose the response message

        if (curr_command == UNKNOWN_COMMAND)
        {
            // unknown command
            continue;
        }
        // char *response = run_command(curr_command);

        // TODO: for now all the replies are ACK; Kept for potential later use
        // if (strlen(response) <= MSG_MAX_LEN)
        // {
        //     // send in one packet
        //     char messageTx[MSG_MAX_LEN];
        //     snprintf(messageTx, MSG_MAX_LEN, "%s", response);

        //     // transmit a reply:
        //     sin_len = sizeof(sinRemote);
        //     sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
        // }

        // check termination condition
        if (curr_command == COMMAND_STOP)
        {
            network_cond = false;
        }
    }
}

// thread function to manage networking logics
static void *network_thread(void *params)
{

    // UPD setups (server/receiver side)

    // initialize Address
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;                // connection may be from network
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // host to network long
    sin.sin_port = htons(PORT);              // host to network short

    // create the socket for UDP
    int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // bind the specified port (PORT)
    bind(socketDescriptor, (struct sockaddr *)&sin, sizeof(sin));

    // check for errors
    if (socketDescriptor == -1)
    {
        printf("ERROR: Failed to create the socket\n");
        exit(-1);
    }

    // int n = -1;

    network_logic(socketDescriptor);

    // close the socket
    close(socketDescriptor);

    return NULL;
}

// start a new thread to listen to the incoming packets and responds to the commands
void Network_init()
{
    // start network thread
    pthread_create(&thread_id, NULL, &network_thread, NULL);

    is_module_initialized = true;
}

// stop network thread
void Network_cleanup()
{
    assert(is_module_initialized);

    // TODO: init function pointers of the commands
    // wait for the thread to finish
    pthread_join(thread_id, NULL);
}

// #include "sleep.h"
// int main(int argc, char const *argv[])
// {
//     Network_init();

//     printf("debug: test main\n");

//     Sleep_ms(5000);

//     Network_cleanup();

//     return 0;
// }
