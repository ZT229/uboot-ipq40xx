## ## English ## | [中文](https://github.com/1980490718/uboot-ipq40xx/blob/master/README.md)
## U-Boot mod for IPQ40xx devices

## Introduction
This project provides a U-Boot compilation and build solution for IPQ40xx series development boards with web-based firmware upgrade functionality, supporting various board configurations.

## Quick Start

## 1. Clone source code
```bash
git clone https://github.com/1980490718/uboot-ipq40xx.git
git clone https://github.com/1980490718/openwrt-sdk-ipq806x-qsdk53
```
Modify the second line of the top-level Makefile to set `STAGING_DIR` to point to `openwrt-sdk-ipq806x/staging_dir`, then execute:
```
make
```
Or modify line 30 of the top-level `build.sh` to set `STAGING_DIR` to point to `openwrt-sdk-ipq806x/staging_dir`, then execute:
```
./build.sh cdp
```
or:
```
./build.sh ap4220
```
##  Build Output
```
make
```
Generates U-Boot as: `bin/openwrt-ipq40xx-u-boot-stripped.elf`

```
./build.sh cdp
```

Generates U-Boot as: `bin/openwrt-cdp-u-boot-stripped.elf`

```
./build.sh ap4220
```

Generates U-Boot as: `bin/openwrt-ap4220-u-boot-stripped.elf`

## Flashing U-Boot with serial interface
1.  Connect the device's LAN port to your computer via Ethernet cable and set your computer's IP to `192.168.1.2`.
2.  Connect the device's UART to your computer via a USB-to-TTL cable.
3.  Open your console software (e.g., PuTTY, minicom) and set the baud rate to `115200`.
4.  Interrupt the boot process when the device powers on.
5.  Input the following commands:
```
setenv serverip 192.168.1.2
setenv ipaddr 192.168.1.1
saveenv
```
6.  Open your TFTP software, set the TFTP server to `192.168.1.2`.
    Copy the file `openwrt-ipq40xx-u-boot-stripped.elf` to the root directory of the TFTP server.
7.  Input the following commands to backup the stock u-boot:
```
sf probe && sf read 0x84000000 0xf0000 0x80000 && tftpput 0x84000000 0x80000 stock_u-boot.bin
```
8.  Input the following commands to flash the U-Boot with web interface:
```
tftpboot 0x84000000 openwrt-ipq40xx-u-boot-stripped.elf
sf probe && sf erase 0xf0000 0x80000 && sf write 0x84000000 0xf0000 0x80000
```
Optional:
```
env default -f && saveenv
```
9.  Reset the device:
```
reset
```
10.  Wait for the console interface to restart, then enter the command 'httpd' to start the web interface, or press and hold the reset button for 3 seconds or more (choose either method).
```
httpd
```
11.  Open the browser (recommended: Google Chrome or Microsoft Edge) and enter the following URL to access the u-boot web interface:
```
http://192.168.1.1
```

## Flashing u-boot with web interface
1. Connect the device's LAN port to your computer via Ethernet cable and set your computer's IP to `192.168.1.2`.
2. press reset key for 3s or more and release it.
3. Open the browser (recommended: Google Chrome or Microsoft Edge) and enter the following URL to access the u-boot web interface:
```
http://192.168.1.1/uboot.html
```
or
```
http://192.168.1.1
```
1. Choose the openwrt-ipq40xx-u-boot-stripped.elf file and click the upgrade button to start the upgrade process.

## Firmware upgrade
1. Connect the device's LAN port to your computer via Ethernet cable and set your computer's IP to `192.168.1.2`.
2. Press reset key for 3s or more and release it.
3. Open the browser (recommended: Google Chrome or Microsoft Edge) and enter the following URL to access the u-boot web interface:
```
http://192.168.1.1/index.html
```
4. Choose the firmware (for NAND flash, accepted formats are openwrt-factory.bin/openwrt-factory.ubi/QSDK.img; for NOR flash, only openwrt-factory.bin/QSDK.img are accepted) that you want to upgrade, then click the upgrade button to start the process.
5. Waiting for the upgrade process to complete.
6. Set your computer's IP to auto.