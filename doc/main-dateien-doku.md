# Dokumentation der Main-Dateien in `pioduino_01`

Diese Dokumentation beschreibt nur das Projekt `pioduino_01`.
Hinweis: Einige `main`-Dateien wurden umbenannt; diese Doku spiegelt die aktuellen Dateinamen wider.

## Inhaltsverzeichnis

- [Projektumgebung — #projektumgebung](#projektumgebung)
- [Projektkonfiguration — platformio.ini](platformio.ini#L1)
- [AS5600 Sensor-Test — src/main_as5600.cpp](src/main_as5600.cpp#L1)
- [ICM-20948 (I2C) — src/main_icm20948_i2c.cpp](src/main_icm20948_i2c.cpp#L1)
- [ICM-20948 (SPI) — src/main_icm20948_spi.cpp](src/main_icm20948_spi.cpp#L1)
- [SimpleFOC Motor Bring-up — src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L1)

Grundlage sind die vorhandenen Dateien in:

- `platformio.ini`
- `src/main_as5600.cpp`
- `src/main_icm20948_i2c.cpp`
- `src/main_icm20948_spi.cpp`
- `src/main_simplefoc_01.cpp`

## Projektumgebung

- **Typ:** PlatformIO‑Projekt (VS Code + pioarduino-IDE (PlatformIO Toolchain) )
- **IDE/Extension:** `pioarduino-vscode-ide` (Fork der offiziellen PlatformIO‑IDE). Wir verwenden dieses Plugin, weil es native Unterstützung für die aktuellen Espressif/ESP32‑Cores (v3.x) und neue SoCs (z. B. C6, H2, P4) bietet und dadurch komfortabler und stabiler ist als das manuelle Überschreiben per URL bei dem orginalen PlatformIO.

Gründe (Kurzfassung):

- **Out‑of‑the‑Box GUI‑Support:** Der New‑Project‑Wizard erstellt automatisch eine moderne `platformio.ini` mit der pioarduino‑Registry (kein Copy‑/Paste von Git‑URLs nötig).
- **Entkopplung von Upstream‑Politik:** Das Fork schützt vor Registry‑ und Kompatibilitätsproblemen, die durch Änderungen in der offiziellen Distribution entstehen können.
- **Moderne Toolchain & Chips:** Direkter Zugriff auf aktuelle ESP‑IDF/GCC‑Toolchains und neue ESP32‑Varianten.
- **Alternative IntelliSense‑Backends:** Möglichkeit, z. B. `clangd` statt ausschließlich Microsoft C/C++ für Embedded‑Entwicklung zu verwenden.

Repository‑Konfiguration (praktisch):

- Das Projekt nutzt `env:esp32dev`; in `platformio.ini` ist ein Platform‑Override auf die pioarduino‑Distribution eingetragen. Siehe [platformio.ini](platformio.ini#L1).
- Wichtige `lib_deps` sind in `platformio.ini` aufgeführt (z. B. `askuric/Simple FOC@^2.4.0`, `sparkfun/SparkFun 9DoF IMU Breakout - ICM 20948 - Arduino Library@^1.3.2`).

Build & Upload (Kurzbefehle):

```bash
# Build
platformio run

# Upload
platformio run -t upload
```

Hinweis: Alternativ zum Installieren des Fork‑Plugins lässt sich die Platform‑URL manuell in `platformio.ini` setzen (funktioniert), siehe WHY_THIS_FORK.md für die Motivation hinter dem Fork:
https://github.com/pioarduino/pioarduino-vscode-ide/blob/HEAD/WHY_THIS_FORK.md

## 1. Projektüberblick

Die aktive Build-Konfiguration ist `env:esp32dev`. Über `build_src_filter` wird aktuell nur `main_simplefoc_01.cpp` gebaut. Die anderen `main`-Dateien sind Alternativen für Tests und Bring-up. Siehe [platformio.ini](platformio.ini#L11).

Aktiv:

- `main_simplefoc_01.cpp`

Vorhandene Alternativen:

- `main_as5600.cpp`
- `main_icm20948_i2c.cpp`
- `main_icm20948_spi.cpp`

Verwendete Bibliotheken:

- `Simple FOC ^2.4.0`
- `SparkFun 9DoF IMU Breakout - ICM 20948 - Arduino Library ^1.3.2`

## 2. Main-Dateien im Detail

## 2.1 `src/main_as5600.cpp`

Datei: [main_as5600.cpp](src/main_as5600.cpp#L1)

Zweck:

- einfacher I2C-Test für den Magnetsensor `AS5600`
- Ausgabe des Winkels in Radiant und Grad

### Verwendete Pins

- `GPIO21`: `SDA`, siehe [src/main_as5600.cpp](src/main_as5600.cpp#L18)
- `GPIO22`: `SCL`, siehe [src/main_as5600.cpp](src/main_as5600.cpp#L19)

### Externe Komponenten

- ESP32-Board
- `AS5600` Magnetsensor über I2C
- Magnet auf der Welle oder Testachse

### Wichtige Parameter

- `AS5600_I2C_ADDRESS = 0x36`, siehe [src/main_as5600.cpp](src/main_as5600.cpp#L20)
- `SERIAL_BAUD = 115200`, siehe [src/main_as5600.cpp](src/main_as5600.cpp#L21)
- `PRINT_INTERVAL_MS = 200`, also 5 Hz Ausgabe, siehe [src/main_as5600.cpp](src/main_as5600.cpp#L22)
- I2C-Takt `400000`, siehe [src/main_as5600.cpp](src/main_as5600.cpp#L40)
- Sensorobjekt `MagneticSensorI2C(AS5600_I2C)`, siehe [.pio/libdeps/esp32dev/Simple%20FOC/src/sensors/MagneticSensorI2C.h](.pio/libdeps/esp32dev/Simple%20FOC/src/sensors/MagneticSensorI2C.h#L21)

### Sinnvolle Alternativen

- I2C-Takt `100000`, wenn das Setup mit 400 kHz instabil ist
- `PRINT_INTERVAL_MS = 50` oder `100` für dichtere Ausgabe
- `PRINT_INTERVAL_MS = 500` oder `1000` für einfache Diagnose
- statt `AS5600_I2C` auch andere vorkonfigurierte Sensoren wie `AS5048_I2C` oder `MT6701_I2C`, sofern die Hardware passt. Siehe [.pio/libdeps/esp32dev/Simple%20FOC/src/sensors/MagneticSensorI2C.h](.pio/libdeps/esp32dev/Simple%20FOC/src/sensors/MagneticSensorI2C.h#L21)
- alternativ eigene Registerdefinition über den generischen `MagneticSensorI2C`-Konstruktor. Siehe [.pio/libdeps/esp32dev/Simple%20FOC/src/sensors/MagneticSensorI2C.h](.pio/libdeps/esp32dev/Simple%20FOC/src/sensors/MagneticSensorI2C.h#L38)

### Technische Hinweise

- Diese Datei eignet sich als Vorstufe für `main_simplefoc_01.cpp`, weil hier nur der Sensor getestet wird.
- Geprüft wird nur die Erreichbarkeit auf `0x36` und die laufende Winkelausgabe.

## 2.2 `src/main_icm20948_i2c.cpp`

Datei: [main_icm20948_i2c.cpp](src/main_icm20948_i2c.cpp#L1)

Zweck:

- I2C-Test für die SparkFun-IMU `ICM-20948`
- Scan des I2C-Busses beim Start
- Test beider möglichen IMU-Adressen `0x68` und `0x69`
- Fehlerzähler, Bus-Recovery und ESP-Neustart bei Kommunikationsproblemen
- geglättete Teleplot-Ausgabe für Accel, Gyro und Magnetometer

### Verwendete Pins

- `GPIO21`: `SDA`, siehe [src/main_icm20948_i2c.cpp](src/main_icm20948_i2c.cpp#L11)
- `GPIO22`: `SCL`, siehe [src/main_icm20948_i2c.cpp](src/main_icm20948_i2c.cpp#L12)

### Externe Komponenten

- ESP32-Board
- SparkFun `ICM-20948` IMU über I2C

### Wichtige Parameter

- `I2C_FREQ_HZ = 100000`: I2C mit 100 kHz, siehe [src/main_icm20948_i2c.cpp](src/main_icm20948_i2c.cpp#L13)
- `I2C_FAIL_THRESHOLD = 4`: nach 4 Fehlern wird eine Recovery versucht, siehe [src/main_icm20948_i2c.cpp](src/main_icm20948_i2c.cpp#L14)
- `NO_SAMPLE_TIMEOUT_MS = 800`: wenn 800 ms lang keine Daten kommen, wird neu gestartet, siehe [src/main_icm20948_i2c.cpp](src/main_icm20948_i2c.cpp#L15)
- `I2C_HARD_RESTART_DELAY_MS = 250`: Wartezeit vor `ESP.restart()`, siehe [src/main_icm20948_i2c.cpp](src/main_icm20948_i2c.cpp#L16)
- `MA_WINDOW_SIZE = 10`: Fenster für gleitenden Mittelwert, siehe [src/main_icm20948_i2c.cpp](src/main_icm20948_i2c.cpp#L17)
- `MA_EPSILON = 0.05`: Ausgabe nur bei relevanter Änderung, siehe [src/main_icm20948_i2c.cpp](src/main_icm20948_i2c.cpp#L18)
- AD0-Autodetektion über `initIMUWithAD0(0)` und `initIMUWithAD0(1)`, siehe [src/main_icm20948_i2c.cpp](src/main_icm20948_i2c.cpp#L109)

### Sinnvolle Alternativen

- I2C mit `400000`, wenn Verdrahtung kurz und stabil ist
- `MA_WINDOW_SIZE = 3` bis `5` für schnellere Reaktion
- `MA_WINDOW_SIZE = 20` für ruhigere Kurven
- `MA_EPSILON = 0.01` für empfindlichere Ausgabe
- `NO_SAMPLE_TIMEOUT_MS = 1500` bis `3000`, wenn die IMU nicht durchgehend Daten liefert
- feste AD0-Vorgabe statt Autodetektion, falls die Beschaltung bekannt ist

### Technische Hinweise

- Die Bibliothek unterstützt `begin(TwoWire&, bool ad0val, uint8_t ad0pin)`. Damit sind beide AD0-Zustände vorgesehen. Siehe [.pio/libdeps/esp32dev/SparkFun%209DoF%20IMU%20Breakout%20-%20ICM%2020948%20-%20Arduino%20Library/src/ICM_20948.h](.pio/libdeps/esp32dev/SparkFun%209DoF%20IMU%20Breakout%20-%20ICM%2020948%20-%20Arduino%20Library/src/ICM_20948.h#L260)
- Die Recovery in dieser Datei erzeugt 9 Clock-Pulse auf `SCL`, wenn der Bus hängt. Siehe [src/main_icm20948_i2c.cpp](src/main_icm20948_i2c.cpp#L88)
- **TODO:** Testen, ob die Bibliothek `Adafruit ICM20X` (PlatformIO‑Paket: `Adafruit ICM20X by Adafruit`) als Alternative zur SparkFun‑Bibliothek funktioniert; insbesondere Kompatibilität mit I2C/SPI, Initialisierung und Mess‑Performance prüfen.

## 2.3 `src/main_icm20948_spi.cpp`

Datei: [main_icm20948_spi.cpp](src/main_icm20948_spi.cpp#L1)

Zweck:

- SPI-Test für die SparkFun-IMU `ICM-20948`
- geglättete Teleplot-Ausgabe
- Fehlerzähler, SPI-Reinitialisierung und Neustart bei Störungen

### Verwendete Pins

- `GPIO18`: `SPI_SCK`, siehe [src/main_icm20948_spi.cpp](src/main_icm20948_spi.cpp#L31)
- `GPIO19`: `SPI_MISO`, siehe [src/main_icm20948_spi.cpp](src/main_icm20948_spi.cpp#L32)
- `GPIO23`: `SPI_MOSI`, siehe [src/main_icm20948_spi.cpp](src/main_icm20948_spi.cpp#L33)
- `GPIO5`: `SPI_CS`, siehe [src/main_icm20948_spi.cpp](src/main_icm20948_spi.cpp#L34)

### Externe Komponenten

- ESP32-Board
- SparkFun `ICM-20948` IMU im SPI-Modus

### Wichtige Parameter

- `SPI_FREQ_HZ = 4000000`, siehe [src/main_icm20948_spi.cpp](src/main_icm20948_spi.cpp#L35)
- `SPI_FAIL_THRESHOLD = 4`, siehe [src/main_icm20948_spi.cpp](src/main_icm20948_spi.cpp#L36)
- `NO_SAMPLE_TIMEOUT_MS = 800`, siehe [src/main_icm20948_spi.cpp](src/main_icm20948_spi.cpp#L37)
- `SPI_HARD_RESTART_DELAY_MS = 250`, siehe [src/main_icm20948_spi.cpp](src/main_icm20948_spi.cpp#L38)
- `MA_WINDOW_SIZE = 10`, siehe [src/main_icm20948_spi.cpp](src/main_icm20948_spi.cpp#L39)
- `MA_EPSILON = 0.05`, siehe [src/main_icm20948_spi.cpp](src/main_icm20948_spi.cpp#L40)

### Sinnvolle Alternativen

- andere freie `CS`-Leitung statt `GPIO5`
- andere SPI-Pinbelegung über `SPI.begin(sck, miso, mosi, ss)`, solange sie zum Boardlayout passt
- `SPI_FREQ_HZ = 1000000` oder `2000000`, wenn die Verbindung instabil ist
- höhere SPI-Frequenz bei sauberer Verdrahtung; 4 MHz ist der Bibliotheks-Default. Siehe [.pio/libdeps/esp32dev/SparkFun%209DoF%20IMU%20Breakout%20-%20ICM%2020948%20-%20Arduino%20Library/src/ICM_20948.h](.pio/libdeps/esp32dev/SparkFun%209DoF%20IMU%20Breakout%20-%20ICM%2020948%20-%20Arduino%20Library/src/ICM_20948.h#L264)

## 2.4 `src/main_simplefoc_01.cpp`

Datei: [main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L1)

Zweck:

- Inbetriebnahme eines BLDC-Motors mit `SimpleFOC`
- Rückführung über `AS5600`
- 3PWM-Treiberansteuerung
- serielle Parametrierung über `Commander`

### Verwendete Pins

```
ESP32                SimpleFOC Mini
------               ----------------
GPIO25 ------------> IN1
GPIO26 ------------> IN2
GPIO27 ------------> IN3
GPIO4  ------------> EN

GND ---------------- GND
          \--------- Netzteil GND

12V Netzteil ------> VMOT

Motor:
U/V/W -------------> Motor
````
Sensor:

- `GPIO21`: `SDA`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L29)
- `GPIO22`: `SCL`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L30)

Treiber:

- `GPIO25`: PWM Phase A, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L33)
- `GPIO26`: PWM Phase B, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L34)
- `GPIO27`: PWM Phase C, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L35)
- `GPIO14`: Enable des Treibers, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L36)

### Externe Komponenten

- ESP32-Board
- `AS5600` Magnetsensor
- 3-phasiger BLDC-Motor
- externer 3PWM-BLDC-Treiber mit Enable-Eingang
- externe Motorversorgung, im Code mit `12.0 V` modelliert

### Wichtige Parameter

Motorparameter:

- `MOTOR_POLE_PAIRS = 7`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L41)
- `MOTOR_PHASE_RESISTANCE_OHM = 2.3`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L42)
- `MOTOR_KV = 220`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L43)
- `MOTOR_PHASE_INDUCTANCE_H = 0.00086`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L44)

Treiber:

- `driver.voltage_power_supply = 12.0`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L74)
- `driver.voltage_limit = 2.0`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L75)

Regelung:

- `motor.foc_modulation = SpaceVectorPWM`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L79)
- `motor.controller = MotionControlType::velocity`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L81)
- `motor.voltage_limit = 12.0`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L83)
- `motor.velocity_limit = 10.0`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L84)
- `motor.voltage_sensor_align = 1.0`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L85)

PID und Filter:

- `PID_velocity.P = 0.1`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L87)
- `PID_velocity.I = 1.0`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L88)
- `PID_velocity.D = 0.0`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L89)
- `PID_velocity.output_ramp = 100.0`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L90)
- `LPF_velocity.Tf = 0.02`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L91)
- `P_angle.P = 10.0`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L93)

Kommunikation:

- `T` setzt den Sollwert `targetAngle`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L126)
- `M` nutzt die `Commander`-Motorsteuerung, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L127)

### Sinnvolle Alternativen

Alternativen für den Motor-Konstruktor:

- `BLDCMotor(pp)`
- `BLDCMotor(pp, R)`
- `BLDCMotor(pp, R, KV)`
- `BLDCMotor(pp, R, KV, Lq, Ld)`

Die Bibliothek erlaubt diese Staffelung über Default-Parameter. Siehe [.pio/libdeps/esp32dev/Simple%20FOC/src/BLDCMotor.h](.pio/libdeps/esp32dev/Simple%20FOC/src/BLDCMotor.h#L27)

Alternativen für den Motion-Control-Modus:

- `torque`
- `velocity`
- `angle`
- `velocity_openloop`
- `angle_openloop`
- `angle_nocascade`
- `custom`

Siehe [.pio/libdeps/esp32dev/Simple%20FOC/src/common/base_classes/FOCMotor.h](.pio/libdeps/esp32dev/Simple%20FOC/src/common/base_classes/FOCMotor.h#L49)

Alternativen für die Modulation:

- `SinePWM`
- `SpaceVectorPWM`
- `Trapezoid_120`
- `Trapezoid_150`

Siehe [.pio/libdeps/esp32dev/Simple%20FOC/src/common/base_classes/FOCMotor.h](.pio/libdeps/esp32dev/Simple%20FOC/src/common/base_classes/FOCMotor.h#L72)

Alternativen für den Treiber:

- `BLDCDriver3PWM(phA, phB, phC, en1)`
- `BLDCDriver3PWM(phA, phB, phC, en1, en2, en3)`

Damit sind auch getrennte Enable-Leitungen möglich. Siehe [.pio/libdeps/esp32dev/Simple%20FOC/src/drivers/BLDCDriver3PWM.h](.pio/libdeps/esp32dev/Simple%20FOC/src/drivers/BLDCDriver3PWM.h#L25)

Praktische Alternativen für die aktuellen Werte:

- `driver.voltage_limit = 3.0` bis `6.0`, wenn 2 V für den Start zu wenig sind
- `motor.velocity_limit` höher als `10.0`, falls die Drehzahlbegrenzung zu streng ist
- `motor.controller = angle`, wenn wirklich Positionsregelung gemeint ist
- `motor.controller = torque`, wenn zunächst nur elektrisches Verhalten getestet werden soll
- `motor.foc_modulation = SinePWM`, falls die Kombination aus Motor und Treiber mit SVM nicht stabil läuft
- I2C-Takt `100000` statt `400000`, falls der AS5600 instabil reagiert

### Wichtige Beobachtung

Die Datei ist in ihrer Bedeutung nicht ganz konsistent:

- Die Variable heisst `targetAngle`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L59)
- Der Hilfetext spricht von `target angle [rad]`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L126)
- Aktiv ist aber `MotionControlType::velocity`, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L81)
- `motor.move(targetAngle)` interpretiert den Sollwert damit aktuell als Geschwindigkeit, nicht als Winkel, siehe [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L136)

Für die Einordnung bedeutet das: Die Datei ist aktuell vor allem ein Bring-up- und Tuning-Sketch.

## 3. Zusammenfassung nach Hardware

### ICM-20948 über I2C

Datei:

- [src/main_icm20948_i2c.cpp](src/main_icm20948_i2c.cpp#L1)

Pins:

- `SDA = GPIO21`
- `SCL = GPIO22`
- `AD0 = GND oder 3V3`, je nach gewünschter Adresse `0x68` oder `0x69`

### ICM-20948 über SPI

Datei:

- [src/main_icm20948_spi.cpp](src/main_icm20948_spi.cpp#L1)

Pins:

- `SCK = GPIO18`
- `MISO = GPIO19`
- `MOSI = GPIO23`
- `CS = GPIO5`

### AS5600

Dateien:

- [src/main_as5600.cpp](src/main_as5600.cpp#L1)
- [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L1)

Pins:

- `SDA = GPIO21`
- `SCL = GPIO22`
- Versorgung `3V3`

### 3PWM-BLDC-Treiber

Datei:

- [src/main_simplefoc_01.cpp](src/main_simplefoc_01.cpp#L1)

Pins:

- `PWM_A = GPIO25`
- `PWM_B = GPIO26`
- `PWM_C = GPIO27`
- `EN = GPIO14`

## 4. Empfohlene praktische Reihenfolge

Wenn das Ziel die Motorinbetriebnahme ist, ergibt sich aus den vorhandenen Dateien diese sinnvolle Kette:

1. `main_as5600.cpp`: prüfen, ob der Winkelsensor stabil arbeitet
2. `main_icm20948_i2c.cpp`: prüfen, ob die ICM-20948 über I2C sauber arbeitet
3. `main_icm20948_spi.cpp`: nur falls die IMU über SPI betrieben werden soll
4. `main_simplefoc_01.cpp`: Motor, Sensor und Treiber gemeinsam hochfahren

Die aktuell aktive Build-Datei ist `main_simplefoc_01.cpp`. Siehe [platformio.ini](platformio.ini#L18).
