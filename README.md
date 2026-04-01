# pioduino_01

PlatformIO project for bringing up an ESP32-based motor-control setup with:

- a BLDC motor
- an AS5600 magnetic angle sensor
- a 3PWM motor driver
- an optional ICM-20948 IMU over I2C or SPI

The repository is organized as a small bring-up lab: several standalone `main_*.cpp` files exist for individual hardware tests, and `platformio.ini` selects which one is currently built.

## Current status

The active build target is `src/main_simplefoc_01.cpp`.

That sketch uses:

- `SimpleFOC`
- `AS5600` over I2C on `GPIO21/GPIO22`
- a 3PWM driver on `GPIO25/GPIO26/GPIO27`
- driver enable on `GPIO14`

The current firmware is intended as a cautious first bring-up:

- low driver voltage limit during startup
- low velocity limit
- serial control via SimpleFOC `Commander`

## Project structure

- `src/main_simplefoc_01.cpp`: BLDC motor bring-up with `SimpleFOC` and `AS5600`
- `src/main_as5600.cpp`: isolated AS5600 I2C test
- `src/main_icm20948_i2c.cpp`: ICM-20948 I2C test with bus scan and recovery
- `src/main_icm20948_spi.cpp`: ICM-20948 SPI test with recovery logic
- `doc/main-dateien-doku.md`: detailed notes about the different `main` files
- `doc/linklist.md`: collected reference links
- `platformio.ini`: PlatformIO environment, library dependencies, and active source filter

## Dependencies

Defined in [`platformio.ini`](/c:/ttt/_embedded/pioduino_01/platformio.ini):

- `Simple FOC` `^2.4.0`
- `SparkFun 9DoF IMU Breakout - ICM 20948 - Arduino Library` `^1.3.2`

The project uses the Arduino framework on `env:esp32dev` and pulls the ESP32 platform from the `pioarduino` release ZIP configured in `platformio.ini`.

## Hardware mapping

### SimpleFOC bring-up

AS5600:

- `GPIO21` -> `SDA`
- `GPIO22` -> `SCL`
- I2C address `0x36`

3PWM driver:

- `GPIO25` -> phase A / `IN1`
- `GPIO26` -> phase B / `IN2`
- `GPIO27` -> phase C / `IN3`
- `GPIO14` -> driver enable

Power:

- logic side: ESP32 `3V3` / `GND`
- motor side: external supply, currently modeled in code as `12 V`
- all grounds must be shared

### ICM-20948 test wiring

I2C mode:

- `GPIO21` -> `SDA`
- `GPIO22` -> `SCL`
- address `0x68` or `0x69` depending on `AD0`

SPI mode:

- `GPIO18` -> `SCK`
- `GPIO19` -> `MISO`
- `GPIO23` -> `MOSI`
- `GPIO5` -> `CS`

## Build and upload

Typical PlatformIO commands:

```powershell
platformio run
platformio run -t upload
platformio device monitor -b 115200
```

If you use the local PlatformIO installation from this workspace, the equivalent command pattern is:

```powershell
$env:PLATFORMIO_CORE_DIR=(Resolve-Path '.')
& 'C:\Users\heiko\.platformio\penv\Scripts\platformio.exe' run
```

## Switching between test programs

PlatformIO currently builds only one source file via `build_src_filter` in [`platformio.ini`](/c:/ttt/_embedded/pioduino_01/platformio.ini).

At the moment:

- `main_simplefoc_01.cpp` is included
- `main_as5600.cpp` is excluded
- `main_icm20948_i2c.cpp` is excluded
- `main_icm20948_spi.cpp` is excluded

To switch to another bring-up sketch, edit `build_src_filter` and enable the file you want to build.

Example:

```ini
build_src_filter =
  +<main_as5600.cpp>
  -<main_simplefoc_01.cpp>
  -<main_icm20948_spi.cpp>
  -<main_icm20948_i2c.cpp>
```

## Recommended bring-up order

1. Run `src/main_as5600.cpp` to verify the angle sensor first.
2. Run `src/main_icm20948_i2c.cpp` or `src/main_icm20948_spi.cpp` if the IMU is part of the setup.
3. Only then run `src/main_simplefoc_01.cpp` with the motor driver connected.

This reduces ambiguity when debugging motor startup, because sensor and bus issues are isolated first.

## Serial interaction

`src/main_simplefoc_01.cpp` registers SimpleFOC `Commander` commands on the serial port.

Relevant command currently exposed:

- `T<value>`: writes the target value used by `motor.move(...)`
- `M...`: forwards commands to the SimpleFOC motor commander

Examples:

```text
T1.57
```

Important: the variable is named `targetAngle`, but the active control mode in the code is currently `MotionControlType::velocity`. So the sketch is presently closer to a bring-up/tuning sketch than a finished angle-control application.

## Safety notes

- Start with the motor unloaded if possible.
- Keep the configured voltage and velocity limits conservative until wiring and sensor direction are verified.
- Verify AS5600 readings before enabling motor movement.
- Use a common ground between ESP32, driver, sensor, and power supply.
- Be ready to cut power quickly during first FOC alignment and early tuning.

## Related documentation

- [main-dateien-doku.md](/c:/ttt/_embedded/pioduino_01/doc/main-dateien-doku.md)
- [linklist.md](/c:/ttt/_embedded/pioduino_01/doc/linklist.md)
- [why_pioardiono.md](/c:/ttt/_embedded/pioduino_01/doc/why_pioardiono.md)
