#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <iomanip>

void printHelp()
{
    std::cout << "USAGE: veabread i2c-bus channel" << std::endl;
    std::cout << "i2c-bus may be 1, 3, 4, 5 or 6" << std::endl;
    std::cout << "channel may be 1-4" << std::endl;
    std::cout << "Example: veabread 6 1" << std::endl;
}

void getVeab(const unsigned int bus, const unsigned int channel)
{
    const float resolution = 0.00245117; // 2.45mV/bit, from design doc
    unsigned int adcReadValue = 0;
    float adcResult = 0.0;
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

    // setting the IO correct, first 4 analog inputs
    buf[0] = 0x08;
    buf[1] = 0x05;
    buf[2] = 0xF0;

    if (write(file, buf, 3) != 3) {
        std::cout << "Writing IO config failed" << std::endl;
        exit(1);
    }

    buf[0] = 0x08;
    buf[1] = 0x11;
    buf[2] = channel - 1;

    if (write(file, buf, 3) != 3) {
        std::cout << "Writing next channel failed" << std::endl;
        exit(1);
    }

    if (read(file, buf, 2) != 2) {
        std::cout << "Read channel sample failef" << std::endl;
        exit(1);
    }

    // bits 15-4 contain the data

    adcReadValue = buf[0];
    adcReadValue <<= 4;
    adcReadValue |= (buf[1] >> 4) & 0xF;

    adcResult = adcReadValue * resolution;
    std::cout << std::setprecision(3) << adcResult << std::endl;
}


int main(int argc, char** argv)
{
    unsigned int inputI2cBus = 0;
    unsigned int inputChannel = 0;
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
            inputChannel = std::stoi(argv[2]);
        }
        catch (...){
            printHelp();
            return -1;
        }
        if(inputChannel < 1 || inputChannel > 4) {
            printHelp();
            return -1;
        }
    }
    
    // at this point all arguments are in range
    getVeab(inputI2cBus, inputChannel);
        
    return 0;
}