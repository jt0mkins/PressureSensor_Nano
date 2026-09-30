"""Log the Nano's serial CSV output to a file.

Usage:  python log_pressure.py [PORT] [OUTPUT.csv]
        defaults: COM12, pressure_YYYYMMDD_HHMMSS.csv
Stop with Ctrl+C. Close the PlatformIO Serial Monitor first (only one program can open the port).
"""
import sys
from datetime import datetime

import serial

port = sys.argv[1] if len(sys.argv) > 1 else "COM12"
out_path = sys.argv[2] if len(sys.argv) > 2 else datetime.now().strftime("pressure_%Y%m%d_%H%M%S.csv")

with serial.Serial(port, 115200, timeout=2) as ser, open(out_path, "w", newline="") as f:
    f.write("pc_time,millis,raw_adc,voltage_V,pressure_bar\n")
    print(f"Logging {port} -> {out_path}  (Ctrl+C to stop)")
    rows = 0
    try:
        while True:
            line = ser.readline().decode("ascii", errors="ignore").strip()
            if not line or not line[0].isdigit():  # skip blanks and the Nano's own header line
                continue
            f.write(f"{datetime.now().isoformat(timespec='milliseconds')},{line}\n")
            f.flush()
            rows += 1
            print(line)
    except KeyboardInterrupt:
        print(f"\nStopped. {rows} rows saved to {out_path}")
