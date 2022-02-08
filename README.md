# CPDev Virtual Machine

The repository contains public sources of the CPDev (Control Program Developer) virtual machine runtime (VM).
The environment is available for download at https://cpdev.kia.prz.edu.pl/instructions/ 

## Platforms

The CPDev virtual machine is portable to multiple platforms. The repository contains simple ports for selected targets, such as:
* Arduino AVR-based boards (e.g. Mega)
* Arduino Due (ARM)
* Beaglebone AI (Linux OS)
* ESP32-based boards (FreeRTOS)
* M5Stack (with Azure MQTT communication)
* STM32 (uses timer interrupt for timing accuracy)
* Small Device C Compiler (Z80, 8051, 68HC08, STM8, etc.)
* CC65 (6502/65C02)
* others.

The ports are implemented in C ++ where possible, or in C in other cases. Some platforms require limited functionality due to memory shortage. Enabling or disabling these functions is done using the vm_cfg.h file.

Note that since CPDev VM is not based on any particular communication standard, you will have to implement the one that suits your needs. See the M5StackAzure folder for an example of the MQTT protocol usage.

## Custom target platforms

To implement a virtual machine on a specific target platform, please refer to the accompanying document about [porting](porting.md).
