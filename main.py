import pyb
import sensor, image, time, os, tf, math

RED_LED_PIN = 1

uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)
tmp = ""
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
clock = time.clock()
pyb.LED(RED_LED_PIN).on()

def degree():
    matrices = 0
    while (not matrices):
        clock.tick()
        img = sensor.snapshot()
        img.lens_corr(1.8) # strength of 1.8 is good for the 2.8mm lens.

        matrices = img.find_datamatrices()
        for matrix in matrices:
            img.draw_rectangle(matrix.rect(), color = (255, 0, 0))
            #print_args = (matrix.rows(), matrix.columns(), matrix.payload(), (180 * matrix.rotation()) / math.pi, clock.fps())
            #print("Matrix [%d:%d], Payload \"%s\", rotation %f (degrees), FPS %f" % print_args)
            uart.write(str(180 * matrix.rotation() / math.pi).encode())
            uart.write("\r\n".encode())

def image_classification():
    sensor.reset()                         # Reset and initialize the sensor.
    sensor.set_pixformat(sensor.RGB565)    # Set pixel format to RGB565 (or GRAYSCALE)
    sensor.set_framesize(sensor.QVGA)      # Set frame size to QVGA (?x?)
    sensor.set_windowing((240, 240))       # Set 240x240 window.
    sensor.skip_frames(time=2000)          # Let the camera adjust.

    pyb.LED(RED_LED_PIN).on()
    sensor.snapshot().save("final.jpg")
    pyb.LED(RED_LED_PIN).off()
    uart.write("\r\n".encode())

while(1):
    a = uart.readline()
    if a is not None:
        tmp += a.decode()
        #print(a.decode())

    #if tmp == "degree":
        #print("read degree")
    #    tmp =""
    #    degree()
    if tmp == "image":
        pyb.LED(RED_LED_PIN).off()
        #print("image_classification")
        tmp = ""
        image_classification()


