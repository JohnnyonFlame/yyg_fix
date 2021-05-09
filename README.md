# What is contained in this repository?

This is a set of fixes and shim libraries for the Raspberry Pi's GameMaker runner, allowing it to boot a number of different games like Spelunky, Nuclear Throne and others, and allowing for blacklisting unintended gamepads (such as sensors, unmapped generic controllers, etc).

1) [sdl2_host](src/backends/sdl2_host.c): A shim half-implementing libbcm_host functions with SDL2.0 for OGA-like devices, forked from [gamemaker-rpi-wrapper](https://github.com/jdonald/gamemaker-rpi-wrapper).
2) [bcm_host](src/backends/bcm_host.c): The half-proprietary DispmanX graphics interfacing library for the Raspberry Pi, modified to inject functions into the GameMaker runner, forked from the Raspberry Pi [userland](https://github.com/raspberrypi/userland) repository.
3) [yyg_fix](src/yyg_fix.c): A series of fixes for the GameMaker runner, such as reimplementations of different GML functions, and joystick device blacklisting.
4) [XDelta patch folder](patches/xdelta) or [IPS patch folder](patches/ips): A collection of patches that removes a few unecessary sanity checks on the Raspberry Pi GameMaker runner, allowing you to boot games not specifically compiled for it, as well as redirecting a couple of dynamic links, depending on what device you're running on (whether it be an OGA-like device or the Raspberry Pi itself).
5) [16Or17To15](scripts/16Or17To15.csx): An experimental extension for downgrading `.win` and `.unx` GameMaker data files to v1.4.1084.

# How did you do it?

[Read my research document](docs/RESEARCH.md) and find out!

# Dependencies needed:

You'll need to install the following dependencies to use this project:
- `libsdl2-dev` (if using the SDL2 backend)
- `libraspberrypi0-dev` (if on Raspberry Pi and using the DispmanX backend)
- `xdelta`
- `libopenal-dev`
- `patchelf`

# Usage:

Either download [a binary release](https://github.com/JohnnyonFlame/yyg_fix/releases/latest) from the releases page, or compile them with your favorite toolchain (you might need to edit the Makefile accordingly):

```
$ make
```

There are now several different backends you can choose from:

Backend | Description
--------|------------
SDL2 | The default backend using SDL 2.0. Makes it possible to run the GameMaker runner on many other ARM devices.
DispmanX | The half-proprietary graphics driver for the Raspberry Pi series of SBCs. Append `LEGACY_RPI=1` after the `make` command to enable.

If you plan on running this project on an OGA-like device, you'll need to compile a program called `rg351p-js2xbox` and place it in your home directory. This is a button remapper that makes it possible to use the buttons on these devices with standardized game controls. You can find this program [here](https://github.com/lualiliu/RG351P_virtual-gamepad).

You'll also need a copy of the Raspberry Pi runners. You can find one freely available at [the yoyogames server](http://download.yoyogames.com/pi/TheyNeedToBeFed.tar.gz), (you might need to _right click_ and select _save as_ on some browsers). These are freeware and thus should not be redistributed, specially pre-patched.
 
Patch your runner. In this case, you can apply one of the [ips patches](patches/ips) with [an online tool](https://www.marcrobledo.com/RomPatcher.js/) or one of the [XDelta patches](patches/xdelta) with the following command on linux:

### If using a Raspberry Pi:

```
$ xdelta patch patches/xdelta/raspi.xdelta TheyNeedToBeFed runner_raspi
```

### If using any other ARM based/OGA-like device (or if you're having trouble using the DispmanX drivers):

```
$ xdelta patch patches/xdelta/armlinux.xdelta TheyNeedToBeFed runner_arm
```

---
If the game is running _Bytecode 16_ or higher, you'll need to downgrade your `.win` or `.unx` file using the [UndertaleModTool (UTMT)](https://github.com/krzys-h/UndertaleModTool) with the [downgrader extension](https://raw.githubusercontent.com/JohnnyonFlame/yyg_fix/master/16Or17To15.csx), the file must be renamed/saved as `game.unx` - regardless of version.

![](https://i.imgur.com/SExco4J.png)

---
Copy the folder named `assets` (including the `.unx` file) from your original game folder to the root of whichever folder you'd like the game to be in on your device, then copy the `lib` folder from this repository and `runner_arm` / `runner_raspi` (depending on which patch you used) to that same new game folder.

Next, create the `run.sh` script in your game folder, like this one (remember to uncomment the commented lines for your device/OS type):

```bash
#!/bin/bash

# Uncomment the commented lines depending on your OS and/or device of choice

cd "$(dirname "$0")"
#export LD_LIBRARY_PATH=/usr/lib32:/usr/config/emuelec/lib32:$PWD/lib   # For 351ELEC/EMUELEC
#export LD_LIBRARY_PATH=$PWD/lib:$LD_LIBRARY_PATH   # For Raspberry Pi and other ARM Devices
export LD_PRELOAD=$PWD/lib/libbcm_host.so
#export REMAPPER=rg351p-js2xbox   # For OGA-like devices

#~/$REMAPPER &   # For OGA-like devices
#./runner_raspi   # For Raspberry Pi devices
#./runner_arm   # For OGA-like and other ARM devices
#killall -9 $REMAPPER   # For OGA-like devices
```

Finally, if you are using 351ELEC/EMUELEC, run the following command:

### If using 351ELEC:

```
$ patchelf --set-interpreter /usr/lib32/ld-linux-armhf.so.3 runner_arm
```

### If using EMUELEC:

```
$ patchelf --set-interpreter /usr/config/emuelec/lib32/ld-linux-armhf.so.3 runner_arm
```
---
## You can also test it via SSH:

### If EmulationStation is running, kill it:
```
$ systemctl stop emustation.service
```

### If you're using an OGA-like device and/or are using 351ELEC/EMUELEC, run:
```
$ LD_LIBRARY_PATH=/usr/lib32:/usr/config/emuelec/lib32:$PWD LD_PRELOAD=libbcm_host.so ./runner_arm
```

### Otherwise, run:
```
$ export LD_LIBRARY_PATH=$PWD/lib:$LD_LIBRARY_PATH LD_PRELOAD=lib/libbcm_host.so ./runner_<raspi or arm>
```

---
# Example installation:
### We'll be using Nuclear Throne as an example:

A good install, which will allow you to launch these games from EmulationStation, should look like this:

```
$ find /roms/ports/nthrone
/roms/ports/nthrone
/roms/ports/nthrone/assets
/roms/ports/nthrone/assets/<many ogg files>.ogg
/roms/ports/nthrone/assets/splash.png
/roms/ports/nthrone/assets/game.unx
/roms/ports/nthrone/run.sh
/roms/ports/nthrone/lib
/roms/ports/nthrone/lib/libcurl.so.4
/roms/ports/nthrone/lib/libcrypto.so.1.0.2
/roms/ports/nthrone/lib/libcurl.so.3
/roms/ports/nthrone/lib/libcurl.so.4.4.0
/roms/ports/nthrone/lib/libssl.so.1.0.2
/roms/ports/nthrone/lib/libbcm_host.so
/roms/ports/nthrone/lib/openssl-1.0.2
/roms/ports/nthrone/lib/openssl-1.0.2/engines
/roms/ports/nthrone/lib/openssl-1.0.2/engines/libubsec.so
/roms/ports/nthrone/lib/openssl-1.0.2/engines/libpadlock.so
/roms/ports/nthrone/lib/openssl-1.0.2/engines/libaep.so
/roms/ports/nthrone/lib/openssl-1.0.2/engines/libatalla.so
/roms/ports/nthrone/lib/openssl-1.0.2/engines/libcapi.so
/roms/ports/nthrone/lib/openssl-1.0.2/engines/libcswift.so
/roms/ports/nthrone/lib/openssl-1.0.2/engines/libnuron.so
/roms/ports/nthrone/lib/openssl-1.0.2/engines/lib4758cca.so
/roms/ports/nthrone/lib/openssl-1.0.2/engines/libsureware.so
/roms/ports/nthrone/lib/openssl-1.0.2/engines/libchil.so
/roms/ports/nthrone/lib/openssl-1.0.2/engines/libgmp.so
/roms/ports/nthrone/lib/openssl-1.0.2/engines/libgost.so
/roms/ports/nthrone/runner_<raspi or arm>
```

### Well, that's all! Enjoy!

# LICENSE:

This is free software. The source files in this repository are released under the [Modified BSD License](LICENSE.md), see the license file for more information.

This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit (http://www.openssl.org/). See the [license list](lib/README.md) in the libraries folder for more information.
