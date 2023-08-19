# Gameboy cartridge arduino connector

Simple project to be able to read Gameboy cartridges via arduino board.

Functionality:
 * Read single byte from address
 * Read block of bytes
 * Write value to address
 * Listening to commands via serial interface
 * Returns results either/or as hex dump or base 64 encoded string

## Requirements

Version 1 supported only boards with number of gpio pins at least 27 (i.e. Arduino Mega) since address bus required pin for each bit.

Version 2 connects to address bus using two 74595 shift registers which reduces number of pins required to 14.

Boards must be 5V compatible.

## Schematic

<picture>
  <img alt="schematic-v2" src="/img/schematic-v2.png" />
</picture>

## Release notes

### Version 2.0.0 (August 19, 2023)

 * Extracted address bus abstract class
 * Implemented shift register address bus class
 * Uploaded new schematic with shift registers

### Version 1.0.0 (August 2, 2023)

 * Read single byte
 * Read block
 * Write single byte
 * Parsing commands from serial
 * Implemented tests for command parsing
