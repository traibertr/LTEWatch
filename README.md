# **Master Thesis - MSE Master - HES-SO**
# Analog LTE-M / NB-IoT Watch

![](1_Document/2_Report/Include/Figure/LTEWatch_img/ltewatch_img_2.png)

## Abstract:
### About Nordic Semiconductor:
  Nordic Semiconductor is a fabless semiconductor company from Norway specialized in wireless communication technologies and more particularly in Internet of things (IoT). The company is well-known for its pioneer role in ultra low power wireless solutions development such as Bluetooth Low Energy (BLE). They later implemented other technologies, such as : ANT+, Thread, Zigbee, Low power and compact LTE-M/NB-IoT cellular IoT solutions, Wi-Fi technology, GSMA.

### Goal of the project:
  Nordic Semiconductor has developed the nRF91 family of cellular LTEM/NB-IoT communication devices. The goal of this Master Thesis is to develop the prototype of an analog wristwatch using a such device. There are many challenges to overcome in this project, to name just the most difficult ones as the mechanical as well the energy consumption constraints.

## Work Done:
- [x] **Specification:** Elaborate the exact specification of the wristwatch (number of hands, battery lifetime, user interface, size, recharging mechanism, case)
- [x] **Antenna study:** The antenna of this device is particularly delicate. Since the communication is actually using 800 MHz, the antenna can not be as small as one would like it. So there has to be done a study how to solve this problems showing several innovative approaches how to solve this problem. At the conclusion of the study, a solution has to chosen for later implementation.
- [x] **Hardware conception:**
  - [x] Component selection (Cellular communication, motors and so on)
  - [x] Schematic and PCB realization of a prototype board 
  - [x] Fabrication and electrical test of the prototype
- [x] **Software development:** Design and develop a simple watch application that must implement
  - [x] Clock handles motors
  - [x] Buttons
  - [x] LCD screen
  - [x] *LTE-M/NB-IoT* data reception and transmission
  - [x] GNSS tracking and timing reception
  - [ ] Accelerometer usage
- [x] **Prototype validation:** Test of the entire development
- [x] **Documentation:** Establish a technical report enabling further developments

## To Do:
1.**Accelerometer:**
- [ ] Find a Zephyr compatible driver for the MC3635 accelerometer
- [ ] Implement accelerometer functionality in LTEWatch firmware
- [ ] Implement accelerometer wake-up function and activity tracking features
2. **LTE-M/NB-IoT Custom Antenna:**
- [ ] Look for an antenna design solution such as an antenna integrated in the watch strap or in the case
- [ ] Design and simulate a custom antenna solution
- [ ] Test custom antenna performance and validate solution
3. **Power Management and Consumption:**
- [ ] Implement low power and power management features from Zephyr and nRF Connect SDK
4. **Bluetooth & *Wifi*:**
- [ ] Integrate the nRF52840 in the hardware design to add *bluetooth* and *Wifi* features for improved connectivity
5. **Round PCB:**
- [ ] Clean LTEWatch hardware design by removing all unnecessary modules and components
- [ ] Root a compact round PCB for a smart-watch design
6. **Mechanical:**
- [ ] Design a 3D printed case for the round PCB
- [ ] Design a watch strap solution that can integrate a custom antenna

## Documentation

Full documentation about the project: [LTEWatch Report](1_Document/Traiber_MSE_TM_SA22.pdf)