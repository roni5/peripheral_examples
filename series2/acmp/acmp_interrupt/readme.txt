acmp_interrupt

This project demonstrates using the analog comparator (ACMP) peripheral to 
compare an analog input to an internal reference. In this example, ACMP pos input
is routed to a GPIO input pin and ACMP neg input is routed to the 1.25V internal reference.
When the voltage on ACMP pos input is lower than 1.25V, the ACMP output pin is low (GND).
When the voltage on ACMP pos input is higher than 1.25V, the ACMP output pin is high (IOVDD).
Rising and falling interrupts are enabled for the ACMP output. The MCU core is in sleep mode
in EM3, but the analog comparator remains functional. When the ACMP output transitions, the MCU
wakes from EM3 to EM0 to handle the interrupt. An interrupt handler checks if the ACMP interrupt
was for a rising or falling edge of the ACMP output, and sets or clears a GPIO output pin 
accordingly before returning to EM3.

How To Test (EFR32xG21):
1.  Open Simplicity Studio and update the kit's firmware from the Simplicity Launcher (if necessary)
2.  Build the project and download to the Starter Kit
3.  In Project Explorer, right click on the main project directory
4.  In the drop-down menu, select "Profile As Simplicity Energy Profiler Target"
5.  The project will compile, load and start-up, proceeding to the main loop which waits in EM3
6.  Observe the current consumption in Energy Profiler of the kit in EM3 - typically ~15uA
7.  Press and hold push button PB1
8.  Observe the spike in current as the MCU transitions to EM0 and then back to EM3
9.  Observe the marginally higher current consumption with PB1 is pressed - typically a few
    uA higher than observed in step 6 (current draw through push button pull-up resistor)
10. Release PB1 and observe again the spike in current as the MCU wakes, handles the interrupt
    and returns to EM3.
11. Toggle PB1
12. Observe the interrupt driven GPIO using an oscilloscope
13. Connect an external supply to ACMP pos input GPIO (with voltage <= IOVDD 3.3V)
14. Repeat steps 7 thru 12, but instead of using PB1, vary the external supply voltage above
    and below 1.25V
    
How To Test (EFR32xG23, EFR32xG24, and EFR32xG25):   
1.  Open Simplicity Studio and update the kit's firmware from the Simplicity Launcher (if necessary)
2.  Build the project and download to the Starter Kit
3.  Flash the firmware hex or binary to the target device
4.  Apply a voltage source or waveform to the ACMP input pin and observe LED/GPIO
    status indicator using an osciloscope
    (LED ON/pin high when Vin > Vref, LED OFF/pin low when Vin < Vref)
    
    NOTE: Although push button 0 can be used to supply an input signal to the ACMP,
          mechanical button bounce may cause extra rising and/or falling edges 
          on the ACMP input, resulting in confusing or undefined behavior on the 
          output indicator pin.  For best results, please use a clean input signal 
          to the ACMP positive input. 
 

Peripherals Used:
ACMP         - Full input range from 0 to Vdd
               Low accuracy mode, less current usage
               Hysteresis disabled
               Output 0 when ACMP is inactive
               VREFDIV set to maximum to disable divide

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PA06 - Interrupt driven GPIO, Expansion Header Pin 14, WSTK Pin 11
PD03 - ACMP positive input, push button PB1, Expansion Header Pin 9, WSTK Pin 6

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB01 - ACMP positive input, push button PB0
PB02 - LED0, WSTK Pin P17

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB01 - ACMP positive input, push button PB0, WSTK Pin 17
PB02 - LED0, WSTK Pin 19

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB01 - ACMP positive input, push button PB1, WSTK Pin 21
PC06 - LED0
