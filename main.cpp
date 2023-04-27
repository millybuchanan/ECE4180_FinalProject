// ECE 4180 Final Project Spring 2023
// Milly Buchanan, LT Ejiogu, Charu Malhotra, Zach Smith
// Line Following Robot
// Uses magnetic heading from 9-DOF to make 90 degree and 180 degree sweeps

// Included Libraries
#include "mbed.h"
#include "Motor.h"
#include "LSM9DS1.h"
#include "uLCD_4DGL.h"
#include <math.h>

// Definitions
#define PI 3.14159
#define DECLINATION -4.94 // Declination for Atlanta, GA
#define ON_LINE 1
#define OFF_LINE 0

// Peripheral Declarations
Serial pc(USBTX, USBRX);

Motor leftMotor(p23, p6, p5); // pwm, fwd, rev
Motor rightMotor(p24, p26, p25); // pwm, fwd, rev

DigitalIn IRsensor(p12);

BusOut leds(LED1, LED2, LED3, LED4);

LSM9DS1 IMU(p28, p27, 0xD6, 0x3C);

uLCD_4DGL uLCD(p9, p10, p11); 

// Global variables
//volatile int scanCount = 0;
volatile float magHeading; 
volatile bool scanSuccessful = false;

// Global functions
float getMagneticHeading()
{
    IMU.readMag();
    float mx = IMU.mx;
    float my = IMU.my;
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

void lineComplete()
{
    while (1)
    {
        // play a little song
        // flash LCD screen
        for (int i = 0; i < 16; i++)
        {
            leds = i;
            wait(0.25f);
        }
    }
}

void scan()
{
    float offLineHeading = getMagneticHeading();
    float startScanHeading;

    // spin bot left 90 degrees to prepare for 180 degree scan
    while(fabs(offLineHeading - getMagneticHeading()) < 90.0f)
    {
        leftMotor.speed(-0.3f);
        rightMotor.speed(0.3f);
        wait(0.25);
    }
    
    // stop bot
    leftMotor.speed(0.0f);
    rightMotor.speed(0.0f);
    wait(1);


    startScanHeading = getMagneticHeading();
    // scan bot right 180 degrees to redetect line
    while((fabs(startScanHeading - getMagneticHeading()) < 180.0))
    {
        leftMotor.speed(0.3f);
        rightMotor.speed(0.3f);
        if (IRsensor.read())
        {
            scanSuccessful = true;
            break;
        }
        wait(0.2);
        IMU.readMag();
    }

    leftMotor.speed(0.0f);
    rightMotor.speed(0.0f);
    if (!scanSuccessful)
    {
        lineComplete();
    }
}

void straight()
{
    leftMotor.speed(0.2f);
    rightMotor.speed(0.2f);
}

int main() 
{
    // Setup and calibrationfor peripherals
    IRsensor.mode(PullUp);
    uLCD.display_control(PORTRAIT);
    uLCD.baudrate(9600);
    IMU.begin();
    IMU.calibrate(1);
    IMU.calibrateMag(0);
    if (!IMU.begin())
    {
        uLCD.printf("Failed to communicate with LSM9DS1. \n");
    }
    while (1)
    {
        while(IRsensor.read())
        {
            straight();
        }
        scan();
    }
}
