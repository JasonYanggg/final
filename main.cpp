#include "mbed.h"
#include "bbcar.h"

Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin8(D8), pin9(D9);
DigitalIn button(SW2);
DigitalIn pin3(D3);
DigitalOut led1(LED1);
DigitalInOut pin10(D10);
Serial uart(D1, D0);
Serial xbee(D12, D11);

BBCar car(pin8, pin9, servo_ticker);
parallax_ping ping1(pin10);
parallax_encoder encoder0(pin3, encoder_ticker);

void go_cm(double speed, double dist)
{
    encoder0.reset();
    car.goStraightCalib(speed);
    while (encoder0.get_cm() < dist) {
        wait_ms(50);
    }
    car.stop();
}

void rotate_90(double speed, int dir)
{
    car.rotate(speed, dir);
    wait(1.5);
    car.stop();
}

int main()
{
    double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table0[] = {-16.348, -16.030, -15.152, -12.281, -6.380, 0.000, 5.503, 11.563, 14.753, 15.950, 16.428};
    double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table1[] = {-15.790, -15.392, -14.514, -11.643, -5.582, 0.000, 6.459, 12.201, 14.913, 15.870, 16.349};

    // first and fourth argument: length of table
    car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);

    while (button == 1);

    xbee.printf("walk straight\n");
    car.goStraightCalib(15);
    while ((float)ping1 > 18) {
        led1 = 1;
        wait(0.01);
    }
    led1 = 0;    
    car.stop();
    wait(1);

    char s[21];
    // float deg = 0;
    // while (deg > 181 || deg < 179) {
    //     sprintf(s, "degree");
    //     uart.puts(s);
    //     int i = 0;
    //     while (1) {
    //         if (uart.readable()) {
    //             char recv = uart.getc();
    //             if (recv == '\r') {
    //                 s[i] = '\0';
    //                 break;
    //             }
    //             s[i++] = recv;
    //         }
    //     }
    //     // printf("%s\r\n", s);
    //     deg = atof(s);
    //     if (deg > 181) {
    //         car.turn(3, -1);
    //         wait(1);
    //         car.stop();
    //     }
    //     else if (deg < 179) {
    //         car.turn(3, 1);
    //         wait(1);
    //         car.stop();
    //     }
    // }
    // printf("%f\r\n", deg);

    // turn left to enter mission 1
    wait(1);
    xbee.printf("turn left\n");
    rotate_90(6, -1);

    // go straight to the front of the parking space
    wait(1);
    xbee.printf("walk straight\n");
    go_cm(15, 55);
    
    // turn right 
    wait(1);
    xbee.printf("start reverse parking\n");
    rotate_90(6, 1);

    // reverse parking
    wait(1);
    go_cm(-15, 30);

    // go out of the parking space
    wait(1);
    go_cm(15, 12);

    // turn left
    wait(1);
    rotate_90(6, -1);

    // go straight
    wait(1);
    go_cm(10, 15);

    // turn right
    wait(1);
    rotate_90(6, 1);

    // image classification
    wait(1);
    xbee.printf("start image classification\n");
    sprintf(s, "image_classification");
    uart.puts(s);
    int i = 0;
    while (1) {
        if (uart.readable()) {
            char recv = uart.getc();
            if (recv == '\r') {
                s[i] = '\0';
                led1 = 1;
                wait(1);
                led1 = 0;
                break;
            }
            s[i++] = recv;
        }
    }
    xbee.printf("The result is \"%s\".\n", s);

    // turn right
    wait(1);
    xbee.printf("leave mission 1\n");
    rotate_90(6, 1);

    // leave mission 1
    wait(1);
    go_cm(15, 50);

    // go to mission 2
    wait(1);
    xbee.printf("walk straight\n");
    car.goStraightCalib(15);
    while ((float)ping1 > 18) {
        led1 = 1;
        wait(0.01);
    }
    led1 = 0;    
    car.stop();
    
    // turn right
    wait(1);
    xbee.printf("turn right\n");
    rotate_90(6, 1);

    // go straight
    wait(1);
    xbee.printf("enter mission 2\n");
    go_cm(15, 30);

    // turn right
    wait(1);
    xbee.printf("go to the corresponding position\n");
    rotate_90(6, 1);

    // go straight
    wait(1);
    go_cm(15, 25);

    // scan
    // wait(1);
    // xbee.printf("scan the object\n");

    // go back
    wait(1);
    xbee.printf("leave mission 2\n");
    go_cm(-15, 25);

    // turn left
    wait(1);
    rotate_90(6, -1);

    // go straight
    car.goStraightCalib(15);
    while ((float)ping1 > 18) {
        led1 = 1;
        wait(0.01);
    }
    led1 = 0;    
    car.stop();

    // turn right
    wait(1);
    xbee.printf("leave final project\n");
    rotate_90(6, 1);

    // exit
    wait(1);
    go_cm(15, 100);

}