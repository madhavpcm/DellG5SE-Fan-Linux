# DellG5SE-Fan-Linux
The Dell G5SE-5505 laptop isn't working with usual fan managers, this script is a small utility to automatically set fan speed according to cpu and gpu thermals.
## Requirements and Setup
Qt5 libraries are needed for this version.Make sure either k10temp or zenpower (zenmonitor) kernel modules are loaded (by default k10temp is loaded). 
You will also need to modprobe the dell smm kernel module, which is not loaded by default on this laptop. 
```shell
$ sudo modprobe dell-smm-hwmon restricted=0 ignore_dmi=1
sudo modprobe ec_sys write_support=1 
```
If you want this setting to stay upon reboot, you can create a / replace by or append to the config file  `/etc/modules-load.d/dell-smm-hwmon.conf` 
```shell
dell-smm-hwmon
ece_sys
```
and the same for `/etc/modprobe.d/dell-smm-hwmon.conf` 
```shell
# This file must be at /etc/modprobe.d/
options dell-smm-hwmon restricted=0 ig
options ec_sys write_support=1
```
## Usage
Original version ported to Qt with CLI and GUI mode
# CLI
```$ fans -h
Usage :
sudo fans -no-gui [-s left_fan_speed right_fan_speed] [-l t1 t2 t3 t4] [-t timer] [-r ] [-b]

Arguments description :
 -h, --help             show this help message and exit.
 -s, --set left right   sets left/rights fan to selected speed (from 0 to 255).
 -l, --loop t1 t2 t3 t4 given the temperature thresholds t1, t2, t3, t4 (in °C),
                        adjusts the fan speed accordingly with a loop.
 -t, --timer t          set the loop timer (in seconds). Default is 20s.
 -m, --manual           Switch to manual fan mode.
 -r, --restore          Gives back the fan control to the BIOS.
 -b, --boost            Set fan speed to BOOST (as fn+F7).
 -v, --verbose          Prints loop fan updates. -1 means no fan update made.

```
# GUI
```$ sudo fans```
will start the GUI app. In Toggle mode Rotate the left and right slider for GPU and CPU fans accordingly. Or, select Graph mode from the menu and set the graph shape according to your wishes.

:warning: Be careful ! Too high temperatures can permanently damage your hardware.

For a better ease of use, you can put this script in your binaries path `/usr/bin/DellFan` or any custom location which is in your $PATH$. For `sudo`  to not ask your password everytime you use, copy the following :
```
ALL ALL=(root) NOPASSWD: /usr/bin/DellFan
```
to your `sudoers` file.

This also should allow you to map profiles / fan speeds to keyboard shortcuts via your display environment (gnome / kde / ...).

:warning: Be careful ! Too high temperatures can permanently damage your hardware.


## Todo
- [x] Make a config file
  - [ ] Live update on temp changes in config file
- [x] Cpp version 
  - [] gui
    - [x] Temperature monitoring, temp graphs.
    - [ ] Update profile / Fan speed.
    - [ ] Keyboard shortcut,
- [x] Direct EC editing.
  - [x] GPU part.
  - [x] CPU part. Need to find a way to disable bios fan control. 
- [ ] Tell me 
