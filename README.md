# 🏠 Voice and Text Command LED Control

ESP32-based smart home lighting with Bluetooth-controlled RGB LEDs for 3 rooms.

---

<table>
<tr>
<td width="50%" valign="top">

## ✨ Features

- 🎙️ Voice/Text Commands via Bluetooth
- 🎨 7 RGB Colors (Red, Green, Blue, Yellow, Purple, Cyan, White)
- 🏘️ 3 Independent Rooms (Kitchen, Living Room, Lounge)
- 💡 Individual Brightness Control
- 🌞 LDR Ambient Light Sensor
- 📱 Android/iOS Compatible

## 🔧 Hardware Required

- ESP32 Development Board
- 3x RGB LED (Common Cathode)
- LDR (Light Dependent Resistor)
- 10kΩ Resistor (LDR pull-down)
- 3x 220Ω Resistors (per LED pin)
- Breadboard & Jumper Wires

## 🔌 Hardware Connections

### Kitchen (RGB LED 1)
| ESP32 Pin | Component | Color |
|-----------|-----------|-------|
| GPIO 5 | Red | Red |
| GPIO 4 | Green | Green |
| GPIO 2 | Blue | Blue |
| GND | Common Cathode | - |

### Living Room (RGB LED 2)
| ESP32 Pin | Component | Color |
|-----------|-----------|-------|
| GPIO 14 | Red | Red |
| GPIO 12 | Green | Green |
| GPIO 13 | Blue | Blue |
| GND | Common Cathode | - |

### Lounge (RGB LED 3)
| ESP32 Pin | Component | Color |
|-----------|-----------|-------|
| GPIO 18 | Red | Red |
| GPIO 19 | Green | Green |
| GPIO 21 | Blue | Blue |
| GND | Common Cathode | - |

### LDR (Light Sensor)
| ESP32 Pin | Component |
|-----------|-----------|
| GPIO 34 | LDR (one end) |
| GND | LDR (other end - with 10kΩ) |
| 3.3V | 10kΩ resistor |

</td>
<td width="50%" valign="top">

## 🎮 Command List

### Basic Commands

| Command | Description |
|---------|-------------|
| `listen` | Activate voice mode (5 sec) |
| `help` | Show command list |
| `light measure` | Measure ambient light with LDR |

### Room Commands

**Assigning Colors to Rooms:**
```
kitchen red
living room green
lounge blue
all white
```

**On/Off:**
```
kitchen on
lounge off
all off
```

### Color Options

- 🔴 Red
- 🟢 Green
- 🔵 Blue
- 🟡 Yellow
- 🟣 Purple
- 🩵 Cyan
- ⚪ White

### Brightness Control

```
kitchen brightness decrease
lounge brightness increase
brightness raise      # Applies to all rooms
```

### Example Usage Scenarios

1. **Turn all lights white:**
   ```
   all white
   ```

2. **Make kitchen red, decrease brightness:**
   ```
   kitchen red
   kitchen brightness decrease
   ```

3. **Measure ambient light:**
   ```
   light measure
   ```

4. **Voice control for lounge:**
   ```
   listen
   lounge blue
   ```

</td>
</tr>
</table>

---

## 📱 Usage

### Bluetooth Connection

1. Connect ESP32 to your computer or power source
2. Open Bluetooth settings on your phone/tablet
3. Find and connect to "ESP32_Home_Control" device
4. Send commands using a Bluetooth terminal app

**Recommended Apps:**
- Android: [Serial Bluetooth Terminal](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal)
- iOS: [BLE Terminal](https://apps.apple.com/app/ble-terminal/id1398703795)

### Voice Command Usage

1. Type or say "listen"
2. LEDs blink to indicate listening mode
3. Give your command within 5 seconds
4. Example: "kitchen red"

## 🔍 Technical Details

### PWM Configuration
- **Frequency**: 5000 Hz
- **Resolution**: 8-bit (0-255)
- **Channel Count**: 9 (3 per LED)

### LDR Reading
- **Pin**: GPIO 34 (Analog)
- **Range**: 0-4095 (12-bit ADC)
- **Reading Period**: 5 seconds

### Timeouts
- **Voice Command Mode**: 5 seconds

## 🐛 Troubleshooting

**Cannot establish Bluetooth connection:**
- Ensure ESP32 is powered
- Check for "Bluetooth voice control active" in serial monitor
- Disconnect from other devices

**LEDs not lighting:**
- Check connections
- Verify resistor values
- Check common cathode (GND) connection

**Commands not working:**
- Type commands in lowercase
- Use help command to view available commands
- Check incoming commands in serial monitor

---

## 👤 Developer

**Beyza Nur Damar** | [GitHub](https://github.com/nurbeyzanur) | [LinkedIn](https://www.linkedin.com/in/beyzanur-damar-722203398/)

⭐ Star this project if you find it useful!
