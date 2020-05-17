# DuinoScope
## Goals and motivation
Duino scope (name subject to change) is an attempt to revive a recovered CGE mount with good mechanical parts and no electronics when I found it. 
The propriatery microcontroller & its associates were completley broke and Celestron wanted ~200$ to replace the parts, and the modern software dosen't support it. 
So, I decided that I, some random dude, could do better than a lare company, such is the folly of man
## Documentation
### Encoders
The system is based on two CUI devices absolute encoders. The absolute encoders were chosen becuase in this application, the position of the scope needs to be kept in some way with
or without power. Technically, this could be done with some EEPROM trickery, but I opted for the absolutes anyway. 
#### Resolution
I have chosen to use the 12 bit resolution to ease some load on the Mega's cpu. 
### Boards
The main box that deals with ui (read leds), motion control, and other tasks is an Ardunio Mega 2560. The pendant controller is based on a $$NAME , which handles user input, an lcd display, and some more switches and leds.
#### Pins
All information on pins can be found in the [Pins.ods](Pins.md). Pins contains wiring information, which can also be found in a visual form in the [Wiring.pdf](Wiring.pdf)(NYI). 