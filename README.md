
# Motor Monitor Arduino

Arduino code for the Induction Motor Monitoring System. This uses an ATMega board for the sensor readings and pre-processing, and ESP-8266 microchip, which handles the API request with the `motor-monitor-backend` server. The communication from these chips is handled via serial.

To view sensor readings and analysis, visit the [web app](https://motor-monitor-frontend.vercel.app/).

## Separation of Concerns

- `motor-monitor-mega` - handles the sensor readings. Transmits readings via `Serial3` from `TXD3` (Pin 14 of Mega).
- `motor-monitor-esp` - handles the WiFi capablities to send POST requests to the server. Receives readings via `Serial` from `RXD` (Pin 1 of ESP-8266).


## Installation

    1. Upload `motor-monitor-mega` to ATMega board with DIP switches 3 and 4 on.
    2. Upload `motor-monitor-esp` to Generic ESP-8266 Module with DIP Switches 5,6, and 7 on. Make sure that TXD3 (Pin 14) and RXD are disconnected before upload.
    3. Connect TXD3 and RXD. Only DIP switches 5 and 6 are on.
    
## Features

- 3-Line Voltage Sensors
- 3-Line Current Sensors
- Temperature Sensor
- WiFi-enabled for wireless capability


## API Reference

#### Post metrics

```http
  POST /api/v1/metrics/${unit_id}
```

| Query Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `line1Voltage` | `float` | Voltage reading on line 1 |
| `line2Voltage` | `float` | Voltage reading on line 2 |
| `line3Voltage` | `float` | Voltage reading on line 3 |
| `line1Current` | `float` | Current reading on line 1 |
| `line2Current` | `float` | Current reading on line 2 |
| `line3Current` | `float` | Current reading on line 3 |
| `temperature` | `float` | Temperature reading in Celsius |


## Authors

- [Engr. Kirk Alyn Santos](https://github.com/kirkalyn13)
- Kurt Avery Santos


## License

[MIT](https://choosealicense.com/licenses/mit/)
