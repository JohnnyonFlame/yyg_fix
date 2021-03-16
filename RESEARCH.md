# A tale about running your gosh darn GameMaker games on your favorite OGA Like.

A fair warning: While I think you as user should be allowed AND encouraged to:

- Choose how, when and where you consume *your* _legally_ obtained games.
- Mod your games however you see fit, within reason (don't cheat online dummy).
- Poke and prod at *anything* that is running on your machine, both for fun and protecting yourself.

I still don't condone using this research to distribute your own games. Acquire a [_GameMaker: Studio 2.0 (GM:S 2.0)_](https://www.yoyogames.com/get) license or help the community develop an open-source runner with the available documentation instead. Support your developers.

## Tools required to reproduce this project:

We'll be using a set of different tools in order to reverse engineer and patch different files. 

First, we'll need [Ghidra](https://ghidra-sre.org/) - a feature-rich, open-source reverse engineering stack - courtesy of the NSA. We'll use this tool to reverse engineer ARM Linux [.ELF](https://pt.wikipedia.org/wiki/Executable_and_Linkable_Format) binary files.

We'll also employ the amazing [UndertaleModTool (UTMT)](https://github.com/krzys-h/UndertaleModTool) tool developed by the folk at _The Underminers Community_ in order to study and patch _GM:S_ data files (such as _.win_ and _.unx_ files).

## Introduction:

Let's say you own one of the many [Homebrew-focused Linux handhelds](https://www.reddit.com/r/SBCGaming/) such as the _Odroid Go Advance (OGA)_, _Odroid Go Super (OGS)_ or one of the many _OGA_-Likes such as the _Anbernic RG351P_, _RK2020_, etc. Wouldn't it be neat to play, say, Nuclear Throne or Spelunky on these little devices? Well, I agree with you.

![](https://i.imgur.com/rQZYLcL.jpg)

In a previous episode, we determined that [emulating x86 runners is indeed possible](https://github.com/JohnnyonFlame/axe11), but even the [_YoYo Compiler (YYC)_](https://help.yoyogames.com/hc/en-us/articles/216753638-Setting-up-GameMaker-Studio-for-YoYoCompiler-Platforms) versions, which are natively compiled unlike the interpreted [_Virtual Machine (VM)_ bytecode](https://techterms.com/definition/bytecode) [versions](https://github.com/krzys-h/UndertaleModTool/wiki/Bytecode-version-differences), can be a tidy bit slow. Luckily, I still have an old beta runner back in very early versions that was bundled in the [_Raspberry Pi_ versions of _They Need To Be Fed_](http://download.yoyogames.com/pi/TheyNeedToBeFed.tar.gz), _Maldita Castilla_ and others... but can it be made to run these games?

## Early Research:

First of all, let's see if we can even run the _Raspberry Pi_ runner on our _351ELEC 2.0_ flashed _RG351P_, which is necessary in order to execute the aforementioned GameMaker bytecode:

```bash
$ LD_LIBRARY_PATH=/usr/lib32 ./runner
./runner: error while loading shared libraries: libcurl.so.4: wrong ELF class: ELFCLASS64
```

No good. Let's try to fool the runner for now, on your cross-compiling machine:

```bash
touch empty.c
arm-linux-gnueabihf-gcc -c empty.c -o empty.o -fPIC
arm-linux-gnueabihf-gcc -fPIC -shared -Wl,-soname,libcurl.so.4 empty.o -o libcurl.so.4
```

Neat. But then you get:

```bash
LD_LIBRARY_PATH=/usr/lib32:. ./runner
./runner: error while loading shared libraries: libbcm_host.so: cannot open shared object file: No such file or directory
```

The library missing here is `libbcm_host.so`, which includes a series of _Broadcom_ specific functionality used on the _Raspberry Pi_, such as the [DispmanX](https://jan.newmarch.name/RPi/Dispmanx/) routines, which GameMaker uses to interface with the GPU. We'll need a [_shim library_](https://en.wikipedia.org/wiki/Shim_(computing)) that implements some of the missing functionality.

Thankfully, someone did the [hard work](https://github.com/jdonald/gamemaker-rpi-wrapper/) for us (Thank you!), we just need to do [some refactoring](bcm_host.c) to adapt it to [_Simple DirectMedia Layer (SDL2.0)_](https://www.libsdl.org/). Now we can piggy back off of the [_librga_ blitter](https://forum.odroid.com/viewtopic.php?f=194&t=38045) enabled _SDL2.0_ libraries present on these devices, since we do have these pesky rotated screens on some of them.

```bash
$ LD_LIBRARY_PATH=/usr/lib32:. LD_PRELOAD=libbcm_host.so ./runner
./runner: ./libcurl.so.4: no version information available (required by ./runner)
librga:RGA_GET_VERSION:4.00,4.000000
ctx=0xcc8098,ctx->rgaFd=3
Rga built version:8827628f

***************************************
*     YoYo Games Raspberry Pi Runner V0.1    *
***************************************
```

![](https://i.imgur.com/u7WQruH.jpg)

The result is a successful boot to the main menu. Neat!

## Expanding our horizons:

The _Raspberry Pi_ demos are neat by themselves, containing gems such as Locomalito's amazing _Maldita Castilla_, but maybe we can expand our horizons a bit here? We'll attempt this by first choosing a donor game that runs on a similar version as the _Raspberry Pi_ runner. There are some amazing GM modding tools that can help us determine this. Here we'll be using the aforementioned _UTMT_ tool. 

First, we know from some [previous literature](https://github.com/Kneesnap/deltarune-circuit-sword) that we're dealing with something akin to _GMS1 v466_, which is bytecode 15. So grab your itch.io, GOG or Steam library and let's look for something that fits the bill.

![](https://i.imgur.com/fW7FryI.png)

Bingo. Let's see where this gets us.

```bash
$ LD_LIBRARY_PATH=/usr/lib32:. LD_PRELOAD=libbcm_host.so ./runner
<...>
**********************************.
Entering main loop.
**********************************.
alarm 0
username:
glGenFramebuffersOES(1, 0x10ef5b8);
glFramebufferTexture2DOES(36160, 36064, 3553, 4, 0);

GameMaker runner closed
```

Crap. Let's ask _Ghidra_ what it thinks about the "GameMaker runner closed" string.

![](https://i.imgur.com/2d6Mfo0.png)

Good, that's a hit. Let's follow the [_Cross Reference (XRef)_](https://resources.infosecinstitute.com/topic/ida-cross-references-xrefs/) to see what references this string constant.

![](https://i.imgur.com/5F5w15H.png)

There's what looks to be an event-display loop, and apparently _Ghidra_ thinks the runner is just lazy, therefore we'll encourage it with a healthy dosage of `while (true);` by patching the [_Branch on Not Equal_ to an _Unconditional Branch_](https://azeria-labs.com/arm-conditional-execution-and-branching-part-6/).

![](https://i.imgur.com/re9WxXK.jpg)

Success! Or at least a partial one. The game becomes very confused if you try exiting out of it, we'll now undo the patch, rename _DAT\_0043f1f4_ to something meaningful like _yyg\_alive_ and double click it to dive deep on the XRefs to see what's up.

![](https://i.imgur.com/EIkm7SU.png)

Process here is a bit laborious - you'll need to go through the list of entries where code references the data (such as _FUN\_00176d28:00176d34(*)_) and where code writes to this data (such as _FUN\_001768ec:00176978(W)_). It should be a simple matter of patching a few _Unconditional Branches_ to test your assumptions. 

In the end, it seems to be just a couple of very simple platform sanity checks. This is very useful practice however, specially for beginners, we'll leave it as an exercise to the reader.

## Yes, but why won't controllers work?

First thing you notice is that, despite GM:S finding your controllers, you can't do shit with them. Maybe _Ghidra_ knows something? Let's see what it knows about the string `/dev/input/js#`. To keep it brief, you'll notice that:
- YYG's runner spams `open(..., "/dev/input/js###")` four times per frame, every frame. This is also true for the x86 runner.
- YYG's documentation states that [gamepads 0 to 3](https://docs2.yoyogames.com/source/_build/3_scripting/4_gml_reference/controls/gamepad%20input/index.html) are XInput devices, and DInput for later ones. This is incorrect for Linux runners. Have fun with hdd gsensors as gamepads I guess.

![](https://i.imgur.com/rNQUQmW.png)

A stopgap solution for now is hooking the [`open` std function](yyg_fix.c) using LD_PRELOAD, and subtracting from the joystick index whenever we go over a blacklisted one. This allows the runner to finally slot the correct controllers in the right places. There are probably better solutions, but there will be time for that later.

This allows us to finally play _Nuclear Throne_ at fullspeed, but you'll notice some broken renderbuffer effects such as lights. We'll ignore them for now.

## Expanding our horizons even further:

This is great, but there are plenty of other amazing titles out there, such as [Spelunky](https://github.com/yancharkin/SpelunkyClassicHD/releases) or [Undertale](https://store.steampowered.com/app/391540/Undertale) which are, unfortunally, locked behind Bytecode 16+ versions... But what if...

```
[20:17] Johnny on Flame: this is one of these oddball questions but still worth a shot, has anyone tried to decompile something that's using newer bytecode [e.g. 17] and recompile it on older bytecode formats [e.g. 15]?
[21:01] Grossley: Yeah I've done that before shouldn't be too hard
```

Enter _UTMT_ again. Within a few moments _Grossley_ had an initial implementation of the [envisioned extension](16Or17To15.csx). There's beauty on well researched, documented, smart tooling that can be extended easily with a few scripts. Having a fast iteration cycle is important for research, and being able to extend your tools directly with simple scripts should never be taken for granted.

![](https://i.imgur.com/SExco4J.png)

Applying this tool to _Spelunky_ gets us far into booting the game, where we smash heads first into another wall:

```
$ LD_LIBRARY_PATH=/usr/lib32:. LD_PRELOAD=libbcm_host.so ./runner
<...>
___________________________________________
Error on load
Unable to find function bool
ERROR!!! :: Error on load
Unable to find function bool
```

Our runner is missing features. We'll need to perform some heart surgery. You know the drill by now: We know `gamepad_is_supported` exists from the documentation, so let's look for strings containing that.

![](https://i.imgur.com/4galbU9.png)

Well that was easy. I went ahead and renamed the function while you weren't looking. Take note of the offsets and spend a little bit of to investigate how the function callbacks work, and what are the `yyg_define_builtin` parameters. If you get too stuck you can consult [previous literature](https://github.com/nkrapivin/libLassebq), it pays off to spend some time here, using the previous cheatsheets only for cleanup to keep your headers a little more consistent with the community efforts. Keep in mind previous efforts might've glossed over details or downright commited mistakes, so it's always good to investigate it yourself, and remember to contribute your errata findings.

After you're done investigating the structures and parameters needed, and with the offsets in [our header](yyg_fix.h), we'll inject new functions at the builtin tables during `bcm_host_init` - way before YYG's runner has a chance, so we'll override any broken implementations or missing functions. That allows us to finally implement the standard [`bool`](https://docs2.yoyogames.com/source/_build/3_scripting/3_gml_overview/bool.html) function.

That gets us farther, but the first time you attempt saving your game, you'll notice the game crashes and then refuses to boot.

```
$ LD_LIBRARY_PATH=/usr/lib32:. LD_PRELOAD=libbcm_host.so ./runner
___________________________________________
############################################################################################
ERROR in
action number 1
of Create Event
for object oScreen:

int64 argument incorrect type
at gml_Script_configLoad
```

If you open that script on _UTMT_ you'll notice that it's all very sane, so it seems `int64` is broken. We'll also implement that one in `yyg_fix.c`. Since our injector happens early, it will override YYG's builtins.

![](https://i.imgur.com/yhECxqF.jpg)

Success! The game now boots and persists any changes you do to config files successfuly!

## Conclusions:

This is all an elbow grease kind of situation, with some effort, you can overcome the limitations of this beta runner by patching functions, hooking system calls and injecting new or reworked functionality into the interpreter. The skills learned in this research project can be applied to a number of other reverse engineering, modding and security projects.

As a byproduct, a number of GameMaker Titles now boot and are playable on this beta runner version.

I hope you enjoyed this read as much as I'm enjoying this project, and remember to support your developers.

## Greets:

This project wouldn't be possible, or would be way harder without the aid of these people, so, a round of applause for:

- The The Underminers Community for all the great work, and for being extremely helpful.
- YoYo Games, for [GameMaker](https://www.yoyogames.com/gamemaker).
- All the developers behind the games used for testing.
- [Ghidra](https://ghidra-sre.org/) for the awesome feature-rich, open-source reverse engineering stack.
- All the contributors and testers of [UndertaleModTool](https://github.com/krzys-h/UndertaleModTool).
- [jdonald](https://github.com/jdonald/) for the initial implementation of [gamemaker-rpi-wrapper](https://github.com/jdonald/gamemaker-rpi-wrapper/).
- Grossley for the `16Or17To15.csx` downgrader.
- [Kneesnap](https://github.com/Kneesnap/) for [inspiring the approach](https://github.com/Kneesnap/deltarune-circuit-sword) and some help.
- [nik](https://github.com/nkrapivin) for [general pointers and corrections, and better definitions for the yyg runner internals](https://github.com/nkrapivin/libLassebq).
- Surkow for helping proofread this article.
- Anyone that I might've neglected here. You know who you are.