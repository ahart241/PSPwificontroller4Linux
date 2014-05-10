/*
 * Author: Andrew Hart <ahart241@gmail.com>
 */
 
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>

int send_event(int fd, __u16 type, __u16 code, __s32 value)
{
    struct input_event event;

    memset(&event, 0, sizeof(event));
    event.type = type;
    event.code = code;
    event.value = value;

    if (write(fd, &event, sizeof(event)) != sizeof(event)) {
        fprintf(stderr, "Error on send_event");
        return -1;
    }

    return 0;
}


#define BUFFSIZE 11
void Die(char *mess) { perror(mess); exit(1); }


int main(int argc, char *argv[]) {

    /* Declarations for uinput */
    int i, j, fd;
    struct uinput_user_dev device;
    memset(&device, 0, sizeof device);

    /* Declarations for UDP socket */
    int sock;
    struct sockaddr_in wifiserver;
    struct sockaddr_in wificlient;
    unsigned char buffer[BUFFSIZE];
    unsigned int clientlen, serverlen;
    int received = 0;

    /* Check arguments and print usage */
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s <port>\n", argv[0]);
        fprintf(stderr, "\n\t20001 -> Controller 1\n\t20002 -> Controller 2\n\t....\n", argv[0]);
        exit(1);
    }

    /*Open uinput device and setup map */
    fd=open("/dev/uinput",O_RDWR);
    strcpy(device.name,"wificontoller");

    device.id.bustype=BUS_USB;
    device.id.vendor=1;
    device.id.product=1;
    device.id.version=1;

    for (i=0; i < ABS_MAX; i++) {
        device.absmax[i] = -1;
        device.absmin[i] = -1;
        device.absfuzz[i] = -1;
        device.absflat[i] = -1;
    }

    for (j=0; j < ABS_MAX; j++) {
        device.absmax[j] = -1;
        device.absmin[j] = -1;
        device.absfuzz[j] = -1;
        device.absflat[j] = -1;
    }

    device.absmin[ABS_X]=0;
    device.absmax[ABS_X]=255;
    device.absfuzz[ABS_X]=0;
    device.absflat[ABS_X]=0;

    device.absmin[ABS_Y]=0;
    device.absmax[ABS_Y]=255;
    device.absfuzz[ABS_Y]=0;
    device.absflat[ABS_Y]=0;

    if (write(fd,&device,sizeof(device)) != sizeof(device))
    {
        fprintf(stderr, "error setup\n");
    }

    if (ioctl(fd,UI_SET_EVBIT,EV_KEY) < 0)
        fprintf(stderr, "error evbit key\n");

    if (ioctl(fd,UI_SET_KEYBIT, BTN_1) < 0)
        fprintf(stderr, "error evbit button0\n");

    if (ioctl(fd,UI_SET_KEYBIT, BTN_2) < 0)
        fprintf(stderr, "error evbit button1\n");

    if (ioctl(fd,UI_SET_KEYBIT, BTN_3) < 0)
        fprintf(stderr, "error evbit button2\n");

    if (ioctl(fd,UI_SET_KEYBIT, BTN_4) < 0)
        fprintf(stderr, "error evbit button3\n");

    if (ioctl(fd,UI_SET_KEYBIT, BTN_5) < 0)
        fprintf(stderr, "error evbit button4\n");

    if (ioctl(fd,UI_SET_KEYBIT, BTN_6) < 0)
        fprintf(stderr, "error evbit button5\n");

    if (ioctl(fd,UI_SET_KEYBIT, BTN_SELECT) < 0)
        fprintf(stderr, "error evbit button6\n");

    if (ioctl(fd,UI_SET_KEYBIT, BTN_START) < 0)
        fprintf(stderr, "error evbit button7\n");

    if (ioctl(fd,UI_SET_KEYBIT, BTN_FORWARD) < 0)
        fprintf(stderr, "error evbit button8\n");

    if (ioctl(fd,UI_SET_KEYBIT, BTN_BACK) < 0)
        fprintf(stderr, "error evbit button9\n");

    if (ioctl(fd,UI_SET_KEYBIT, BTN_LEFT) < 0)
        fprintf(stderr, "error evbit button10\n");

    if (ioctl(fd,UI_SET_KEYBIT, BTN_RIGHT) < 0)
        fprintf(stderr, "error evbit button11\n");


    if (ioctl(fd,UI_SET_EVBIT,EV_ABS) < 0)
            fprintf(stderr, "error evbit absolute\n");

    for(i=ABS_X;i<ABS_MAX;i++)
        if (ioctl(fd,UI_SET_ABSBIT,i) < 0)
            fprintf(stderr, "error set absolute bit %d\n", i);

    for(j=ABS_Y;j<ABS_MAX;j++)
        if (ioctl(fd,UI_SET_ABSBIT,j) < 0)
            fprintf(stderr, "error set absolute bit %d\n", j);

    if (ioctl(fd,UI_DEV_CREATE) < 0)
    {
        fprintf(stderr, "error create\n");
    }


    /* Create the UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        Die("Failed to create socket");
    }

    /* Construct the server sockaddr_in structure */
    memset(&wifiserver, 0, sizeof(wifiserver));       /* Clear struct */
    wifiserver.sin_family = AF_INET;                  /* Internet/IP */
    wifiserver.sin_addr.s_addr = htonl(INADDR_ANY);   /* Any IP address */
    wifiserver.sin_port = htons(atoi(argv[1]));       /* server port */

    /* Bind the socket */
    serverlen = sizeof(wifiserver);
    if (bind(sock, (struct sockaddr *) &wifiserver, serverlen) < 0) {
        Die("Failed to bind server socket");
    }



    /* Run until cancelled */
    while (1) {
        /* Receive a message from the client */
        clientlen = sizeof(wificlient);
        if ((received = recvfrom(sock, buffer, BUFFSIZE, 0,
                        (struct sockaddr *) &wificlient,
                        &clientlen)) < 0) {
            Die("Failed to receive message");
        }

        //fprintf(stderr,
        //        "Client connected: %s\t\t", inet_ntoa(wificlient.sin_addr));
        //fprintf(stderr,
        //        "1: %i\t",  buffer[1]);
        //fprintf(stderr,
        //        "packets: %i\t",  buffer[2]);
        //fprintf(stderr,
        //        "counter: %i\t",  buffer[3]);
        //fprintf(stderr,
        //        "4: %i\t",  buffer[4]);
        //fprintf(stderr,
        //        "5: %i\t",  buffer[5]);

        //fprintf(stderr,
        //        "Rel X: %i\t",  buffer[6]);
        send_event(fd, EV_ABS, ABS_X, buffer[6]);
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "Rel Y: %i\t",  buffer[7]);
        send_event(fd, EV_ABS, ABS_Y, buffer[7]);
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "DPad UP:  %i\t", (1 == ((buffer [8] >> 4) & 1)));
        send_event(fd, EV_KEY, BTN_FORWARD, (1 == ((buffer [8] >> 4) & 1)));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "DPad DOWN:  %i\t", (1 == ((buffer [8] >> 6) & 1)));
        send_event(fd, EV_KEY, BTN_BACK, (1 == ((buffer [8] >> 6) & 1)));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "DPad LEFT:  %i\t",  (1 == ((buffer [8] >> 7) & 1)));
        send_event(fd, EV_KEY, BTN_LEFT, (1 == ((buffer [8] >> 7) & 1)));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "DPad RIGHT:  %i\n", (1 == ((buffer [8] >> 5) & 1)));
        send_event(fd, EV_KEY, BTN_RIGHT, (1 == ((buffer [8] >> 5) & 1)));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "Select:  %i\t", (1 == ((buffer [8] >> 0) & 1)));
        send_event(fd, EV_KEY, BTN_SELECT, (1 == ((buffer [8] >> 0) & 1)));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "Start:  %i\t", (1 == ((buffer [8] >> 3) & 1)));
        send_event(fd, EV_KEY, BTN_START, (1 == ((buffer [8] >> 3) & 1)));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "Btn 1:  %i\t", (1 == ((buffer [9] >> 6) & 1)));
        send_event(fd, EV_KEY, BTN_1, (1 == ((buffer [9] >> 6) & 1)));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "Btn 2:  %i\t", (1 == ((buffer [9] >> 5) & 1)));
        send_event(fd, EV_KEY, BTN_2, (1 == ((buffer [9] >> 5) & 1)));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "Btn 3:  %i\t", (1 == ((buffer [9] >> 7) & 1)));
        send_event(fd, EV_KEY, BTN_3, (1 == ((buffer [9] >> 7) & 1)));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "Btn 4:  %i\t", (1 == ((buffer [9] >> 4) & 1)));
        send_event(fd, EV_KEY, BTN_4, (1 == ((buffer [9] >> 4) & 1)));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "Btn 5:  %i\t", (1 == ((buffer [9] >> 0) & 1)));
        send_event(fd, EV_KEY, BTN_5, (1 == ((buffer [9] >> 0) & 1)));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "Btn 6:  %i\n", (1 == ((buffer [9] >> 1) & 1)));
        send_event(fd, EV_KEY, BTN_6, (1 == ((buffer [9] >> 1) & 1)));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        //fprintf(stderr,
        //        "Btns:  %i\t",  buffer[9]);
        //fprintf(stderr,
        //        "10: %i\t",  buffer[10]);
        //fprintf(stderr,
        //        "11: %i\n",  buffer[11]);

    }
}

