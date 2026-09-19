# Project README

## 1. Objective

The objective of this project is to create a direct wireless control system that uses a mobile phone to turn a physical LED connected to an ESP32 microcontroller on and off.

## 2. Components Used

I used Sinric Pro because it can securely communicate with Google's servers.You need to install smart home hub on your phone where you link the Sinric Pro service and assign the ESP32 devices to a specific digital room. The voice user interface that captures your spoken commands, processes the natural language, and passes the action down to Sinric Pro.

## 3. Wiring / Circuit

Instead of building a physical circuit with switches and breadboards, I used a software-defined link by connecting the ESP32 directly to a smartphone through Wi-Fi. By routing commands digitally through apps like Blynk, Sinric Pro, and Google Home, virtual data packets completely replace wire connections. Activating a button or voice command on the phone transmits a signal to the ESP32, allowing control of its onboard hardware without any wiring.

## 4. Working Principle

The project operates by establishing a secure wireless network loop that links your smartphone, Google Assistant, and the ESP32 microcontroller over a shared Wi-Fi connection. The control cycle begins when you give a voice command or tap a virtual button on your phone, which is instantly captured and processed by the Sinric Pro smart home cloud platform. Sinric Pro translates this user action into a small digital data packet and beams it over the internet directly to the ESP32. Upon receiving the signal, the microcontroller's firmware immediately alters the electrical state of its onboard GPIO pin, switching the physical LED on or off without the need for any external wiring circuits.

By connecting your phone, Google, and the ESP32 to the same Wi-Fi network, you create a cloud, removing the need for a physical connection. Your voice command ("Hey Google...") or a digital screen tap serves as the trigger, which is collected by Sinric Pro because it connects the ESP32 to Google Home. Sinric Pro converts your command into a piece of computer code and sends it over the internet to the exact digital address of your ESP32. The ESP32 reads that incoming code packet and turns the electrical power state of the LED from 0 volts to 3.3 volts, which causes the onboard LED to light up.

## 5. Code
My code is in the Voice_control_using_ESP32.ino file in week 02

## 6. Result

Here is the link to the recording of me testing out this project,                  https://drive.google.com/file/d/1KAClYPte2DTUvOLUj1ewq0WlyGc-y5Xs/view?usp=sharing

## 7. Problems Encountered & Solutions

One problem that I faced was that when my ESP32 is powered on and connected to the internet, the Google Home app lists the LED as "Offline." I fixed this problem by going to the Google Assistant app, navigating to your linked services, and by saying the "Sync my devices" voice command to force Google to refresh its connection with the Sinric Pro servers. Another problem I faced was that Google Assistant confuses your voice command with a general internet search, or it says it doesn't understand which light you are talking about.I fixed this by giving the device a simple name inside the Sinric Pro dashboard and Google Home app. Avoid names like "ESP32_LED_01" and instead use plain human language like "The LED" so Google's natural language processing can easily identify it.

## 8. Review Video Link

- Upload the project review/demo video to Google Drive.
