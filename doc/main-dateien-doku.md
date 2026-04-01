# Dokumentation der Main-Dateien in `pioduino_01`

Diese Dokumentation beschreibt nur das Projekt `pioduino_01`.
Hinweis: Einige `main`-Dateien wurden umbenannt; diese Doku spiegelt die aktuellen Dateinamen wider.
Grundlage sind die vorhandenen Dateien in:

- `platformio.ini`
- `src/main_as5600.cpp`
- `src/main_icm20948_i2c.cpp`
- `src/main_icm20948_spi.cpp`
- `src/main_simplefoc_01.cpp`

## 1. Projektueberblick

Die aktive Build-Konfiguration ist `env:esp32dev`. Ueber `build_src_filter` wird aktuell nur `main_simplefoc_01.cpp` gebaut. Die anderen `main`-Dateien sind Alternativen fuer Tests und Bring-up. Siehe [platformio.ini](C:/ttt/_embedded/pioduino_01/platformio.ini#L11C1).

Aktiv:

- `main_simplefoc_01.cpp`

Vorhandene Alternativen:

- `main.cpp`
- `main_as5600.cpp`
- `main_spi.cpp`

Verwendete Bibliotheken:

- `Simple FOC ^2.4.0`
- `SparkFun 9DoF IMU Breakout - ICM 20948 - Arduino Library ^1.3.2`

## 2. Main-Dateien im Detail

## 2.1 `src/main_as5600.cpp`

Datei: [main_as5600.cpp](C:/ttt/_embedded/pioduino_01/src/main_as5600.cpp#L1C1)

Zweck:

- einfacher I2C-Test fuer den Magnetsensor `AS5600`
- Ausgabe des Winkels in Radiant und Grad

### Verwendete Pins

- `GPIO21`: `SDA`, siehe [main_as5600.cpp](C:/ttt/_embedded/pioduino_01/src/main_as5600.cpp#L18C1)
- `GPIO22`: `SCL`, siehe [main_as5600.cpp](C:/ttt/_embedded/pioduino_01/src/main_as5600.cpp#L19C1)

### Externe Komponenten

- ESP32-Board
- `AS5600` Magnetsensor ueber I2C
- Magnet auf der Welle oder Testachse

### Wichtige Parameter

- `AS5600_I2C_ADDRESS = 0x36`, siehe [main_as5600.cpp](C:/ttt/_embedded/pioduino_01/src/main_as5600.cpp#L20C1)
- `SERIAL_BAUD = 115200`, siehe [main_as5600.cpp](C:/ttt/_embedded/pioduino_01/src/main_as5600.cpp#L21C1)
- `PRINT_INTERVAL_MS = 200`, also 5 Hz Ausgabe, siehe [main_as5600.cpp](C:/ttt/_embedded/pioduino_01/src/main_as5600.cpp#L22C1)
- I2C-Takt `400000`, siehe [main_as5600.cpp](C:/ttt/_embedded/pioduino_01/src/main_as5600.cpp#L40C1)
- Sensorobjekt `MagneticSensorI2C(AS5600_I2C)`, siehe [main_as5600.cpp](C:/ttt/_embedded/pioduino_01/src/main_as5600.cpp#L24C1)

### Sinnvolle Alternativen

- I2C-Takt `100000`, wenn das Setup mit 400 kHz instabil ist
- `PRINT_INTERVAL_MS = 50` oder `100` fuer dichtere Ausgabe
- `PRINT_INTERVAL_MS = 500` oder `1000` fuer einfache Diagnose
- statt `AS5600_I2C` auch andere vorkonfigurierte Sensoren wie `AS5048_I2C` oder `MT6701_I2C`, sofern die Hardware passt. Siehe [MagneticSensorI2C.h](C:/ttt/_embedded/pioduino_01/.pio/libdeps/esp32dev/Simple%20FOC/src/sensors/MagneticSensorI2C.h#L21C1)
- alternativ eigene Registerdefinition ueber den generischen `MagneticSensorI2C`-Konstruktor. Siehe [MagneticSensorI2C.h](C:/ttt/_embedded/pioduino_01/.pio/libdeps/esp32dev/Simple%20FOC/src/sensors/MagneticSensorI2C.h#L38C1)

### Technische Hinweise

- Diese Datei eignet sich als Vorstufe fuer `main_simplefoc_01.cpp`, weil hier nur der Sensor getestet wird.
- Geprueft wird nur die Erreichbarkeit auf `0x36` und die laufende Winkelausgabe.

## 2.2 `src/main_icm20948_i2c.cpp`

Datei: [main_icm20948_i2c.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_i2c.cpp#L1C1)

Zweck:

- I2C-Test fuer die SparkFun-IMU `ICM-20948`
- Scan des I2C-Busses beim Start
- Test beider moeglichen IMU-Adressen `0x68` und `0x69`
- Fehlerzaehler, Bus-Recovery und ESP-Neustart bei Kommunikationsproblemen
- geglaettete Teleplot-Ausgabe fuer Accel, Gyro und Magnetometer

### Verwendete Pins

- `GPIO21`: `SDA`, siehe [main_icm20948_i2c.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_i2c.cpp#L11C1)
- `GPIO22`: `SCL`, siehe [main_icm20948_i2c.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_i2c.cpp#L12C1)

### Externe Komponenten

- ESP32-Board
- SparkFun `ICM-20948` IMU ueber I2C

### Wichtige Parameter

- `I2C_FREQ_HZ = 100000`: I2C mit 100 kHz, siehe [main_icm20948_i2c.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_i2c.cpp#L13C1)
- `I2C_FAIL_THRESHOLD = 4`: nach 4 Fehlern wird eine Recovery versucht, siehe [main_icm20948_i2c.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_i2c.cpp#L14C1)
- `NO_SAMPLE_TIMEOUT_MS = 800`: wenn 800 ms lang keine Daten kommen, wird neu gestartet, siehe [main_icm20948_i2c.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_i2c.cpp#L15C1)
- `I2C_HARD_RESTART_DELAY_MS = 250`: Wartezeit vor `ESP.restart()`, siehe [main_icm20948_i2c.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_i2c.cpp#L16C1)
- `MA_WINDOW_SIZE = 10`: Fenster fuer gleitenden Mittelwert, siehe [main_icm20948_i2c.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_i2c.cpp#L17C1)
- `MA_EPSILON = 0.05`: Ausgabe nur bei relevanter Aenderung, siehe [main_icm20948_i2c.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_i2c.cpp#L18C1)
- AD0-Autodetektion ueber `initIMUWithAD0(0)` und `initIMUWithAD0(1)`, siehe [main_icm20948_i2c.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_i2c.cpp#L109C1)

### Sinnvolle Alternativen

- I2C mit `400000`, wenn Verdrahtung kurz und stabil ist
- `MA_WINDOW_SIZE = 3` bis `5` fuer schnellere Reaktion
- `MA_WINDOW_SIZE = 20` fuer ruhigere Kurven
- `MA_EPSILON = 0.01` fuer empfindlichere Ausgabe
- `NO_SAMPLE_TIMEOUT_MS = 1500` bis `3000`, wenn die IMU nicht durchgehend Daten liefert
- feste AD0-Vorgabe statt Autodetektion, falls die Beschaltung bekannt ist

### Technische Hinweise

- Die Bibliothek unterstuetzt `begin(TwoWire&, bool ad0val, uint8_t ad0pin)`. Damit sind beide AD0-Zustaende vorgesehen. Siehe [ICM_20948.h](C:/ttt/_embedded/pioduino_01/.pio/libdeps/esp32dev/SparkFun%209DoF%20IMU%20Breakout%20-%20ICM%2020948%20-%20Arduino%20Library/src/ICM_20948.h#L260C1)
- Die Recovery in dieser Datei erzeugt 9 Clock-Pulse auf `SCL`, wenn der Bus haengt. Siehe [main_icm20948_i2c.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_i2c.cpp#L88C1)

## 2.3 `src/main_icm20948_spi.cpp`

Datei: [main_icm20948_spi.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_spi.cpp#L1C1)

Zweck:

- SPI-Test fuer die SparkFun-IMU `ICM-20948`
- geglaettete Teleplot-Ausgabe
- Fehlerzaehler, SPI-Reinitialisierung und Neustart bei Stoerungen

### Verwendete Pins

- `GPIO18`: `SPI_SCK`, siehe [main_icm20948_spi.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_spi.cpp#L31C1)
- `GPIO19`: `SPI_MISO`, siehe [main_icm20948_spi.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_spi.cpp#L32C1)
- `GPIO23`: `SPI_MOSI`, siehe [main_icm20948_spi.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_spi.cpp#L33C1)
- `GPIO5`: `SPI_CS`, siehe [main_icm20948_spi.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_spi.cpp#L34C1)

### Externe Komponenten

- ESP32-Board
- SparkFun `ICM-20948` IMU im SPI-Modus

### Wichtige Parameter

- `SPI_FREQ_HZ = 4000000`, siehe [main_icm20948_spi.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_spi.cpp#L35C1)
- `SPI_FAIL_THRESHOLD = 4`, siehe [main_icm20948_spi.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_spi.cpp#L36C1)
- `NO_SAMPLE_TIMEOUT_MS = 800`, siehe [main_icm20948_spi.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_spi.cpp#L37C1)
- `SPI_HARD_RESTART_DELAY_MS = 250`, siehe [main_icm20948_spi.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_spi.cpp#L38C1)
- `MA_WINDOW_SIZE = 10`, siehe [main_icm20948_spi.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_spi.cpp#L39C1)
- `MA_EPSILON = 0.05`, siehe [main_icm20948_spi.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_spi.cpp#L40C1)

### Sinnvolle Alternativen

- andere freie `CS`-Leitung statt `GPIO5`
- andere SPI-Pinbelegung ueber `SPI.begin(sck, miso, mosi, ss)`, solange sie zum Boardlayout passt
- `SPI_FREQ_HZ = 1000000` oder `2000000`, wenn die Verbindung instabil ist
- hoehere SPI-Frequenz bei sauberer Verdrahtung; 4 MHz ist der Bibliotheks-Default. Siehe [ICM_20948.h](C:/ttt/_embedded/pioduino_01/.pio/libdeps/esp32dev/SparkFun%209DoF%20IMU%20Breakout%20-%20ICM%2020948%20-%20Arduino%20Library/src/ICM_20948.h#L264C1)

## 2.4 `src/main_simplefoc_01.cpp`

Datei: [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L1C1)

Zweck:

- Inbetriebnahme eines BLDC-Motors mit `SimpleFOC`
- Rueckfuehrung ueber `AS5600`
- 3PWM-Treiberansteuerung
- serielle Parametrierung ueber `Commander`

### Verwendete Pins

Sensor:

- `GPIO21`: `SDA`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L29C1)
- `GPIO22`: `SCL`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L30C1)

Treiber:

- `GPIO25`: PWM Phase A, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L33C1)
- `GPIO26`: PWM Phase B, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L34C1)
- `GPIO27`: PWM Phase C, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L35C1)
- `GPIO14`: Enable des Treibers, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L36C1)

### Externe Komponenten

- ESP32-Board
- `AS5600` Magnetsensor
- 3-phasiger BLDC-Motor
- externer 3PWM-BLDC-Treiber mit Enable-Eingang
- externe Motorversorgung, im Code mit `12.0 V` modelliert

### Wichtige Parameter

Motorparameter:

- `MOTOR_POLE_PAIRS = 7`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L41C1)
- `MOTOR_PHASE_RESISTANCE_OHM = 2.3`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L42C1)
- `MOTOR_KV = 220`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L43C1)
- `MOTOR_PHASE_INDUCTANCE_H = 0.00086`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L44C1)

Treiber:

- `driver.voltage_power_supply = 12.0`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L74C1)
- `driver.voltage_limit = 2.0`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L75C1)

Regelung:

- `motor.foc_modulation = SpaceVectorPWM`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L79C1)
- `motor.controller = MotionControlType::velocity`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L81C1)
- `motor.voltage_limit = 12.0`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L83C1)
- `motor.velocity_limit = 10.0`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L84C1)
- `motor.voltage_sensor_align = 1.0`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L85C1)

PID und Filter:

- `PID_velocity.P = 0.1`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L87C1)
- `PID_velocity.I = 1.0`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L88C1)
- `PID_velocity.D = 0.0`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L89C1)
- `PID_velocity.output_ramp = 100.0`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L90C1)
- `LPF_velocity.Tf = 0.02`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L91C1)
- `P_angle.P = 10.0`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L93C1)

Kommunikation:

- `T` setzt den Sollwert `targetAngle`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L126C1)
- `M` nutzt die `Commander`-Motorsteuerung, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L127C1)

### Sinnvolle Alternativen

Alternativen fuer den Motor-Konstruktor:

- `BLDCMotor(pp)`
- `BLDCMotor(pp, R)`
- `BLDCMotor(pp, R, KV)`
- `BLDCMotor(pp, R, KV, Lq, Ld)`

Die Bibliothek erlaubt diese Staffelung ueber Default-Parameter. Siehe [BLDCMotor.h](C:/ttt/_embedded/pioduino_01/.pio/libdeps/esp32dev/Simple%20FOC/src/BLDCMotor.h#L27C1)

Alternativen fuer den Motion-Control-Modus:

- `torque`
- `velocity`
- `angle`
- `velocity_openloop`
- `angle_openloop`
- `angle_nocascade`
- `custom`

Siehe [FOCMotor.h](C:/ttt/_embedded/pioduino_01/.pio/libdeps/esp32dev/Simple%20FOC/src/common/base_classes/FOCMotor.h#L49C1)

Alternativen fuer die Modulation:

- `SinePWM`
- `SpaceVectorPWM`
- `Trapezoid_120`
- `Trapezoid_150`

Siehe [FOCMotor.h](C:/ttt/_embedded/pioduino_01/.pio/libdeps/esp32dev/Simple%20FOC/src/common/base_classes/FOCMotor.h#L72C1)

Alternativen fuer den Treiber:

- `BLDCDriver3PWM(phA, phB, phC, en1)`
- `BLDCDriver3PWM(phA, phB, phC, en1, en2, en3)`

Damit sind auch getrennte Enable-Leitungen moeglich. Siehe [BLDCDriver3PWM.h](C:/ttt/_embedded/pioduino_01/.pio/libdeps/esp32dev/Simple%20FOC/src/drivers/BLDCDriver3PWM.h#L25C1)

Praktische Alternativen fuer die aktuellen Werte:

- `driver.voltage_limit = 3.0` bis `6.0`, wenn 2 V fuer den Start zu wenig sind
- `motor.velocity_limit` hoeher als `10.0`, falls die Drehzahlbegrenzung zu streng ist
- `motor.controller = angle`, wenn wirklich Positionsregelung gemeint ist
- `motor.controller = torque`, wenn zunaechst nur elektrisches Verhalten getestet werden soll
- `motor.foc_modulation = SinePWM`, falls die Kombination aus Motor und Treiber mit SVM nicht stabil laeuft
- I2C-Takt `100000` statt `400000`, falls der AS5600 instabil reagiert

### Wichtige Beobachtung

Die Datei ist in ihrer Bedeutung nicht ganz konsistent:

- Die Variable heisst `targetAngle`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L59C1)
- Der Hilfetext spricht von `target angle [rad]`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L126C1)
- Aktiv ist aber `MotionControlType::velocity`, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L81C1)
- `motor.move(targetAngle)` interpretiert den Sollwert damit aktuell als Geschwindigkeit, nicht als Winkel, siehe [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L136C1)

Fuer die Einordnung bedeutet das: Die Datei ist aktuell vor allem ein Bring-up- und Tuning-Sketch.

## 3. Zusammenfassung nach Hardware

### ICM-20948 ueber I2C

Datei:

- [main_icm20948_i2c.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_i2c.cpp#L1C1)

Pins:

- `SDA = GPIO21`
- `SCL = GPIO22`
- `AD0 = GND oder 3V3`, je nach gewuenschter Adresse `0x68` oder `0x69`

### ICM-20948 ueber SPI

Datei:

- [main_icm20948_spi.cpp](C:/ttt/_embedded/pioduino_01/src/main_icm20948_spi.cpp#L1C1)

Pins:

- `SCK = GPIO18`
- `MISO = GPIO19`
- `MOSI = GPIO23`
- `CS = GPIO5`

### AS5600

Dateien:

- [main_as5600.cpp](C:/ttt/_embedded/pioduino_01/src/main_as5600.cpp#L1C1)
- [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L1C1)

Pins:

- `SDA = GPIO21`
- `SCL = GPIO22`
- Versorgung `3V3`

### 3PWM-BLDC-Treiber

Datei:

- [main_simplefoc_01.cpp](C:/ttt/_embedded/pioduino_01/src/main_simplefoc_01.cpp#L1C1)

Pins:

- `PWM_A = GPIO25`
- `PWM_B = GPIO26`
- `PWM_C = GPIO27`
- `EN = GPIO14`

## 4. Empfohlene praktische Reihenfolge

Wenn das Ziel die Motorinbetriebnahme ist, ergibt sich aus den vorhandenen Dateien diese sinnvolle Kette:

1. `main_as5600.cpp`: pruefen, ob der Winkelsensor stabil arbeitet
2. `main_icm20948_i2c.cpp`: pruefen, ob die ICM-20948 ueber I2C sauber arbeitet
3. `main_icm20948_spi.cpp`: nur falls die IMU ueber SPI betrieben werden soll
4. `main_simplefoc_01.cpp`: Motor, Sensor und Treiber gemeinsam hochfahren

Die aktuell aktive Build-Datei ist `main_simplefoc_01.cpp`. Siehe [platformio.ini](C:/ttt/_embedded/pioduino_01/platformio.ini#L18C1).
