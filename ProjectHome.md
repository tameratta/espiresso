### This code has been migrated to GitHub and development will continue there ###

Source code for semi-automatic control of a Gaggia Classic Espresso machine by a Raspberry Pi. Includes temperature regulation, pump and solenoid on/off control, PWM pump modulation, water flow volume measurement, ultrasonic ranger for water level measurement, LCD display of temperature, pressure sensor display, push-button controls and logging of temperature, pressure, water volume dispensed, shot number and time data to CSV files.

This has been redesigned to support new hardware added to the machine. The last working revision corresponding to the old hardware as described on the blog is [R53](https://code.google.com/p/espiresso/source/detail?r=53). Updates beyond that point are for new hardware, which added pressure sensing and other features.

[R53](https://code.google.com/p/espiresso/source/detail?r=53)
  * Tried and tested version for original hardware (as described on blog)
  * Uses kernel drivers for TSIC and HCSR-04
[R87](https://code.google.com/p/espiresso/source/detail?r=87)
  * Current working version for new hardware (not yet fully documented)
  * All existing devices are working
  * Using PIGPIOD for TSIC, HCSR, Flow sensor and other GPIO
  * Hardware PWM still using low level registers
  * Support for ADS1015 ADC, used to read pressure sensor and buttons
  * Support for PWM modulation of Ulka pump
  * Improved user interface with icons to show boiler power and pump activity
  * Shot timer
  * More detailed logging to file