# Simon Says — Custom PCB Edition

A handheld memory game built from the ground up: watch the LED sequence, repeat it with the buttons, and see how far you can get before you slip. Each round adds a step and speeds up. It started as a breadboard prototype and became a custom PCB designed in Altium.

<!-- Drop a hero photo here once you have one -->
<!-- ![Finished board](images/finished_board.jpg) -->

## What it does

- Plays the classic Simon Says memory game with 4 LEDs and 4 buttons
- Shows round number and playful win/lose messages on a 1.3" OLED display
- Tracks a persistent high score that survives being powered off (stored in EEPROM)
- Plays a distinct fanfare the moment you beat the high score
- Has a silent mode for playing without sound
- Includes a hidden high-score reset (hold two buttons on the start screen)
- Runs off a rechargeable 7.4V Li-ion battery — fully handheld

## Hardware

| Component | Detail |
|-----------|--------|
| Microcontroller | Arduino Nano (ATmega328) |
| Display | 1.3" SH1106 OLED, 128x64, I2C |
| Inputs | 4x tactile buttons (INPUT_PULLUP, no external resistors) |
| Outputs | 4x LEDs with 220Ω current-limiting resistors |
| Audio | Piezo buzzer |
| Power | 7.4V Li-ion into the Nano's VIN pin |
| PCB | 2-layer board designed in Altium, ground pour on the bottom layer |

## How it's built

The project went through the full development cycle, from prototype to fabricated board:

1. **Breadboard prototype** — got the core game logic, LEDs, buttons, and buzzer working
2. **Feature build-out** — added the OLED display, EEPROM high score, custom sounds, and messages
3. **Schematic capture** — drew the full circuit in Altium, including custom components for parts without library models
4. **PCB layout** — placed components for handheld ergonomics, routed signals on top, and used a ground pour on the bottom layer
5. **Fabrication** — generated Gerbers, ordered the board, and hand-populated it

## Repo structure

```
/code    - Arduino sketch (.ino)
/pcb     - Altium files, Gerbers, and schematic/layout screenshots
/images  - build photos and video
/docs    - engineering log of problems solved and design decisions
```

## Engineering log

I included a catalog of all the problems and decisions I had to make during development. From debugging the display not working, to building the high-score memory code, to all the problems associated with the PCB development, all of the errors and solutions are documented here: [docs/DESIGN_LOG.md](docs/DESIGN_LOG.md).

## Credits

The base game concept and original starting code are from [Arduino_Scuola on the Arduino Project Hub](https://projecthub.arduino.cc/Arduino_Scuola/a-simple-simon-says-game-6f7fef). I rewrote and extended it substantially — adding the display, persistent high scores, custom audio, silent mode, the hidden reset, and the entire custom PCB.

## Built by

Keenan Murnion — email: keenan.murnion@hotmail.com
