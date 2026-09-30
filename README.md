# Pressure Sensor Logger (Arduino Nano)

This project reads a 0–100 bar pressure sensor with an Arduino Nano and saves the readings to a CSV file you can open in Excel.

How it works:

1. The sensor outputs a voltage between 0.5 V (0 bar) and 4.5 V (100 bar).
2. The Nano measures that voltage 10 times a second and converts it to pressure.
3. The Nano sends each reading to your PC over USB.
4. A small Python script on the PC saves the readings to a `.csv` file.

---

## What you need

**Hardware**
- An Arduino Nano. Clones with a "CH340" USB chip work fine.
- A USB cable that carries data, not just charging. If the PC doesn't detect the Nano, try a different cable first.
- A 0–100 bar pressure sensor with 0.5–4.5 V output.
- An 8.2 kΩ resistor (optional, but recommended; see Wiring).
- A breadboard and jumper wires.
- A multimeter (not essential, but very useful for troubleshooting).

**Software** (all free)
- [Visual Studio Code](https://code.visualstudio.com/) (VS Code)
- The **PlatformIO IDE** extension for VS Code
- [Python 3](https://www.python.org/downloads/) with the **pyserial** package

---

## 1. Wire it up

Do the wiring with the Nano **unplugged** from USB.

| Sensor wire | Connect to (Nano pin) |
|---|---|
| +V (power) | **5V** |
| GND (ground) | **GND** |
| Signal (output) | **A0**, through an 8.2 kΩ resistor |

- **Check the wire colours against your sensor's datasheet or label.** Sensors often use different colours from what you'd guess, for example red = +V, black = GND, and yellow, green or blue = signal.
- **The GND connection is essential.** If the sensor's ground isn't connected to the Nano's GND, the readings will drift around and be wrong.
- **The resistor goes in series:** Signal → resistor → A0. It protects the A0 pin if something is wired wrong, and it doesn't affect the readings.

---

## 2. Install the software

### VS Code and PlatformIO
1. Download and install **VS Code** from https://code.visualstudio.com/.
2. Open VS Code and click the **Extensions** icon on the left sidebar (four squares).
3. Search for **PlatformIO IDE** and click **Install**.
4. Wait for it to finish. The first install takes a few minutes and downloads extra tools; watch the bottom-right corner for progress. Restart VS Code when it asks you to.
5. When it's done, an **ant-head icon** appears on the left sidebar.

### Python and pyserial
1. Install **Python 3** from https://www.python.org/downloads/. On the first screen of the installer, tick **"Add Python to PATH"**.
2. Open a terminal (in VS Code: **Terminal → New Terminal**) and run:
   ```
   python -m pip install pyserial
   ```
   Install `pyserial`, not `serial`. They're different packages, and `serial` won't work.

---

## 3. Open the project

1. In VS Code, go to **File → Open Folder…** and choose the `PressureSensor_Nano` folder.
2. **If the folder is on a network drive,** open it through the drive letter (for example `P:\Desktop\PressureSensor_Nano`), **not** a path starting with `\\`. The compiler can't build from `\\server\...` paths.
3. PlatformIO detects the project automatically from the `platformio.ini` file.

Files in the folder:

| File | What it is |
|---|---|
| `PressureSensor_Nano.ino` | The program that runs on the Nano |
| `platformio.ini` | Settings: board type, COM port, speed |
| `log_pressure.py` | The PC script that saves readings to CSV |

---

## 4. Find your COM port

When you plug the Nano into USB, Windows gives it a **COM port** number (for example COM12). You need to tell PlatformIO which one to use.

1. Plug the Nano in.
2. Find the port in either of these places:
   - **VS Code:** click the ant-head icon → **Devices**. Look for **USB-SERIAL CH340 (COMxx)**.
   - **Windows:** right-click Start → **Device Manager** → expand **Ports (COM & LPT)**.
3. Open `platformio.ini` and set both of these lines to your port:
   ```ini
   upload_port = COM12
   monitor_port = COM12
   ```

The COM number can change if you plug the Nano into a different USB socket. If uploading suddenly stops working, check this first.

---

## 5. Upload the program to the Nano

1. Look at the **blue status bar** at the bottom of VS Code.
2. Click the **→ (right arrow)** icon, labelled **PlatformIO: Upload**.
3. Wait. The terminal should finish with:
   ```
   [SUCCESS]
   ```

You only need to do this once. The program stays on the Nano even when it's unplugged, so you only have to re-upload after changing the code.

---

## 6. Check it's working (Serial Monitor)

1. Click the **plug icon** (**PlatformIO: Serial Monitor**) in the blue status bar.
2. You should see lines like this appearing 10 times a second:
   ```
   millis,raw_adc,voltage_V,pressure_bar
   100,101.3,0.475,0.00
   200,101.2,0.475,0.00
   ```

| Column | Meaning |
|---|---|
| `millis` | Time since the Nano started, in milliseconds |
| `raw_adc` | Raw reading, from 0 to 1023 |
| `voltage_V` | Voltage at A0 |
| `pressure_bar` | Calculated pressure |

**With the sensor open to air,** the voltage should be about **0.5 V** and the pressure about **0 bar**.

3. **Close the Serial Monitor before you start logging** (step 7): click the **trash-can icon** on its terminal tab. Only one program can use the COM port at a time.

---

## 7. Log data to a CSV file

1. Make sure the Serial Monitor is closed.
2. Open a terminal in VS Code (**Terminal → New Terminal**) and run:
   ```
   python log_pressure.py
   ```
   To use a different port or filename:
   ```
   python log_pressure.py COM12 my_test.csv
   ```
3. The readings scroll past in the terminal while they're being saved.
4. Press **Ctrl+C** to stop.

The file is saved in the project folder, named like `pressure_20260929_202330.csv` (the date and time you started). You can open it in Excel.

The CSV file has an extra first column, `pc_time`, with the PC's real date and time for each reading. The `millis` column restarts from 0 every time you start the logger, because opening the port restarts the Nano.

---

## 8. Calibration (optional)

These settings are at the top of `PressureSensor_Nano.ino`:

```cpp
const float VREF      = 4.8;    // actual voltage of the Nano's 5V pin
const float V_MIN     = 0.5;    // sensor voltage at 0 bar
const float V_MAX     = 4.5;    // sensor voltage at full scale
const float P_MAX_BAR = 100.0;  // full-scale pressure
```

- **`VREF`:** use a multimeter to measure between the Nano's **5V** and **GND** pins while it's plugged into USB, and enter that value. It's often 4.6–4.9 V rather than exactly 5.
- **`V_MIN`:** if the sensor is open to air and the pressure doesn't read 0, set this to the `voltage_V` value you see at that point.
- **Using a different sensor:** change `V_MIN`, `V_MAX` and `P_MAX_BAR` to match its datasheet.

**Upload again (step 5) after changing any of these.**

---

## Troubleshooting

| Problem | What to try |
|---|---|
| Nano doesn't appear under Devices or Device Manager | Try a different USB cable, since many are charge-only. Try another USB socket. |
| `Please specify upload_port` | Set `upload_port` in `platformio.ini` (step 4). |
| `Access is denied` / port busy | Something else has the port open. Close the Serial Monitor, stop the logger, and close the Arduino IDE if it's open. |
| `stk500_getsync(): not in sync` | Change the board in `platformio.ini`: try `board = nanoatmega328`, and if that doesn't work, `board = nanoatmega328new`. Cheap clones usually need `nanoatmega328`. |
| Upload fails once, then works | This is normal on clones. Just try again. |
| `No such file or directory` during a build | PlatformIO was busy in the background. Wait a few seconds and build again. |
| Build fails on a `\\server\...` path | Open the project through a drive letter instead (step 3). |
| Pressure wanders around or reads high with nothing connected | Check the **GND** wire between the sensor and the Nano, and check the signal wire is really in **A0**. Measure the signal wire to GND with a multimeter: it should be about 0.5 V. |
| `ModuleNotFoundError: No module named 'serial'` | Run `python -m pip install pyserial`. |
