# TM_Analog_LTE-M_NB-IoT_Watch
 Master Thesis - Smart Watch
## Abstract :
Nordic Semiconductor in Norway has developed the NRF91 family of cellular LTE-M / NB-IoT communication devices. The goal of this Master Thesis is to develop the prototype of an analog wristwatch using a such device. There are many challenges to overcome in this project, to name just the most difficult ones as the mechanical as well the energy consumption constraints.

## Work to be done :
- [x] **Specification:** Elaborate the exact specification of the wristwatch (number of hands, battery lifetime, user interface, size, recharging mechanism, case)
- [x] **Antenna study:** The antenna of this device is particularly delicate. Since the communication is actually using 800 MHz, the antenna can not be as small as one would like it. So there has to be done a study how to solve this problems showing several innovative approaches how to solve this problem. At the conclusion of the study, a solution has to chosen for later implementation.
- [ ] **Hardware conception:**
  - [x] Component selection (Cellular communication, motors and so on)
  - [x] Schematic and PCB realization of a prototype board 
  - [ ] Fabrication and electrical test of the prototype
- [ ] **Software development:** Design and develop a simple watch application that must implement
  - [x] Clock handles motors
  - [ ] Buttons
  - [ ] LCD screen
  - [ ] *LTE-M/NB-IoT* data reception and transmission
  - [ ] GNSS tracking and timing reception
  - [ ] Accelerometer usage
- [ ] **Prototype validation:** Test of the entire development
- [ ] **Documentation:** Establish a technical report enabling further developments