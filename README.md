# ctfl-rakete-pcb

Converting this sticker desing into a PCB that lights up its flame and lighthouse top from the bottom side using some LEDs and a coin cell battery.

![ctfl-rakete-pcb-front.png](img/ctfl-rakete-pcb-front.png)
![ctfl-rakete-pcb-back.png](img/ctfl-rakete-pcb-back.png)

## Implementing the colors

Produce the PCBs with **red solder mask** and **white Silkscreen**.

| color | objects | pcb layers present | notes |
| --- | --- | --- | --- |
| white | background |  | considering the background to be transparent |
| white | w_flame | F.Cu, F.Silkscreen |  |
| yellow | y_flame, y_light | none | blank FR4 should give a nice translucent yellow |
| red | r_flame | F.Mask | only red solder mask should give a nice translucent red |
| red | r_tower, r_top, r_roof | F.Mask, f.Cu | with the copper underneath, the red solder mask should appear darker than at the flame |
| light grey | w_base, w_tower | F.Cu, F.Silkscreen |  |
| dark grey | g_base | F.Cu | not differentiated from black outlines |
| black | b_contours, b_name | F.Cu, F.Mask | only copper coated with tin should give a nice contouring contrast |

### Extracting solder mask geometry (F.Mask) from the complete SVG file

In KiCAD the solder mask is expressed as an exclusion mask from the usually all present solder mask coverage.
Meaning that you define geometry where you want solder mask NOT to be.

So the gemonetry we import into KiCAD has to be the negative of the red parts of our logo.

Additionally the solder mask has to back up the silkscreen so that negative image also has to include our white parts.

I concrete terms we need to perform the following steps on our yource vector grapfics to get the desired F.Mask geometry:
1. Delete the yellow lighthous part at the top because it is clipped by all surrounding parts and itself does not clip any other.
1. Combine the white and the red flame part into one.
1. Subtract the yellow flame part from the formerly combined rest of the flame.
1. Also subtract the dark grey socket part from the remaining flame.
1. Combine all remaining red and white parts.
1. Subtract the black outlines which shall poke through the solder mask and the silk screen. Now we have our positive image of the solder mask we want (plus the outline which we also want for positioning).
1. Duplicate the outline and move the duplicate below our solder mask positive.
1. Subtract the positive from the duplicate of the outline.
1. Save as new SVG file (`F.Mask.svg`)
1. Now you can import the SVG file to the appropriate layer in the pcb layout while scaling it by **1.85**.

## Part sourcing

| partcount | reference | title | reichelt.de | mouser.de |
| --- | --- | --- | --- | --- |
| 1 | BT1 | battery clip | [KZH 20SMD](https://www.reichelt.de/knopfzellenclip-fuer-20-mm-smd-kzh-20smd-p56574.html) | [BAT-HLD-003-SMT](https://www.mouser.de/ProductDetail/TE-Connectivity-Linx-Technologies/BAT-HLD-003-SMT?qs=TuK3vfAjtkVRZQIT6eTqjQ%3D%3D) |
| 0..1 | SW1 | power switch | [SS SMD402](https://www.reichelt.de/schiebeschalter-1x-um-liegend-smd-ss-smd402-p112181.html) | [MSS-102545-14A-V-SMT](https://www.mouser.de/ProductDetail/CUI-Devices/MSS-102545-14A-V-SMT?qs=DRkmTr78QARl%2FtwuH8uRQg%3D%3D) |
| 0..1 | U1 | microcontroller | [ATTINY212](https://www.reichelt.de/8-bit-attiny-avr-risc-mikrocontroller-2-kb-20-mhz-soic-8-attiny212-ssn-p335520.html) | [ATTINY212-SSN](https://www.mouser.de/ProductDetail/Microchip-Technology/ATTINY212-SSN?qs=3HJ2avRr9PJX8cJeG3zkDw%3D%3D) |
| 14 | D? | SMD LEDs | - | [859-LTW-220DS5](https://www.mouser.de/ProductDetail/Lite-On/LTW-220DS5?qs=xb8aMrBSZRKaoLOPaZYE%252BQ%3D%3D) |
| 7 | D? | 5mm LEDs | [EVERLIGHT 334-15/X1C5-6QSA/EU](https://www.reichelt.de/led-5-mm-bedrahtet-warmweiss-7150-mcd-50--led-el-5-7150ww-p164207.html) | [941-C512AWNSCZ0B0152](https://www.mouser.de/ProductDetail/Cree-LED/C512A-WNS-CZ0B0152?qs=xEJ61ozf1a3Ja%252B55bf1fHw%3D%3D) |
| 0..1 | J1 | programming header (break off 3 of 50) | - | [437-8018305040001101](https://www.mouser.de/ProductDetail/Preci-dip/801-83-050-40-001101?qs=KBM%2FMdeLKegYqxBCoh54QQ%3D%3D) |
| 0..1 | R1 | 0805 reset pullup resistor 4.7 kOhm | - | [603-RC0805JR-134K7L](https://www.mouser.de/ProductDetail/YAGEO/RC0805JR-134K7L?qs=fRxXYNj8ftF%2Fopn8%252BQV78Q%3D%3D) |
| 0..2 | C1..2 | 0805 capacitor 100 nF | - | [80-C0805C104J5R](https://www.mouser.de/ProductDetail/KEMET/C0805C104J5RACTU?qs=gbgtKHXraGH15GhoNPXHBg%3D%3D) |
| 0..1 | C3 | 0805 capacitor 1 uF | - | [963-UMK212B7105KG-T](https://www.mouser.de/ProductDetail/TAIYO-YUDEN/UMK212B7105KG-T?qs=PzICbMaShUdi76GLXMVYcA%3D%3D) |

## Default firmware

The directory `defaultFirmware` holds an Arduino sketch that is meant to be used with the [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore) board support package for Arduino and can then be uploaded using a programmer like [SerialUPDI](https://github.com/wagiminator/AVR-Programmer/tree/master/SerialUPDI_Programmer), e.g. the [Adafruit UPDI Friend](https://learn.adafruit.com/adafruit-updi-friend).

