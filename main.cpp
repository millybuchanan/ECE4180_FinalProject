#include "mbed.h"
#include "LSM9DS1.h"
#include "uLCD_4DGL.h"

// Calculates magnetic heading from 9-DOF and prints to LCD
#define PI 3.14159
// Earth's magnetic field varies by location. Add or subtract
// a declination to get a more accurate heading. Calculate
// your's here:
// http://www.ngdc.noaa.gov/geomag-web/#declination
#define DECLINATION -4.94 // Declination (degrees) in Atlanta,GA.

LSM9DS1 IMU(p28, p27, 0xD6, 0x3C);
uLCD_4DGL uLCD(p9, p10, p11); 
DigitalOut myled(LED1);
Serial pc(USBTX, USBRX);
// Calculate pitch, roll, and heading.
// Pitch/roll calculations taken from this app note:
// http://cache.freescale.com/files/sensors/doc/app_note/AN3461.pdf?fpsp=1
// Heading calculations taken from this app note:
// http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/Magnetic__Literature_Application_notes-documents/AN203_Compass_Heading_Using_Magnetometers.pdf
float getMagneticHeading(float mx, float my)
{
    uLCD.cls();
    // touchy trig stuff to use arctan to get compass heading (scale is 0..360)
    mx = -mx;
    float heading;
    if (my == 0.0)
    {
        heading = (mx < 0.0) ? 180.0 : 0.0;
    } else
    {
        heading = atan2(mx, my)*360.0/(2.0*PI);
    }

    heading -= DECLINATION; //correct for geo location
    if(heading>180.0) 
    {
        heading = heading - 360.0;
    } else if(heading<-180.0) 
    {
        heading = 360.0 + heading;
    } else if(heading<0.0) 
    {
        heading = 360.0  + heading;
    }

    uLCD.printf("Magnetic Heading: %f degress\n\r",heading);
    return heading;
}




int main()
{
    uLCD.display_control(PORTRAIT);
    uLCD.baudrate(9600);
    uLCD.printf("Begin...");
    wait(5);
    uLCD.cls();
    IMU.begin();
    if (!IMU.begin()) {
        uLCD.printf("Failed to communicate with LSM9DS1.\n");
    }
    IMU.calibrate(1);
    IMU.calibrateMag(0);
    while(1) {
        IMU.readMag();
        getMagneticHeading(IMU.mx, IMU.my);
        myled = 1;
        wait(0.5);
        myled = 0;
        wait(0.5);
    }
}

