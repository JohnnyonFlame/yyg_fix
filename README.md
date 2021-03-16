# What is contained in this repository?

This is a set of fixes and shim libraries for the Raspberry Pi's GameMaker runner, allowing it to boot a number of different games like Spelunky, Nuclear Throne and others, and allowing for blacklisting unintended gamepads (such as sensors, unmapped generic controllers, etc).

1) bcm_host: A shim half-implementing libbcm_host functions with SDL2.0 for OGA-like devices, forked from [gamemaker-rpi-wrapper](https://github.com/jdonald/gamemaker-rpi-wrapper).
1) yyg_fix: A series of fixes for the GameMaker runner, such as reimplementations of different GML functions, and joystick device blacklisting.
1) patch.xdelta: A patch that removes a few unecessary sanity checks on the Raspberry Pi GameMaker runner, allowing you to boot games not specifically compiled for it.

# How did you do it?

[Read my research document](RESEARCH.md) and find out!

# Usage:

Compile these libraries with your favorite toolchain, you might need to edit the Makefile to suit your toolchain:

```
$ make
```

You'll need a copy of the Raspberry Pi runners. You can find one freely available at [the yoyogames server](http://download.yoyogames.com/pi/TheyNeedToBeFed.tar.gz). These are freeware and thus should not be redistributed, specially pre-patched.

Patch your runner (in this case, you can apply an [xdelta](patch.xdelta) on the _They Need To Be Fed_ runner.

```
$ xdelta patch patch.xdelta TheyNeedToBeFed TheyNeedToBeFed_PATCHED
```

Patch it once more if you're running on 351ELEC/EMUELEC instead of ArkOS.

```
$ patchelf --set-interpreter /usr/lib32/ld-linux-armhf.so.3 TheyNeedToBeFed_PATCHED # For 351ELEC
$ patchelf --set-interpreter /usr/config/emuelec/lib32/ld-linux-armhf.so.3 TheyNeedToBeFed_PATCHED # For EMUELEC
```

If the game is running _Bytecode 16_ or higher, you'll need to downgrade your `.win`/`.unx` file using the [UndertaleModTool (UTMT)](https://github.com/krzys-h/UndertaleModTool) with the [downgrader extension](16Or17To15.csx), then save it or rename it as `game.unx`.

![](https://i.imgur.com/SExco4J.png)

Copy the generated `libcurl.so.4`, `libbcm_host.so` and `TheyNeedToBeFed_PATCHED` to the root, and the game data on a folder named `assets` (including the `.unx` file) to your device, a good install should look like this:

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
```

Now you can either launch from the terminal:

```
$ systemctl stop emustation.service
$ LD_LIBRARY_PATH=/usr/lib32:/usr/config/emuelec/lib32:$PWD LD_PRELOAD=libbcm_host.so ./TheyNeedToBeFed_PATCHED
```

Or create a launch script, like this one:

```bash
#!/bin/bash

cd "$(dirname "$0")"
export LD_LIBRARY_PATH=/usr/lib32:$PWD
export LD_PRELOAD=$PWD/libbcm_host.so
export REMAPPER=rg351p-js2xbox

~/$REMAPPER &
./TheyNeedToBeFed_patched
killall -9 $REMAPPER
```

Enjoy!

# LICENSE:

This is free software. The source files in this repository are released under the [Modified BSD License](LICENSE.md), see the license file for more information.