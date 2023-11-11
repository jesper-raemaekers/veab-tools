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
    std::cout << "usage: veabswitch BUS" << std::endl << std::endl;
    std::cout << "BUS: 1, 3, 4, 5 or 6" << std::endl;
    std::cout << "Example: veabswitch 6" << std::endl;
}

void getSw(const unsigned int bus)
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
        exit(1);
    }

    // setting GPIO 4, 5, 6 as digital outputs
    buf[0] = 0x08;
    buf[1] = 0x07;
    buf[2] = 0x70;

    if (write(file, buf, 3) != 3) {
        std::cout << "Writing IO config failed (2)" << std::endl;
        exit(1);
    }

    // prepare reading from address
    buf[0] = 0x10;
    buf[1] = 0x0D;

    if (write(file, buf, 2) != 2) {
        std::cout << "Writing read address failed" << std::endl;
        exit(1);
    }

    if (read(file, buf, 1) != 1) {
        std::cout << "Read data failed" << std::endl;
        exit(1);
    }

    if(buf[0] & 0x80)
    {
        std::cout << "Switch on" << std::endl;
    }
    else
    {
        std::cout << "Switch off" << std::endl;
    }



}


int main(int argc, char** argv)
{
    unsigned int inputI2cBus = 0;
    if(argc != 2)
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
    }
    
    // at this point all arguments are in range
    getSw(inputI2cBus);
        
    return 0;
}