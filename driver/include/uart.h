/*
 * Remote Receiver from UART
 * Victor Manuel Fernandez Castro
 * 14 August 2015
 */

#ifndef UART_H
#define UART_H

// Initialization of the serial port
int uart_init();

// Destroy the connection
void uart_destroy();

// Dispatching loop
int uart_loop();

#endif
