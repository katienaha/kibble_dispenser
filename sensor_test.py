import RPi.GPIO as GPIO
import time


def release_kibble(pwm):
    pwm.ChangeDutyCycle(5)
    time.sleep(0.1)
    pwm.ChangeDutyCycle(7)
    time.sleep(0.1)
    pwm.ChangeDutyCycle(0)

GPIO.setmode(GPIO.BCM)
GPIO.setup(4,GPIO.IN)
GPIO.setup(18,GPIO.OUT)
pwm=GPIO.PWM(18,50)
pwm.start(8)
pwm.ChangeDutyCycle(7)
time.sleep(0.1)
pwm.ChangeDutyCycle(0)

prev_input = False



while True:
    #take a reading
    input = GPIO.input(4)
    #if the last reading was low and this one high, alert us
    if ((not prev_input) and input):
        print("Under Pressure")
        release_kibble(pwm)
    #update previous input
    prev_input = input
    #slight pause
    time.sleep(0.10)
    