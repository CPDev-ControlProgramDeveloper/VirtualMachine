# CPDev Virtual Machine

The repository contains public sources of the CPDev (Control Program Developer) virtual machine runtime (VM).
The environment is available for download at https://cpdev.kia.prz.edu.pl 

# Arduino and ESP32 library

To use CPDev VM with Arduino use the included CPDevVM.zip or create the CPDevVM.zip file by yourself from the CPDevVM folder.
In the Arduino IDE, navigate to Sketch -> Include Library -> Add .ZIP Library and use CPDevVM.zip.

Refer to the SampleArduino or SampleESP32 to learn how to invoke the library functions.

Note that the smallest Arduino boards (i.e. Uno) provide only the limited functionality of the VM due to the ROM shortage.
