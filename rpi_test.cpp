#include <stdio.h>
#include <string>
#include <iostream>
#include <pigpio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <memory.h>

#define DESTINATION_ADDR 0x13
#define SLAVE_ADDR 0x13

int init_slave(bsc_xfer_t &xfer, int addr) {

	gpioSetMode(18, PI_ALT3);
	gpioSetMode(19, PI_ALT3);
	xfer.control = (addr<<16) | 0x305;

	return bscXfer(&xfer);
} 


int close_slave(bsc_xfer_t &xfer) {

	xfer.control = 0;	
	return bscXfer(&xfer);
}

void master_gpio(){

    char message[] = "Hello World";
    int handle = i2cOpen(1, DESTINATION_ADDR, 0);
    //std::cout << "Handle: " << handle << std::endl;
    i2cWriteDevice(handle, message, 12);
    gpioDelay(20000);
    i2cClose(handle);
}

void slave_gpio(){

    bsc_xfer_t xfer; //http://abyz.me.uk/rpi/pigpio/cif.html#bscXfer

    //SLAVE (DESTINATION_ADDR = SLAVE_ADDR)
    int status = init_slave(xfer, SLAVE_ADDR);

    xfer.txCnt = 12;
    status = bscXfer(&xfer);
    if (status < 0){printf("Error 2\n");return;}
    printf("Received %d bytes\n", xfer.txCnt); // 1 char = 1 byte

    //strcpy(xfer.rxBuf, "ABCD");
    char read_var[xfer.rxCnt];
    read_var = xfer.rxBuf;
    std::cout << "rxBuf = " << read_var << std::endl; 
    //xfer.txCnt = 4;

    for (int j = 0; j < xfer.rxCnt; j++) // Print bytes received in rxBuf
        printf("%c", xfer.rxBuf[j]);

    status = close_slave(xfer); // Close slave
}

int main(int argc, char *argv[]) {

	int key = 0;
	if (gpioInitialise() < 0) {printf("Error 1\n"); return 1;}

    while(key != 'q') {
		
        master_gpio();
        slave_gpio();   

		printf("Press q to quit. Press any other key to send a hello message.\n");
		key = getchar();
	}

	gpioTerminate();
	return 0;
}




