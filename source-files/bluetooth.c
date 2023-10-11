/**
 * @file bluetooth.c
 * @brief This is a source file for the Bluetooth module.
 *
 * This source file contains the declaration of the functions
 * for the Bluetooth module, which provides the utilities
 * for scanning, pairing, connecting and disconnecting to nearby devices.
 *
 * @author Nick Hannay
 * @date 2023-03-07
 */

#include "bluetooth.h"
#include "audio_player.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

// the duration of the bluetooth scan = 1.28s * SCAN_LENGTH
#define SCAN_LENGTH 4

// Bluetooth Info
int bt_adapter_id;
int bt_adapter_fd;

// Private functions definitions
static void openBT(void);
static void closeBT(void);
static void checkError(void);
static int runCommand(char *command);

//------------------------------------------------
//////////////// Public Functions ////////////////
//------------------------------------------------

void Bluetooth_printDevicesToConsole(inquiry_info *devices, int num_devices)
{
    openBT();
    char addr[19] = {0};
    char name[256] = {0};
    for (int i = 0; i < num_devices; i++)
    {
        // bdaddr is the 6 byte address of the bluetooth device. format: XX:XX:XX:XX:XX:XX
        ba2str(&(devices + i)->bdaddr, addr);
        memset(name, 0, sizeof(name));

        // send request to device with address equal to bdaddr, for human readable name
        if (hci_read_remote_name(bt_adapter_fd, &(devices + i)->bdaddr, sizeof(name), name, 0) < 0)
        {
            strcpy(name, "[unknown]");
        }
        printf("[%d]: %s\n", i, name);
    }
    closeBT();
}

void Bluetooth_getHumanReadableNames(bluetooth_scan_t *scanned_devices, char *names[])
{
    openBT();
    for (int i = 0; i < scanned_devices->num_devices; i++)
    {

        names[i] = calloc(256, sizeof(char));
        // send request to device with address equal to bdaddr, for human readable name
        if (hci_read_remote_name(bt_adapter_fd, &(scanned_devices->devices + i)->bdaddr, 255, names[i], 0) < 0)
        {
            strcpy(names[i], "[unknown]");
        }
    }
    closeBT();
}

int Bluetooth_pair(bdaddr_t *device_address)
{
    openBT();
    char addr[19] = {0};
    ba2str(device_address, addr);
    char *tmp = "bluetoothctl pair ";
    char *bt_command = calloc(sizeof(char), strlen(tmp) + 19);

    strcat(strcat(bt_command, tmp), addr);

    int result = runCommand(bt_command);
    free(bt_command);
    bt_command = NULL;
    closeBT();

    return result;
}

int Bluetooth_scan(bluetooth_scan_t *scanner)
{
    openBT();
    scanner->devices = malloc(BT_MAX_DEV_RSP * sizeof(inquiry_info));
    if (scanner->devices == NULL)
    {
        fprintf(stderr, "failed to allocate memory for scanned devices\n");
        closeBT();
        return (-1);
    }

    // scan for bluetooth devices
    int num_rsp = hci_inquiry(bt_adapter_id, SCAN_LENGTH, BT_MAX_DEV_RSP, NULL, &scanner->devices, IREQ_CACHE_FLUSH);
    if (num_rsp < 0)
    {
        fprintf(stderr, "hci_inquiry: error scanning devices");
        closeBT();
        return (-1);
    }

    scanner->num_devices = num_rsp;
    closeBT();
    return (1);
}

void Bluetooth_disconnect(void)
{
    openBT();
    runCommand("bluetoothctl disconnect");
    closeBT();
}

int Bluetooth_connect(bdaddr_t *device_address)
{
    openBT();
    char addr[19] = {0};
    ba2str(device_address, addr);
    char *tmp = "bluetoothctl connect ";
    char *bt_command = calloc(sizeof(char), strlen(tmp) + 19);

    strcat(strcat(bt_command, tmp), addr);

    int result = runCommand(bt_command);
    free(bt_command);
    bt_command = NULL;

    closeBT();
    return result;
}

//------------------------------------------------
/////////////// Private Functions ////////////////
//------------------------------------------------

static void openBT(void)
{
    // get default bt adapter
    bt_adapter_id = hci_get_route(NULL);
    checkError();
    bt_adapter_fd = hci_open_dev(bt_adapter_id);
    checkError();
}

static void closeBT(void)
{
    if (close(bt_adapter_fd) == -1)
    {
        fprintf(stderr, "failed to close bluetooth file descriptor\n");
    }
}

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

static void checkError(void)
{
    switch (errno)
    {
    case EADDRNOTAVAIL:
        fprintf(stderr, "Error: Bluetooth hardware address not available\n");
        exit(1);
        break;
    case EFAULT:
        fprintf(stderr, "Error: Invalid memory location for Bluetooth hardware address\n");
        exit(1);
        break;
    case EINVAL:
        fprintf(stderr, "Error: Invalid Bluetooth device identifier\n");
        exit(1);
        break;
    case EBADF:
        fprintf(stderr, "Error: Invalid Bluetooth device id\n");
        exit(1);
        break;
    case EBUSY:
        fprintf(stderr, "Error: Bluetooth adapter already in use\n");
        exit(1);
        break;
    case EIO:
        fprintf(stderr, "Error: IO error when attempting to open Bluetooth adapter\n");
        exit(1);
        break;
    case ENODEV:
        fprintf(stderr, "Error: Bluetooth adapter does not exist or is unavailable\n");
        exit(1);
        break;
    case ENOMEM:
        fprintf(stderr, "Error: Insufficient memory to allocate Bluetooth resources\n");
        exit(1);
        break;
    case ENOTTY:
        fprintf(stderr, "Error: Specified fd is not a TTY device\n");
        exit(1);
        break;
    default:
        // no error
        break;
    }
}