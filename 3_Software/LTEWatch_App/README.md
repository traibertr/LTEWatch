# LTEWatch_App:
 Smart Watch Application

## LTEWatch prototype Board v1.0:
![LTEWatch Prototype Board v1.0](/doc/img/ltewatch_img_1.png)
### Description:

### Board Pin Assignment:
| **PIN** | **I/O** | **CFG** | **SIGNAL** | **DESCRIPTION** |
|:---|:---:|:---:|:---:|:---|
| **P0.00** | DO (*Digital Out*) | - | ``M8`` | Stepper Motor driving output*D3M4* |
| **P0.01** | DO | - | ``M7`` | Stepper Motor driving output *D3COM* |
| **P0.02** | DO | - | ``M6`` | Stepper Motor driving output *D3M1* |
| **P0.03** | DO | - | ``M5`` | Stepper Motor driving output *D2M4* |
| **P0.04** | DO | - | ``M4`` | Stepper Motor driving output *D2MCOM* |
| **P0.05** | DO | - | ``M3`` | Stepper Motor driving output *D2M1* |
| **P0.06** | SO (*Serial Out*) | UART | ``USB_TX`` | *TX* signal of the *USB-to-UART* interface |
| **P0.07** | SI (*Serial In*) | UART | ``USB_RX`` | *RX* signal of the *USB-to-UART* interface |
| **P0.08** | SIO | I2C | ``SDA`` | Data line of *I2C* Bus |
| **P0.09** | CLK | I2C | ``SCL`` | Clock line of *I2C* Bus |
| **P0.10** | DI | Pull-up | ``BAT_/INT_CHRG`` | Battery charger interrupt line |
| **P0.11** | DO | - | ``BAT_LVL_EN`` | Battery level monitoring enable |
| **P0.13** | DO | - | ``M2`` | Stepper Motor driving output *D1M4* |
| **P0.14** | DO | - | ``M1`` | Stepper Motor driving output *D1COM* |
| **P0.15** | DO | - | ``M0`` | Stepper Motor driving output *D1M1* |
| **P0.16**/AIN3 | AI (*Analog In*) | No Pull | ``BAT_LVL`` | Battery level |
| P0.17 | DI (*Digital In*) | No Pull | ``BTN2`` | Push button *BT2* |
| **P0.18** | DI | No Pull | ``BTN1`` | Push button *BT1* |
| **P0.19** | DI | No Pull | ``BTN0`` | Push button *BT0* |
| **P0.20** | SO | JTAG | ``SWO`` | Serial Wire Out (SWO) line |
| **P0.21** | DO | SPI | ``ACC_CS`` | Accelerometer *Chip Select* (SPI) |
| **P0.22** | DI/O (*Digital Bidirectional*) | - | ``ACC_/INT`` | Accelerometer interrupt line |
| **P0.23** | SO | UART | GPS-TXD | GNSS *uART* TX line |
| **P0.24** | SI | UART | GPS-RXD | GNSS *uART* RX line |
| **P0.25** | DO | - | GPS-/RST | GNSS reset |
| **P0.26** | SO | - | ``DISP_EXT_COMIN`` | Display ``EXT_COMIN`` serial line |
| **P0.27** | DO | - | ``DISP_/EN`` | Display power enable |
| **P0.28** | DO | SPI | ``DISP_SCS`` | Display *Chip Select* |
| **P0.29** | SI | SPI | ``MISO`` | Serial data input signal |
| **P0.30** | CLK | SPI | ``SCK`` | Serial clock signal |
| **P0.31** | SO | SPI | ``MOSI`` | Serial data output signal |
| **COEX0** | DO | - | ``GPS-EN`` | GNSS power enable |
| **MAGPIO0** | DO | - | ``LTE_EXT_ANT`` | *LTE* external antenna enable |
| **MAGPIO1** | DO | - | ``MCU2GPS_EXTINT`` | *GNSS* receiver external interrupt |

### Build The Project

**Specification:**
- **Version**: *ncs* v2.1.2
- **IDE**: VS Code

## Firmware Description:

### LTEWatch Firmware Diagram:

![](/doc/img/software_diag.svg)

### LTEWatch Firmware Layers Decomposition Diagram:
![](/doc/img/software_layers.svg)
