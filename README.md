# Trampa is a WIP at present.

# What is Trampa
Trampa is a midi controller application.
It is written in C++, utilises FreeRTOS and runs on the Pico W microprocessor.

# Hardware
This repo is configured to use the Picoprobe debugging system.  Details of picoprobe can be found [here](https://github.com/raspberrypi/picoprobe#readme).


# Software

## Building Trampa
Trampa uses the following external libraries.  They need to be downloaded to allow successful building of the Trampa codebase.

### Raspberry Pico SDK
git clone https://github.com/RaspberryPi/pico-sdk --recurse-submodules

### FreeRTOS
git clone -b smp https://github.com/FreeRTOS/FreeRTOS-Kernel --recurse-submodules

# Work TBD
- [ ] Complete design & coding of midi message 'bank' configuration (JSON baased)
- [ ] Add EEPROM storage for bank configurations and the code to utilise it
- [ ] Remove midi note generation thread
- [ ] Implement paired button press detection to switch midi configuration bank
- [ ] Incorporate OLED displays
- [ ] Add support for wifi hotspot/connectivity
- [ ] Add support for uploading midi bank configuration via http
- [ ] Add support for simple web based configuration of system