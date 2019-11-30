#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include "../src/tiny_prompt.h"
#include "../src/ascii_code.h"

//socat PTY,link=/dev/ttyS10 PTY,link=/dev/ttyS11 &

char *portname = "/dev/ttyS10";
int fd;

int set_interface_attribs(int fd, int speed) {
    struct termios tty;

    if( tcgetattr(fd, &tty) < 0 ) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t) speed);
    cfsetispeed(&tty, (speed_t) speed);

    tty.c_cflag |= (CLOCAL | CREAD); /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8; /* 8-bit characters */
    tty.c_cflag &= ~PARENB; /* no parity bit */
    tty.c_cflag &= ~CSTOPB; /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS; /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL
            | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if( tcsetattr(fd, TCSANOW, &tty) != 0 ) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void set_mincount(int fd, int mcount) {
    struct termios tty;

    if( tcgetattr(fd, &tty) < 0 ) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5; /* half second timer */

    if( tcsetattr(fd, TCSANOW, &tty) < 0 )
        printf("Error tcsetattr: %s\n", strerror(errno));
}



int main() {

    unsigned char serial_buf[1];
    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if( fd < 0 ) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }

    // Baudrate 115200, 8 bits, no parity, 1 stop bit
    set_interface_attribs(fd, B115200);
    //set_mincount(fd, 0);                /* set to pure timed read */
    tiny_prompt_init(fd);

    do {
        // reading serial byte by bytr
        if( read(fd, serial_buf, 1) > 0 ) {
            tiny_prompt_execute((const unsigned char*) serial_buf);
        } else {
            printf("Error from read: %s\n", strerror(errno));
            printf("Timeout from read\n");
        }
        /* repeat read to get full message */
    } while (1);

}
