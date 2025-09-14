<img width="815" height="533" alt="image" src="https://github.com/user-attachments/assets/603630a1-ad9e-4b6d-b927-91b095f565e9" />🔒 DigitDigital Biometric Lock System (8051)
A multi-layer secure access control system built on the AT89C51 (8051) microcontroller, combining fingerprint, keypad password, RFID, and a hidden secret PIN. It drives a DC motor to simulate a door/gate, shows real-time status on an I²C 16x2 LCD, and triggers a buzzer on unauthorized attempts.

---
Features

🔐 Fingerprint authentication as the primary access method

🔢 5‑digit password via 4x3 matrix keypad

📶 RFID fallback after failed password attempts

🕵️ Secret PIN backup if RFID is unavailable

🔔 Buzzer alert on invalid attempts

🖥️ I²C LCD with user-friendly prompts

⚙️ DC motor control for gate/door simulation

---
Authentication Flow

Step 1 — Fingerprint: Place finger on sensor. If valid, proceed to Password.

Step 2 — Password: Enter 5‑digit password 1234*. Three attempts are allowed. If correct, access is granted and the gate opens.

Step 3 — RFID Fallback: After three wrong passwords, scan RFID. The system compares against the stored tag 53000C21156B. Valid tag grants access; invalid triggers the buzzer.

Step 4 — Secret PIN Backup: If RFID is skipped/unavailable, enter secret PIN *2468. Correct PIN grants access; invalid triggers the buzzer.

---
Hardware
AT89C51 microcontroller (8051 family)

4x3 matrix keypad

UART‑based RFID reader

Fingerprint sensor (simulated via digital input)

16x2 LCD with I²C interface module

Buzzer connected to P2.0

DC motor + driver circuit (gate/door simulation)

---
Software Stack
Language: Embedded C

IDE/Compiler: Keil µVision

Optional simulation: Proteus

Libraries:

reg51.h (8051 core definitions)

UART routines (RFID communication)

I²C routines (LCD interface)

---
Key Functions
wait_for_fingerprint() → Blocks until a valid touch is detected

get_password() → Captures a 5‑digit entry via keypad

check_password() → Compares input with stored password

ask_for_RFID() → Prompts for RFID or fallback selection

get_RFID() → Reads a 12‑byte RFID via UART

check_RFID() → Validates scanned RFID against stored tag

ask_for_SecretPIN() → Requests the hidden emergency PIN

activate_buzzer() → Alerts on invalid attempts or access denial

lcd_cmd(), lcd_data(), lcd_string() → I²C LCD control utilities

---
Example Workflow
Power ON → LCD: “Enter Fingerprint”

Valid fingerprint → LCD: “Fingerprint OK”

Enter password 1234* → Access granted → Motor runs to open gate

Three wrong passwords → LCD: “Enter RFID”

Scan RFID

If valid (53000C21156B) → Access granted

If invalid → Buzzer rings

Skip RFID → Enter secret PIN *2468

If valid → Access granted

Else → Buzzer ringsal Biometric Lock System (8051 Microcontroller)



Developed by Dhananjai
📌 Secure Access Control System with Multi-Level Authentication

---

