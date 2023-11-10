#include <iostream>
#include <string>
#include <cstring>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <iomanip>


void printHelp()
{
    std::cout << "USAGE: veabled i2c-bus color" << std::endl;
    std::cout << "i2c-bus may be 1, 3, 4, 5 or 6" << std::endl;
    std::cout << "channel may be red green blue" << std::endl;
    std::cout << "Example: veabled 6 red" << std::endl;
}

void setLed(const unsigned int bus, const unsigned int color)
{
    int file;
    char filename[20];

    snprintf(filename, 19, "/dev/i2c-%d", bus);
    file = open(filename, O_RDWR);
    if (file < 0) {
        std::cout << "opening " << filename << " failed" << std::endl;
        exit(1);
    }

    if (ioctl(file, I2C_SLAVE, 0x10) < 0) {
        std::cout << "opening device on address 0x10 failed" << std::endl;
        exit(1);
    }

    char buf[3];

    // setting the IO correct, first 4 analog inputs, others digital
    buf[0] = 0x08;
    buf[1] = 0x05;
    buf[2] = 0xF0;

    if (write(file, buf, 3) != 3) {
        std::cout << "Writing IO config failed" << std::endl;
    }

    // setting GPIO 4, 5, 6 as digital outputs
    buf[0] = 0x08;
    buf[1] = 0x07;
    buf[2] = 0x70;

    if (write(file, buf, 3) != 3) {
        std::cout << "Writing IO config failed (2)" << std::endl;
    }

    // setting LED pins
    buf[0] = 0x08;
    buf[1] = 0x0B;
    buf[2] = 0x00;

    switch(color)
    {
        case 1:
            buf[2] = 0x60;
            break;
        case 2:
            buf[2] = 0x50;
            break;
        case 3:
            buf[2] = 0x30;;
            break;
    }

    if (write(file, buf, 3) != 3) {
        std::cout << "Writing color failed" << std::endl;
    }

}


int main(int argc, char** argv)
{
    unsigned int inputI2cBus = 0;
    unsigned int inputColor = 0;
    if(argc != 3)
    {
        printHelp();
        return -1;
    }
    else
    {
        // number ok, now try and parse the numbers
        try{
            inputI2cBus = std::stoi(argv[1]);
        }
        catch (...){
            printHelp();
            return -1;
        }
        if(strcmp(argv[2], "red") == 0) {
            inputColor = 1;
        }
        if(strcmp(argv[2], "green") == 0) {
            inputColor = 2;
        }
        if(strcmp(argv[2], "blue") == 0) {
            inputColor = 3;
        }
        if(inputColor == 0)
        {
            printHelp();
            return -1;
        }
    }
    
    // at this point all arguments are in range
    setLed(inputI2cBus, inputColor);
        
    return 0;
}