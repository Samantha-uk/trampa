# What is Trampa
Trampa is a midi controller application.
It is written in C++, utilises FreeRTOS and runs on the Pico W microprocessor.

#Hardware
This repo is configured to use the Picoprobe debugging system.  Details can be found here.


#Software

## Building Trampa
Trampa uses the following external libraries.  They need to be downloaded to allow successful building of the Trampa codebase.



### Raspberry Pico SDK
git clone https://github.com/RaspberryPi/pico-sdk --recurse-submodules

### FreeRTOS
git clone -b smp https://github.com/FreeRTOS/FreeRTOS-Kernel --recurse-submodules

