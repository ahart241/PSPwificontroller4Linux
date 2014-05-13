PSPwificontroller4Linux
=======================

PSP wificontroller server for Linux

Author: Andrew Hart <ahart241@gmail.com>

# Requirements

Linux kernel >=2.6.12-rc2 with uinput and joystick support

## Installation Instructions

## Compiling

gcc ./src/wifiserver.c

## Installing server

cp ./src/a.out /usr/bin/wifiserver

## Setting Server to Run at Startup

## Configuring Udev

cp ./udev-rules/90-uinput.rules /etc/udev/rules.d

# Testing Instructions
