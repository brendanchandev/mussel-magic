import RPi.GPIO as GPIO
import time

# Set the GPIO pin number for the UV light
uv_light_pin = 17

# Set up the GPIO mode and UV light pin
GPIO.setmode(GPIO.BCM)
GPIO.setup(uv_light_pin, GPIO.OUT)

def turn_on_uv_light():
    GPIO.output(uv_light_pin, GPIO.HIGH)
    print("UV light turned on")

def turn_off_uv_light():
    GPIO.output(uv_light_pin, GPIO.LOW)
    print("UV light turned off")

try:
    while True:
        # Turn on the UV light for 5 minutes
        turn_on_uv_light()
        time.sleep(300)  # 5 minutes

        # Turn off the UV light for 25 minutes
        turn_off_uv_light()
        time.sleep(1500)  # 25 minutes

except KeyboardInterrupt:
    # Cleanup GPIO on program exit
    GPIO.cleanup()