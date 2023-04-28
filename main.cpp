#include "mbed.h"
#include "Motor.h"
#include "uLCD_4DGL.h"

Serial pc(USBTX, USBRX);

uLCD_4DGL uLCD(p9, p10, p11);

DigitalIn sensor0(p13);
DigitalIn sensor1(p14);
DigitalIn sensor2(p15);
DigitalIn sensor3(p16);
DigitalIn sensor4(p17);
DigitalIn sensor5(p18);
DigitalIn sensor6(p19);
DigitalIn sensor7(p20);

BusOut leds(LED1, LED2, LED3, LED4);

#define TAPE 1
#define BKGD 0

Motor mR(p23, p6, p5); // pwm, fwd, rev
Motor mL(p24, p26, p25); // pwm, fwd, rev

void goStraight()
{
    mR.speed(0.2f); 
    mL.speed(0.2f);
    wait(0.25);
}

void turnRight()
{
    while (1)
    {
        //pc.printf("Turning Right\n\n");
        mR.speed(-0.2f); 
        mL.speed(0.2f);
        wait(0.25);

        if(sensor6.read() != TAPE && sensor7.read() != TAPE)
        {
            break;
        }
    }
}

void turnLeft()
{
    while (1)
    {
    //pc.printf("Turning Left\n\n");
        mR.speed(0.2f); 
        mL.speed(-0.2f);
        wait(0.25);

        if(sensor0.read() != TAPE && sensor1.read() != TAPE)
        {
            break;
        }
    }
}

void exitSequence()
{
    while (1)
    {
        for (int i = 0; i < 16; i++)
        {
            leds = i;
        }
    }
}

int main() {
    sensor0.mode(PullUp);
    sensor1.mode(PullUp);
    sensor2.mode(PullUp);
    sensor3.mode(PullUp);
    sensor4.mode(PullUp);
    sensor5.mode(PullUp);
    sensor6.mode(PullUp);
    sensor7.mode(PullUp);

    uLCD.baudrate(9600);
    uLCD.printf("TAPE following robot beginning in...\n");
    wait(1);
    uLCD.printf("3...\n");
    wait(1);
    uLCD.printf("2...\n");
    wait(1);
    uLCD.printf("1...\n");
    wait(1);
    uLCD.cls();
    uLCD.printf("GO !!!!");
    wait(1);
    uLCD.cls();

    while(1)
    {
    //go straight if edge sensors are on background
        while (sensor2.read() == TAPE || sensor3.read() == TAPE ||sensor4.read() == TAPE || sensor5.read() == TAPE)
        {
            // maybe try this as a while loop?
            if (sensor6.read() == TAPE && sensor7.read() == TAPE)
            {
                uLCD.printf("Turning right!");
                mL.speed(0.0f);
                mR.speed(0.0f);
                turnRight();
            }
            uLCD.cls();

            // and this a while loop? 
            if (sensor0.read() == TAPE && sensor1.read() == TAPE)
            {
                uLCD.printf("Turning left!");
                mL.speed(0.0f);
                mR.speed(0.0f);
                turnLeft();
            }
            uLCD.cls();

            goStraight();
            uLCD.printf("Going Straight!");
            wait(0.25);
            uLCD.cls();
        }
        

        pc.printf("Stopping\n\n\n");
        mL.speed(0.0f);
        mR.speed(0.0f);
        break;
    }

    exitSequence();
}