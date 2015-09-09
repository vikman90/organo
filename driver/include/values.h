/*
 * Constant values
 * Victor Manuel Fernandez Castro
 * 22 August 2015
 */

#ifndef VALUES_H
#define VALUES_H

#define MIDI_DEFAULT_TEMPO 500000	// usec / quarter = 120 bpm
#define MIDI_DEFAULT_METRO 24		// Clocks per quarter

#define SCORE_HOME "/home/pi/midis/"
#define BUFFER_LENGTH 4096	// Length of receiving buffer
#define LOG_IDENT "organd"	// Logging identity

/*
 * Track 0: 7 keys, starting at C4 (baroque keyboard)
 * Track 1: 7 keys, starting at C4 (romantic keyboard)
 * Track 2: 7 keys, starting at C4 (pedals)
 * Track 3: 7 keys, starting at C4 (stops)
 */

#define OUTPUT_NTRACKS  4			// Number of tracks
#define OUTPUT_LENGTH 7				// Length of each register
#define OUTPUT_OFFSET 60			// Base MIDI note

#define UART_PATH "/dev/ttyAMA0"	// Path to UART
#define UART_BAUDRATE B9600			// Baudrate of remote receiver
#define UART_BUFFER_MAX 10			// Minimal length of received data (lock)
#define UART_BUFFER_MIN 10			// Maximum length of received data (buffer)
#define UART_SERIAL "0016D34"		// Remote serial code

#define SOCKET_BACKLOG 5					// Listening queue length
#define SOCKET_PATH "/run/organd.sock"	// Path to socket file

#define LCD_ROWS 4
#define LCD_COLS 20
#define LCD_RS 20
#define LCD_ES 16
#define LCD_D4 12
#define LCD_D5 7
#define LCD_D6 8
#define LCD_D7 25
#define LCD_TIMEOUT 1.0				// Timeout to force LCD updating (seconds)

#define ROT_CH_A 18					// Pin for channel A
#define ROT_CH_B 24					// Pin for channel B
#define ROT_PUSH 23					// Pin for button

#define GPIO_BASE 0x20200000		// Physical address of GPIO
#define GPIO_LENGTH 0x80			// Length of GPIO (bytes)
#define GPIO_PULSE_WIDTH { 0, 100 }	// Pulse width (seconds, ns)

#define PIN_RCKL 27					// Register clock
#define PIN_SRCKL 22				// Shifting clock
#define PIN_PORTS { 2, 3, 17, 4 }	// Ports for channels
#define PIN_BUZZER 21				// Buzzer

#define METRONOME_PULSE { 0, 1000000 }	// Pulse width (seconds, ns)

#define DB_HOST "localhost"
#define DB_USER "organo"
#define DB_PASS "ugr2015"
#define DB_NAME "organo"

#endif