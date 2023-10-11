/**
 * @file network.h
 * @brief This is a header file for the network module.
 *
 * This header file contains the declarations of the functions
 * for the network module, which provides the utilities
 * for sending, receiving, and parsing UDP packets.
 *
 * @author Amirhossein Etaati
 * @date 2023-03-10
 */

#if !defined(_NETWORK_H_)
#define _NETWORK_H_

// Start a new thread to listen to the incoming packets and responds to the commands
void Network_init();

// Wait until network thread stops
void Network_cleanup();

#endif // _NETWORK_H_
