# Project README

*This is a sample structure — feel free to customize it or add more sections based on your project.*

---

### 1. Objective
The objective of this project is to design and build a remote-controlled robotic system that utilizes the Blynk IoT platform to connect a ESP32 to a phone digital phone. Rather than relying on traditional, hardware-limited radio frequency controllers or short-range Bluetooth modules, this project leverages the power of Wi-Fi and cloud computing to transform a phone into a control hub for an ESP32.

---

### 2. Components Used
To build this project, I used both the Blynk website and mobile app to set up the necessary datastreams and design a dashboard featuring buttons, switches, and sliders to control the robot's arms, shoulders, hips, and wheels. I then used the Arduino IDE to program the ESP32 microcontroller, integrating the unique Blynk credentials generated during setup.Another critical component of this project is Wi-Fi. Internet connectivity is required because the Blynk app does not communicate directly with the robot; instead, they send and receive commands through a cloud-based server. When a control is adjusted on the app, the signal travels over the internet to the Blynk Cloud, which instantly routes the instruction to the ESP32’s onboard Wi-Fi module.

---

### 3. Wiring / Circuit
My circuit diagram is in the file, Sample_Wiring_or_Connection_Diagram.png in W01.
---

### 4. Working Principle
Controlling a robot with the Blynk app involves creating a wireless connection between a smartphone and a Wi-Fi-enabled microcontroller with I am using the ESP32. Using the Blynk digital dashboard, you can design a custom user interface with visual widgets such as buttons or joysticks that link to specific data streams and virtual pins. When you press a control on your phone, the command travels through the cloud via Wi-Fi to the microcontroller, which then processes the signal and directs a motor driver to move the robot forward, backward, or turn in real time.

The ESP32 has built-in Wi-Fi, allowing it to talk to the internet and your smartphone also connects to the internet. This creates a wireless bridge between your hand and the robot, meaning you don't need any physical cords or a short-range Bluetooth connection. The Blynk app acts as your remote control. Instead of buying a physical plastic remote, you drag and drop visual tools (called widgets) like buttons or joysticks onto your phone screen. Inside the app, you assign each widget a Virtual Pin. Think of a Virtual Pin as a private radio channel. When you push a button on your phone, the app packages that action into a digital data packet and sends it over the channel.The Blynk Cloud: Your phone doesn't talk directly to the robot. Instead, your phone sends the data packet up to the Blynk Cloud server, which instantly beams it down to your ESP32. Once the ESP32 receives the packet, it reads the code. It realizes you pushed the "Forward" button, so it sends electrical signals to your motor driver. The motor driver acts as the muscle, taking power from your battery pack and spinning the wheels to physically move the robot.

---

### 5. Code
My code is in the Blynk_control.ino file in week 01


---

### 6. Result
Here is the link to the recording of me testing out this project, https://drive.google.com/file/d/1Cl0t_bd9ijEQiNcXdZo_d_HX_raaWdtT/view?usp=sharing

---

### 7. Problems Encountered & Solutions
I faced the problem of trying to control the arm, shoulder, and hip motors. I over came this by changing the motor's PWM frequency and the motor's PWM response value. Another problem I faced was that sometimes the robot would disconnect, so I had to add a delay function so that it pauses the code, causes the ESP32 to miss its check-in with the server, and drops the connection.

---

### 8. Review Video Link
This is the review video link
https://drive.google.com/file/d/1Qz6o5ifk2HSCaLgdyvEOzkZWmrS3kDek/view?usp=drivesdk
