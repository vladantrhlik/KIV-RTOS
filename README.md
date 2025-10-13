# Implementace PROC FS

Plán struktury:
 - `1/`
 - `2/`
 - ...
 - `n/` - info k tasku s PID=n
    - `n_fd` - počet otevřených souborů
    - `n_page` - počet alokovaných stránek
 - `sched` - počet runnable, blocked tasků
 - `tasks` - aktuální počet tasků, pidy tasků (?) apod.
 - `self` - aktuální proces (stejný výstup jako [aktuální pid]/)
 - `ticks` - počet tiků od startu

každý task bude mít vlastní adresář / soubor (jednodušší)


# KIV-RTOS
An educational operating system for bare-metal Raspberry Pi Zero W (BCM2835-based board).

This software is developed as a demonstration project for operating systems course on University of West Bohemia, Department of Computer Science and Engineering (KIV/OS).

### Features
- CMake-based buildsystem
- written in C/C++ and ARM assembly
- bootloader with support of image transfer via UART (to avoid "SD-card dances")
- basic kernel memory management
- support for tasks
- basic time-slicing scheduler
- drivers (GPIO, UART, AUX, timer, 74HC595 shift register, 7-segment display, I2C, SSD1306 OLED display, TRNG)
- C++ development support
- paging support
- userspace process isolation
- spinlocks, mutexes
- semaphores
- condition variables
- named pipes
- minimalistic ELF loader
- real-time process scheduler (EDF) with fallback to Round Robin

### Planned features
- userspace heap
- minimalistic standard library (and standard template library)
- SPI support
- eMMC driver
- FAT32 support
- WiFi and Bluetooth driver

## License

This software is distributed under MIT license. See attached `LICENSE` file for details.
