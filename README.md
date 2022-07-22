# About
This project uses an ESP8266 to host a super quick, WebSockets based, user interface.

[Youtube Video Demo](https://www.youtube.com/watch?v=jFhkxL-Fyv8)

## Requirements
- An ESP8266
- The latest version of the Arduino IDE with:
   - [The ESP8266 Core Installed](https://github.com/esp8266/Arduino)
   - [The ESP8266FS plugin Installed](https://github.com/esp8266/arduino-esp8266fs-plugin) (used to upload the HTML, JS and CSS files to the ESP)
    - The "WebSockets" library installed (available from the library manager. From the top menu of the IDE, choose Sketch -> Include Library -> Manage Libraries).

## Setup
- Attach a potentiometer to the `A0` pin.
- Attach a servo to the `D2` pin.
- Connect the ESP8266 to your computer
- Open the `ESP8266WebSocketsDemo` sketch and update the network settings to match your network's details (right after the #includes).
- Upload the sketch.
- From the top menu in the IDE, choose Tools -> ESP8266 Sketch Upload to upload the web files from the `data` directory.
- Reboot your ESP8266 and open the serial monitor to see its IP address.
- Navigate to the IP address on your phone and enjoy!
