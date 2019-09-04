# ESP

## Model

### ESP8266
Has 
- 17 GPIO pins, but 6 of these pins (6-11) are used for communication with the on-board flash memory chip. So 11 GPIO pins
- WiFi Communication
- An analog input

#### Price
3$-6$ according to https://www.makeradvisor.com

#### Where to buy in France

15.99€ for 3 boards (5.33€ per board) : 
- https://www.amazon.fr/AZ-Delivery-NodeMCU-ESP8266-développement-development/dp/B074Q2WM1Y/ref=sr_1_3?__mk_fr_FR=ÅMÅŽÕÑ&keywords=NodeMCU&qid=1567073501&s=gateway&sr=8-3

### ESP32
More expensive, more functionnalities, may be overkill. 

## Programming

Several option are available : 
- Arduino IDE
- SDK for C-programming
- LUA Interpreter firmware
- MicroPython firmware

The easiest method seems to be the Arduino IDE method. 

## Requirements
- ESP8266 board
- A computer that can run Arduino IDE (Windows, Mac or Linux)
- A USB-To-Serial converter. It must be a **3.3V** model
- A USB Cable
- A 3.3V Power supply or voltage regulator\*
- A WiFi network to connect to

(\*) Your board may already include these.
