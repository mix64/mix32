Multi-Architecture 32-bit Operating System
===
**This project is in development.**
## Target 
* 32-bit RISC-V (RV32G)
* x86

## System Requirements
* 32-bit RISC-V : [rv32g-emulator](https://github.com/mix64/rv32g-emulator "mix64/rv32g-emulator")
* x86 : [Bochs 2.6.9](http://bochs.sourceforge.net "Bochs")

## Build
```
$ mkdir build && make
```

## Usage (x86)
1. Write .img path to **bochsrc_template.bxrc**.
2. Start bochs from that script.

## Usage (RV32G)
1. Download [rv32g-emulator](https://github.com/mix64/rv32g-emulator "mix64/rv32g-emulator") and build.
2. Give a disk image to start the emulator.

