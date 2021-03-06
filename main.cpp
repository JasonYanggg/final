#include "mbed.h"
#include "bbcar.h"

#define left_v 5.3
#define right_v 6

RawSerial xbee(D12, D11);
Serial uart(D1, D0);
PwmOut pin8(D8), pin9(D9);
DigitalIn button(SW2);
DigitalIn pin3(D3);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalInOut pin10(D10);
// RawSerial pc(USBTX, USBRX);

Ticker servo_ticker;
Ticker encoder_ticker;
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

void rotate(double speed, int dir, double time)
{
    car.rotate(speed, dir);
    wait(time);
    car.stop();
}

// void reply_messange(char *xbee_reply, char *messange){
//   xbee_reply[0] = xbee.getc();
//   xbee_reply[1] = xbee.getc();
//   xbee_reply[2] = xbee.getc();
//   if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
//     // pc.printf("%s\r\n", messange);
//     xbee_reply[0] = '\0';
//     xbee_reply[1] = '\0';
//     xbee_reply[2] = '\0';
//   }
// }

// void check_addr(char *xbee_reply, char *messenger){
//   xbee_reply[0] = xbee.getc();
//   xbee_reply[1] = xbee.getc();
//   xbee_reply[2] = xbee.getc();
//   xbee_reply[3] = xbee.getc();
// //   pc.printf("%s = %c%c%c\r\n", messenger, xbee_reply[1], xbee_reply[2], xbee_reply[3]);
//   xbee_reply[0] = '\0';
//   xbee_reply[1] = '\0';
//   xbee_reply[2] = '\0';
//   xbee_reply[3] = '\0';
// }

int main()
{
    double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table0[] = {-16.507, -16.029, -15.152, -12.201, -6.220, 0.000, 5.822, 11.803, 14.833, 15.950, 16.588};
    double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table1[] = {-15.949, -15.471, -14.514, -11.563, -5.503, 0.000, 6.619, 12.201, 15.072, 16.109, 16.508};

    // first and fourth argument: length of table
    car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);  
    car.stop();
    led1 = 1;
    led2 = 1;
    led3 = 1;

    // pc.baud(9600);
    char xbee_reply[4];

    // XBee setting
    xbee.baud(9600);
    // xbee.printf("+++");
    // // xbee_reply[0] = xbee.getc();
    // // xbee_reply[1] = xbee.getc();
    // // if(xbee_reply[0] == 'O' && xbee_reply[1] == 'K'){
    // //     // pc.printf("enter AT mode.\r\n");
    // //     xbee_reply[0] = '\0';
    // //     xbee_reply[1] = '\0';
    // // }
    // xbee.printf("ATMY 0x248\r\n");
    // // reply_messange(xbee_reply, "setting MY : 0x248");

    // xbee.printf("ATDL 0x148\r\n");
    // // reply_messange(xbee_reply, "setting DL : 0x148");

    // xbee.printf("ATID 0x1\r\n");
    // // reply_messange(xbee_reply, "setting PAN ID : 0x1");

    // xbee.printf("ATWR\r\n");
    // // reply_messange(xbee_reply, "write config");

    // xbee.printf("ATMY\r\n");
    // // check_addr(xbee_reply, "MY");

    // xbee.printf("ATDL\r\n");
    // // check_addr(xbee_reply, "DL");

    // xbee.printf("ATCN\r\n");
    // // reply_messange(xbee_reply, "exit AT mode");
    // // xbee.getc();
    // led1 = 0;

    while (button == 1);
    led1 = 1;

    xbee.printf("walk straight\r\n");
    car.goStraightCalib(15);
    while ((float)ping1 > 20) {
        wait(0.01);
    }
    car.stop();
    led1 = 0;  
    
    // wait(1);
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

    // turn right to enter mission 1
    wait(1);
    led1 = 1;
    xbee.printf("turn right\r\n");
    rotate(right_v, 1, 1.5);

    // go straight to the front of the parking space
    wait(1);
    led1 = 0;
    xbee.printf("walk back\r\n");
    go_cm(-15, 55);
    led1 = 1;
    
    // turn left
    wait(1);
    xbee.printf("turn left\r\n");
    rotate(left_v, -1, 1.5);

    // reverse parking
    wait(1);
    led1 = 0;
    xbee.printf("start reverse parking\r\n");
    go_cm(-15, 25);
    led1 = 1;

    // go out of the parking space
    wait(1);
    led1 = 0;
    xbee.printf("go out\r\n");
    go_cm(15, 12);
    led1 = 1;

    // turn left
    wait(1);
    xbee.printf("turn left\r\n");
    rotate(left_v, -1, 1.5);

    // go straight
    wait(1);
    xbee.printf("go straight");
    go_cm(10, 10);

    // turn right
    wait(1);
    xbee.printf("turn right\r\n");
    rotate(right_v, 1, 1.5);

    // image classification
    wait(1);
    led1 = 0;
    xbee.printf("image\r\n");
    sprintf(s, "image");
    uart.puts(s);
    int i = 0;
    while (1) {
        if (uart.readable()) {
            char recv = uart.getc();
            if (recv == '\r') {
                break;
            }
        }
    }
    xbee.printf("finish snapshot\r\n");
    led1 = 1;

    // go straight
    wait(1);
    xbee.printf("go straight\r\n");
    go_cm(10, 5);

    // turn right
    wait(1);
    xbee.printf("turn right\r\n");
    rotate(right_v, 1, 1.5);

    // leave mission 1
    wait(1);
    led1 = 0;
    xbee.printf("leave mission 1\r\n");
    go_cm(15, 45);

    // turn right
    wait(1);
    xbee.printf("turn right\r\n");
    rotate(right_v, 1, 1.5);

    // go to mission 2
    wait(1);
    xbee.printf("walk straight\r\n");
    car.goStraightCalib(15);
    while ((float)ping1 > 25) {
        wait(0.01);
    }
    led1 = 1;    
    car.stop();
    
    // turn right
    wait(1);
    // rotate(right_v, 1, 1.5);
    xbee.printf("turn right\r\n");
    car.turn(right_v, 1);
    wait(3);
    car.stop();

    // go straight
    wait(1);
    led1 = 0;
    xbee.printf("enter mission 2\r\n");
    go_cm(15, 32);
    led1 = 1;

    // turn right
    wait(1);
    xbee.printf("go to the corresponding position\r\n");
    rotate(right_v, 1, 1.5);

    // go straight
    wait(1);
    led1 = 0;
    xbee.printf("go straight\r\n");
    go_cm(15, 25);
    led1 = 1;

    // scan
    wait(1);
    double dis[3];
    xbee.printf("scan the object\r\n");
    rotate(left_v, -1, 0.5);
    dis[0] = float(ping1);
    led1 = 0;
    wait(0.5);
    led1 = 1;
    rotate(right_v, 1, 0.5);
    dis[1] = float(ping1);
    led1 = 0;
    wait(0.5);
    led1 = 1;
    rotate(right_v, 1, 0.5);
    dis[2] = float(ping1);
    led1 = 0;
    wait(0.5);
    led1 = 1;
    rotate(left_v, -1, 0.5);

    // judge the object
    if (dis[0] - dis[1] > 5) {
        led1 = 0;
        xbee.printf("the object is triangle\r\n");
    }
    else if (dis[0] - dis[1] > -5) {
        if (dis[1] > dis[2]) {
            led2 = 0;
            xbee.printf("the object is right triangle\r\n");
        }
        else {
            led3 = 0;
            xbee.printf("the object is square\r\n");
        }
    }
    else {
        led1 = 0;
        led3 = 0;
        xbee.printf("the object is concave\r\n");
    }
    wait(5);
    led1 = 1;
    led2 = 1;
    led3 = 1;

    // go back
    wait(1);
    led1 = 0;
    xbee.printf("leave mission 2\r\n");
    go_cm(-15, 25);
    led1 = 1;

    // turn left
    wait(1);
    xbee.printf("turn left\r\n");
    rotate(left_v, -1, 1.5);

    // go straight
    wait(1);
    led1 = 0;
    xbee.printf("go straight\r\n");
    car.goStraightCalib(15);
    while ((float)ping1 > 20) {
        wait(0.01);
    }
    led1 = 1;    
    car.stop();

    // turn right
    wait(1);
    xbee.printf("turn right\r\n");
    rotate(right_v, 1, 1.5);

    // exit
    wait(1);
    led1 = 0;
    xbee.printf("exit\r\n");
    car.goStraightCalib(15);
    while ((float)ping1 > 20) {
        wait(0.01);
    }
    led1 = 1;    
    car.stop();

}