/* File: bitOps.c
 * Author: Denis Myer
 * Last Edited: Gary Hubley
 * Date: 2026/02/04
 * Description: DEMONSTRATION OF BOOLEAN AND SHIFT, BIT OPERATIONS
 */
#include <linux/kd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
void changemode(int mode);
int kbhit(void);
long long GetTimeMS(void);
void DelayMS(long long);

int main(void) {
  int bitmode = 1;
  char ch;
  int delayTimeMS = 500;
  changemode(1);
  while (ch != 'q' || ch != 'Q') {
    while (!kbhit()) {
      DelayMS(500);
      if (ch == 'l') {

        bitmode = bitmode << 1;

        if (bitmode == 8) {
          bitmode = 1;
        }
        ioctl(1, KDSETLED, bitmode);
      }

      else if (ch == 'r') {
        bitmode = bitmode >> 1;
        if (bitmode == 0) {
          bitmode = 4;
        }

        ioctl(1, KDSETLED, bitmode);
      }
    }

    ch = getchar();

    if (ch == 'l' || ch == 'L') {
      printf("\nL\n");
    } else if (ch == 'r' || ch == 'R') {
      printf("\nR\n");
    } else if (ch == 'q' || ch == 'Q') {
      printf("\nQuit\n");
      changemode(0);
      return 0;
    }

    /*
      unsigned char A, B, C, D, E, F;
      A = 0x34;  // 0011 0100 //
      B = 0x27;  // 0010 0111 //
      C = A & B; // 0010 0100 //
      D = A | B; // 0011 0111 //
      E = A ^ B; // 0001 0011 //
      F = ~A;    // 1100 1011 //
      printf("BOOLEAN OPERATIONS\n");
      printf("A = (0011 0100) = %x\n", A);
      printf("B = (0010 0111) = %x\n", B);
      printf("A AND B (0010 0100) = %x\n", C);
      printf("A OR B (0011 0111) = %x\n", D);
      printf("A XOR B (0001 0011) = %x\n", E);
      printf("NOT A (1100 1011) = %x\n", F);
      printf("SHIFT OPERATIONS\n");
      A = 0x34;   // 0011 0100 //
      B = A << 1; // 0110 1000Z ZERO FILL //
      C = A >> 2; // 0000 1101 //
      printf("A = (0011 0100) = %x\n", A);
      printf("SHIFT LEFT ONE (0110 1000) = %x\n", B);
      printf("SHIFT RIGHT TWO (0000 1101) = %x\n", C);

      return (0);
    */
    ioctl(1, KDSETLED, bitmode);
  }
}

void changemode(int mode) {
  static struct termios oldt, newt;
  if (mode == 1) {
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  } else {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  }
}

int kbhit(void) {
  struct timeval timeout;
  fd_set rdfs;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  FD_ZERO(&rdfs);
  FD_SET(STDIN_FILENO, &rdfs);
  select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &timeout);
  return FD_ISSET(STDIN_FILENO, &rdfs);
}

long long GetTimeMS(void) {
  struct timeb time;
  ftime(&time);
  return (long long)time.time * 1000 + time.millitm;
}

void DelayMS(long long ms) {
  long long start = GetTimeMS();

  while (GetTimeMS() - start < ms) {
  }
}
