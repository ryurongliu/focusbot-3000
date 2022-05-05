# FOCUSBOT 3000
An ESP32-controlled robot designed to help you focus by eating your phone for designated amounts of time. Think of it as a mix between an egg timer and a productivity app. 

## [Video Demo](https://www.youtube.com/watch?v=Pd9XV4gEil0)
[![Alien Radio Video Demo](https://img.youtube.com/vi/Pd9XV4gEil0/maxresdefault.jpg)](https://www.youtube.com/watch?v=Pd9XV4gEil0)
See detailed explanation [here](https://ryurongliu.github.io/focusbot-3000/).  

## Concept
FOCUSBOT is simple and easy to use. The left eye controls the time interval (1 min, 5 min, 10 min, 30 min, 1 hr) and the right eye controls the multiple (x1, x2, x3, x4, x5). For example, choosing (10 min) and (x5) will give you 50 minutes of focus time. Then, place your phone inside FOCUSBOT's mouth, close it, and press its nose to start the timer. The lights in its right eyebrow will blink to show you how much time has elapsed.   
If you're tempted to open its mouth, it will scream at you until you close it again. You won't be able to access the inside of its mouth anyways, since FOCUSBOT has its teeth closed during focus time.   
Once focus time is over, FOCUSBOT will beep at you to let you know time's up. At this point, you'll be able to open FOCUSBOT's mouth and take your phone out again.   

## Tech Specs
- [TTGO T-Display ESP32 WiFi BLE](https://www.amazon.com/LILYGO-T-Display-Arduino-Development-CH9102F/dp/B099MPFJ9M)
- potentiometer (x2)
- servo motor
- light dependent resistor
- button (momentary switch)
- Adafruit Neopixel LED strips
- piezo buzzer   

## Reproduction
Components are connected to the ESP32 as follows: 
- left potentiometer to pin 12
- right potentiometer to pin 13
- button to pin 22
- cheek lights to pin 17
- brow lights to pin 32
- light dependent resistor to pin 36
- piezo buzzer to pin 2
- servo motor to pin 15

Pinouts can be adjusted as needed within [focusbot.ino](code/focusbot.ino).
