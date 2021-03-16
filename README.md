# What is contained in this repository?

This is a set of fixes and shim libraries for the Raspberry Pi's GameMaker runner, allowing it to boot a number of different games like Spelunky, Nuclear Throne and others, and allowing for blacklisting unintended gamepads (such as sensors, unmapped generic controllers, etc).

1) [bcm_host](bcm_host.c): A shim half-implementing libbcm_host functions with SDL2.0 for OGA-like devices, forked from [gamemaker-rpi-wrapper](https://github.com/jdonald/gamemaker-rpi-wrapper).
1) [yyg_fix](yyg_fix.c): A series of fixes for the GameMaker runner, such as reimplementations of different GML functions, and joystick device blacklisting.
1) [patch.xdelta](patch.xdelta) or [patch.ips](patch.ips): A patch that removes a few unecessary sanity checks on the Raspberry Pi GameMaker runner, allowing you to boot games not specifically compiled for it.
1) [16Or17To15](16Or17To15.csx): An experimental extension for downgrading `.win` and `.unx` GameMaker data files to v1.4.1084.

# How did you do it?

[Read my research document](RESEARCH.md) and find out!

# Usage:

Either download [a binary release](https://github.com/JohnnyonFlame/yyg_fix/releases/latest) from the releases page, or compile them with your favorite toolchain (you might need to edit the Makefile accordingly):

```
$ make
```

You'll also need a copy of the Raspberry Pi runners. You can find one freely available at [the yoyogames server](http://download.yoyogames.com/pi/TheyNeedToBeFed.tar.gz), (you might need to _right click_ and select _save as_ on some browsers). These are freeware and thus should not be redistributed, specially pre-patched.
 
Patch your runner. In this case, you can apply the [ips patch](patch.ips) with [an online tool](https://www.marcrobledo.com/RomPatcher.js/) or the [xdelta](patch.xdelta) one with the following command on linux:

```
$ xdelta patch patch.xdelta TheyNeedToBeFed TheyNeedToBeFed_PATCHED
```

If the game is running _Bytecode 16_ or higher, you'll need to downgrade your `.win`/`.unx` file using the [UndertaleModTool (UTMT)](https://github.com/krzys-h/UndertaleModTool) with the [downgrader extension](16Or17To15.csx), then save it or rename it as `game.unx`.

![](https://i.imgur.com/SExco4J.png)

Copy the generated `libcurl.so.4`, `libbcm_host.so` and `TheyNeedToBeFed_PATCHED` to the root, and the game data on a folder named `assets` (including the `.unx` file) to your device, and then create the `launch.sh` script on your game folder, like this one (remember to uncomment the patchelf lines if necessary):

```bash
#!/bin/bash

cd "$(dirname "$0")"
export LD_LIBRARY_PATH=/usr/lib32:/usr/config/emuelec/lib32:$PWD
export LD_PRELOAD=$PWD/libbcm_host.so
export REMAPPER=rg351p-js2xbox

# Uncomment either of these lines depending on your OS of choice
# patchelf --set-interpreter /usr/lib32/ld-linux-armhf.so.3 TheyNeedToBeFed_PATCHED # For 351ELEC
# patchelf --set-interpreter /usr/config/emuelec/lib32/ld-linux-armhf.so.3 TheyNeedToBeFed_PATCHED # For EMUELEC

~/$REMAPPER &
./TheyNeedToBeFed_patched
killall -9 $REMAPPER
```

A good install, which will allow you to launch these games from EmulationStation, should look like this:

```
$ find /roms/ports/nthrone
/roms/ports/nthrone
/roms/ports/nthrone/assets
/roms/ports/nthrone/assets/<many ogg files>.ogg
/roms/ports/nthrone/assets/splash.png
/roms/ports/nthrone/assets/game.unx
/roms/ports/nthrone/libbcm_host.so
/roms/ports/nthrone/libcurl.so.4
/roms/ports/nthrone/TheyNeedToBeFed_PATCHED
/roms/ports/nthrone/launch.sh
```

You can also test it via SSH, using one of the two patchelf lines (if you're either on 351ELEC or EMUELEC):

```
$ patchelf --set-interpreter /usr/lib32/ld-linux-armhf.so.3 TheyNeedToBeFed_PATCHED # For 351ELEC
$ patchelf --set-interpreter /usr/config/emuelec/lib32/ld-linux-armhf.so.3 TheyNeedToBeFed_PATCHED # For EMUELEC
$ systemctl stop emustation.service
$ LD_LIBRARY_PATH=/usr/lib32:/usr/config/emuelec/lib32:$PWD LD_PRELOAD=libbcm_host.so ./TheyNeedToBeFed_PATCHED
```

Enjoy!

# LICENSE:

This is free software. The source files in this repository are released under the [Modified BSD License](LICENSE.md), see the license file for more information.