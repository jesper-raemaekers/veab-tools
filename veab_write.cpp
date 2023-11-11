#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

void printHelp()
{
    std::cout << "USAGE: veabwrite i2c-bus channel value" << std::endl;
    std::cout << "i2c-bus may be 1, 3, 4, 5 or 6" << std::endl;
    std::cout << "channel may be 1-4" << std::endl;
    std::cout << "value 0.0 - 10.0" << std::endl;
    std::cout << "Example: veabwrite 6 1 1.5" << std::endl;
}

void setVeab(const unsigned int bus, const unsigned int channel, const float value)
{
    const float resolution = 0.0024414; // 2.45mV/bit, from design doc
    const unsigned int dacValue = value / resolution; // we lose up to 2.45 mV here, but thats okay

    int file;
    char filename[20];

    snprintf(filename, 19, "/dev/i2c-%d", bus);
    file = open(filename, O_RDWR);
    if (file < 0) {
        std::cout << "opening " << filename << " failed" << std::endl;
        exit(1);
    }

    if (ioctl(file, I2C_SLAVE, 0x60) < 0) {
        std::cout << "opening device on address 0x60 failed" << std::endl;
        exit(1);
    }

    char buf[3];

    buf[0] = 0x40 + (channel - 1);
    buf[1] = 0x80 + ((dacValue >> 8) & 0xF);
    buf[2] = dacValue & 0xFF;
      
    if (write(file, buf, 3) != 3) {
        std::cout << "Writing to device failed" << std::endl;
        exit(1);
    }

}


int main(int argc, char** argv)
{
    unsigned int inputI2cBus = 0;
    unsigned int inputChannel = 0;
    float inputValue = 0;
    if(argc != 4)
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
            inputValue = std::atof(argv[3]);
        }
        catch (...){
            printHelp();
            return -1;
        }
        if(inputChannel < 1 || inputChannel > 4) {
            printHelp();
            return -1;
        }
        if(inputValue < 0.0 || inputChannel > 10.0) {
            printHelp();
            return -1;
        }

    }
    
    // at this point all arguments are in range
    setVeab(inputI2cBus, inputChannel, inputValue);
        
    return 0;
}