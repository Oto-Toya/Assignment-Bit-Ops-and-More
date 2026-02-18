/* File: bitOps.c
 * Author: Denis Myer
 * Last Edited: Gary Hubley - WenXing Tan
 * Date: 2026/02/12
 * Description: DEMONSTRATION OF BOOLEAN AND SHIFT, BIT OPERATIONS
 */
#include <linux/kd.h>           // KDSETLED
#include <stdio.h>
#include <sys/ioctl.h>          // ioctl() for KDSETLED
#include <sys/time.h>           // select() for kbhit()
#include <sys/types.h>          // select() for kbhit()
#include <termios.h>            // termios for changemode()
#include <unistd.h>             // STDIN_FILENO for kbhit(), usleep() for DelayMS()

void changemode(int mode);      // 1 for non-blocking, 0 for blocking
int kbhit(void);                // Returns 1 if keyboard input is available, 0 otherwise
void DelayMS(long long ms);     // Delays execution for a specified number of milliseconds

// Main function demonstrating bit operations and keyboard interaction
int main(void) {
    int bitmode = 1;
    char ch = '\0';
    changemode(1);                // Set terminal to non-blocking mode

    // Main loop to handle keyboard input and LED control
    // The loop continues until 'q' or 'Q' is pressed
    while (ch != 'q' && ch != 'Q') {
        while (!kbhit()) {          // Loop until a key is pressed
            DelayMS(500);             // Delay to control the speed of LED changes

            if (ch == 'l') {          // If 'l' is pressed, shift the bitmode left
                bitmode = bitmode << 1; // Shift left to move to the next LED

                if (bitmode == 8) {     // If bitmode exceeds 7 (0b111), reset to 1 (0b001)
                    bitmode = 1;
                }
                ioctl(1, KDSETLED, bitmode); // Update the LED state based on the current bitmode
            }
            else if (ch == 'r') {   // If 'r' is pressed, shift the bitmode right
                bitmode = bitmode >> 1; // Shift right to move to the previous LED

                if (bitmode == 0) {     // If bitmode becomes 0 (no LEDs), reset to 4 (0b100) to light the last LED
                    bitmode = 4;
                }
                ioctl(1, KDSETLED, bitmode); // Update the LED state based on the current bitmode
            }
        }

        ch = getchar();             // Read the pressed key

        // Check if the pressed key is 'q' or 'Q' to quit the program, returns to normal terminal mode before exiting
        if (ch == 'q' || ch == 'Q') {
            printf("\nQuit\n");
            changemode(0);
            return 0;
        }
        ioctl(1, KDSETLED, bitmode); // Update the LED state based on the current bitmode after processing the key press
    }

    changemode(0);
    return 0;
}

// Function to change terminal mode for non-blocking input
void changemode(int mode) {
    static struct termios oldt, newt; // Static variables to store terminal settings

    // If mode is 1, set terminal to non-blocking mode; if mode is 0, restore original terminal settings
    if (mode == 1) {
        tcgetattr(STDIN_FILENO, &oldt);    // Get current terminal settings and store in oldt
        newt = oldt;                       // Copy old settings to newt
        newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echoing of input characters
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Apply new terminal settings immediately
    }
    else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore original terminal settings immediately
    }
}

// Function to check if a keyboard key has been hit (non-blocking)
int kbhit(void) {
    struct timeval timeout;       // Structure to specify the timeout for select()
    fd_set rdfs;                  // Set of file descriptors to be monitored for reading

    timeout.tv_sec = 0;           // Set timeout to 0 seconds for non-blocking behavior
    timeout.tv_usec = 0;          // Set timeout to 0 microseconds for non-blocking behavior
    FD_ZERO(&rdfs);               // Clear the set of file descriptors
    FD_SET(STDIN_FILENO, &rdfs);  // Add standard input (keyboard) to the set of file descriptors to be monitored
    select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &timeout); // Monitor the file descriptors for reading with the specified timeout

    return FD_ISSET(STDIN_FILENO, &rdfs); // Return 1 if standard input is ready for reading (a key has been hit), otherwise return 0
}

long long GetTimeMS(void) {
	struct timeb time;      // Structure to hold time information
	ftime(&time);           // Get the current time and store it in the time structure
	return (long long)time.time * 1000 + time.millitm;  // Calculate and return the current time in milliseconds by combining seconds and milliseconds
}

// Function to delay execution for a specified number of milliseconds
void DelayMS(long long ms) {        // Get the current time in milliseconds at the start of the delay
	long long start = GetTimeMS();  // Loop until the specified number of milliseconds has passed or a key is hit

	// The loop checks the elapsed time by comparing the current time with the start time. If a key is hit during the delay, it breaks out of the loop to allow for immediate response to user input.
    while (GetTimeMS() - start < ms) {
		if (kbhit())        // If a key is hit during the delay, break out of the loop to allow for immediate response to user input
			break;          // If no key is hit, the loop continues until the specified delay time has elapsed
    }
}
