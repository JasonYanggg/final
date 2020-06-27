#include "mbed.h"
#include "bbcar.h"

#define left_v 5.5
#define right_v 6

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

void rotate_90(double speed, int dir, double time)
{
    car.rotate(speed, dir);
    wait(time);
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

    xbee.printf("walk straight\r\n");
    car.goStraightCalib(15);
    while ((float)ping1 > 20) {
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

    // // turn left to enter mission 1
    // wait(1);
    // xbee.printf("turn left\r\n");
    // rotate_90(left_v, -1, 1.5);

    // // go straight to the front of the parking space
    // wait(1);
    // xbee.printf("walk straight\n");
    // go_cm(15, 55);
    
    // // turn right 
    // wait(1);
    // xbee.printf("start reverse parking\n");
    // rotate_90(right_v, 1, 1.5);

    // // reverse parking
    // wait(1);
    // go_cm(-15, 25);

    // // go out of the parking space
    // wait(1);
    // go_cm(15, 20);

    // // turn left
    // wait(1);
    // rotate_90(left_v, -1, 1.5);

    // // go straight
    // wait(1);
    // go_cm(10, 15);

    // // turn right
    // wait(1);
    // rotate_90(right_v, 1, 1.5);

    // // image classification
    // wait(1);
    // xbee.printf("image\r\n");
    // sprintf(s, "image");
    // uart.puts(s);
    // int i = 0;
    // while (1) {
    //     if (uart.readable()) {
    //         char recv = uart.getc();
    //         if (recv == '\r') {
    //             break;
    //         }
    //     }
    // }
    // xbee.printf("finish snapshot\r\n", s);

    // // turn right
    // wait(1);
    // xbee.printf("leave mission 1\r\n");
    // rotate_90(right_v, 1, 1.5);

    // // leave mission 1
    // wait(1);
    // go_cm(15, 50);

    // // go to mission 2
    // wait(1);
    // xbee.printf("walk straight\r\n");
    // car.goStraightCalib(15);
    // while ((float)ping1 > 20) {
    //     led1 = 1;
    //     wait(0.01);
    // }
    // led1 = 0;    
    // car.stop();
    
    // // turn right
    // wait(1);
    // xbee.printf("turn right\r\n");
    // rotate_90(right_v, 1, 1.5);

    // // go straight
    // wait(1);
    // xbee.printf("enter mission 2\r\n");
    // go_cm(15, 30);

    // // turn right
    // wait(1);
    // xbee.printf("go to the corresponding position\r\n");
    // rotate_90(right_v, 1, 1.5);

    // go straight
    wait(1);
    go_cm(15, 25);

    // scan
    wait(1);
    double dis[3];
    xbee.printf("scan the object\r\n");
    rotate_90(left_v, -1, 0.5);
    dis[0] = float(ping1);
    wait(0.5);
    rotate_90(right_v, 1, 0.5);
    dis[1] = float(ping1);
    wait(0.5);
    rotate_90(right_v, 1, 0.5);
    dis[2] = float(ping1);
    wait(0.5);
    rotate_90(left_v, -1, 0.5);

    // judge the object
    if (dis[0] - dis[1] > 5) {
        xbee.printf("the object is triangle\r\n");
    }
    else if (dis[0] - dis[1] > -5) {
        if (dis[1] > dis[2]) {
            xbee.printf("the object is right triangle\r\n");
        }
        else {
            xbee.printf("the object is square\r\n");
        }
    }
    else {
        xbee.printf("the object is concave\r\n");
    }

    // go back
    wait(1);
    xbee.printf("leave mission 2\r\n");
    go_cm(-15, 25);

    // turn left
    wait(1);
    rotate_90(left_v, -1, 1.5);

    // go straight
    wait(1);
    car.goStraightCalib(15);
    while ((float)ping1 > 20) {
        led1 = 1;
        wait(0.01);
    }
    led1 = 0;    
    car.stop();

    // turn right
    wait(1);
    xbee.printf("end\r\n");
    rotate_90(right_v, 1, 1.5);

    // exit
    wait(1);
    go_cm(15, 100);

}