# AutoNex Bot Documentation 2025
AutoNex Bot is a 4-wheel robot DIY kit. We are participating in the WRO Future Engineers category for the year 2025.

## 1. Hardware Provided

### CVPro Controller
The **CVPro Controller** is based on the **ESP32 microcontroller** and efficiently processes sensor data.  
Features:
- **RGB LED** for status indication.
- **Start pushbutton** to initiate programmed tasks.
- **Integrated 3200 mAh battery** with a built-in charging module.

---

### Servo Motor with Ackermann Steering System
- Provides precise directional control, replicating the steering mechanism of real-world vehicles.
- Ensures smooth and efficient turns.

---

### BO Motor
- Drives the robot’s forward and backward movement.

---

### Color Sensor
- Detects colored paths or zones on the ground.
- Ideal for **line-following** or **zone-based navigation** tasks.

---

### Ultrasonic Sensors (6 Units)
- **Front (3)**: Detects obstacles ahead with enhanced accuracy.
- **Rear (1)**: Improves safety during reverse movement.
- **Left (1)** & **Right (1)**: Enables lateral obstacle detection and supports wall-following tasks.

---

### Mobile Phone Holder with OTG Cable
- Securely holds a mobile device.
- Enables applications such as **image processing**, **remote control**, or **augmented vision**.

---

## 2. Software Used

### Arduino IDE
- A cross-platform programming environment used to write, compile, and upload code to the **CVPro Controller**.

### Arduino C++
- The programming language used to develop code for the **CVPro Bot**.
- Enables control of motors, sensors, and other components.

### CVPro Android Mobile Application
- A dedicated mobile app for **computer vision–based tasks** such as object detection, tracking, and image processing.

---

## 3. Dimensions and Weight
- **Length:** 280 mm  
- **Breadth:** 190 mm  
- **Height:** 250 mm  
- **Weight:** 975 g *(without a mobile phone)*, 1,120 g *(with a mobile phone)*

---

## 4. Wiring Diagram & Components Provided

### CVPro Controller
- Central hub for controlling motors and sensors.
- Labeled ports for each connection (**S-M**, **DC-M**, **US sensors**, **CS**).

### Motors
- **Servo Motor (S-M port):** Mounted on the front axle for Ackermann steering control.
- **BO Motor (DC-M port):** Drives the rear wheels for forward/backward motion.

### Ultrasonic Sensors (US)
- **F1-US**, **F-US**, **F2-US**: Three front-facing ultrasonic sensors for obstacle detection in front.
- **B-US**: Rear-facing ultrasonic sensor for detecting obstacles behind.
- **L-US**: Left-side ultrasonic sensor for side obstacle detection.
- **R-US**: Right-side ultrasonic sensor for side obstacle detection.

### Color Sensor (CS port)
- Positioned below the front ultrasonic sensors.
- Detects surface colors for line following, object sorting, or other vision-based tasks.

### Power Supply
- **18650 Li-ion battery** powers the CVPro Controller and all connected components.
- On/Off power switch integrated into the controller.

### Wiring Layout
- **Blue cables** represent signal/power connections from the CVPro Controller to each device.
- Each sensor and motor is connected to its **dedicated labeled port** for organized wiring.

---

## License
This project is licensed under the [MIT License](LICENSE).
