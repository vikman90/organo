/*
 * Local socket management for the protocol of the organ control
 * Victor Manuel Fernandez Castro
 * 8 August 2015
 */

#ifndef SOCKET_H
#define SOCKET_H

// Initialization of the socket
int socket_init(int uid, int gid);

// Destroy the socket
void socket_destroy();

// Dispatching loop
void socket_loop();

#endif
