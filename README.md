# Matsuura Kernel || Private Build for Xperia E3 Kernel

Matsuura Kernel is based on Lineage OS source (renaissance), and included many improvement, features and optimization on processor for Xperia E3 that use Lineage OS Nougat as base ROM. 

Matsuura Kernel offer kernel stability, smoothness process on cpu and This kernel is include several feature to increase performance, This kernel need kernel configuration app to control this kernel, so try to find kernel configuration app after flash this kernel.

Recommended Kernel Configuration App:
- Kernel Adiutor 
- Device Control

This kernel feature is :
- Linux Kernel v3.4.113
- Added CPU overclock up to 1,6Ghz
- Added CPU underclock to 96Mhz (300Mhz as minimum idle freq)
- Added GPU overclock and underclock to 100Mhz as lower and 550Mhz as higher frequency
- Compatibility with AOSP,CM And LineageOS based ROM [Android 7.0+]
- Upstreamed CPU Governor from 3.4.y -> 3.10.y
- Added custom CPU Governor 
: Alucard,Intelliactive,IntelliMM,PegasusQ,Smartmax,Lionheart,Relaxed, Chill & Intellidemand
- Added custom I/O Sched Governor 
: zen,sio,sioplus,tripndroid,fifo,fiops,maple,bfq and vr
- Added custom TCP Modules
: Advanced,Bic,Westwood,Hybla,Vegas,Veno,Yeah,Ascarex and more
- Implement Power Efficient Workqueues
- Added Interface of Gentle Fair Sleepers,CPU Boost,Android Logger,Wakelock Toggles and Arch Power (Option Disable by Default)
- Added Frandom Support
- Added Dynamic Fsync 2.0
- Entropy Tweaks
- NEON VFPV4 Optimized (hard)
- Added Intelli Plug v5.4
- Added Intelli Thermal v1.0
- Added Headset High Perfomance Mode (HPF)
- Added Simple GPU Algorithm
- Added Faux Sound Control v3.6
- Added Powersuspend Driver v1.5
- Added State Notifier Driver
- Compiled using GCC 5.4.1 Hyper Toolchains
- Added Compatibility with AnyKernel v1 Script
- Added EXFAT File System Support (Need Test)

#BETA PROJECT

Build status :

- First  Build        Done         20180217      [  Uploaded  ] / [XDA-VERSION]
- Second Build        Done         20180225      [  Uploaded  ]
- Third  Build (F2FS) Done	   20180304	 [  Uploaded  ] / [XDA-VERSION]
- Third  Build 	      Done 	   20180307      [Not Uploaded]
- Third  Build Rev-1  Done	   20180319	 [  Uploaded  ]

Thanks to: 
- @rmnhg      [For kernel source] 
- @infus38    [For some kernel patch] 
- @vinay94185 [For Overclock]
