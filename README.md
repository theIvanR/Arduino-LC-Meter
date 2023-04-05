# Self-contained Arduino LC Meter with Internal Calibration  

# Introduction:
When I set out to measure inductances using an Arduino, I found that existing methods were either overcomplicated (requiring an external shield) or cumbersome and inaccurate (using an external comparator). Additionally, these methods were limited in their ability to measure capacitances within a wide range.

To solve this problem, I decided to create my own LC meter using an Arduino and only passive external components. By combining the "Two Pin" capacitance tester with RobotZer0's idea of using the internal comparator, I was able to develop a truly self-contained LC meter with internal calibration.

# Features:
My code improves upon existing methods by adding rudimentary autoranging, internal calibration/zeroing of the test capacitance, low pass filtering, and integration averaging. These features improve the accuracy of the measurement by reducing statistical noise and increasing the precision by removing the offset component from capacitance measurement.

# Future Plans:
While I aim to keep this project as barebones as possible for easy implementation, I plan to create a second code file in the future that includes more advanced features. These may include more advanced reference calibration capability (utilizing newtonian or other polynomial weight functions which will be stored in the EEPROM and can be called by a smart calibration function) and support for LCDs.

# Instructions:
To wire the barebones configuration of the LC meter, connect the following pins:

    Pin 11: Pulse pin
    Pin 7: GND reference for internal comparator
    Pin 6: HIGH input for internal comparator
    Pin A5: Mode Selection pin
    Pin A2: Autoranging Capacitor Positive test pin
    Pin A0: Autoranging Capacitor Negative pin

