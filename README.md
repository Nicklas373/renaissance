# Yume Kernel || Private Build for Xperia E3 Kernel

Yume Kernel is based on Lineage OS source (renaissance) with several optimization and followed by linux kernel upstream to 3.4.113 for Xperia E3 that use Lineage OS Nougat as base ROM. 

Yume Kernel offer kernel stability, smoothness process on cpu and This kernel is include several feature to increase performance, This kernel need kernel configuration app to control this kernel, so try to find kernel configuration app after flash this kernel.

Recommended Kernel Configuration App:
- Kernel Adiutor 
- Device Control

This kernel feature is :
- Linux Kernel v3.4.113 (Upstream based on git.kernel.org)
- Added CPU overclock up to 1,6Ghz
- Added CPU underclock to 96Mhz (300Mhz as minimum idle freq)
- Added GPU overclock and underclock to 100Mhz as lower and 550Mhz as higher frequency
- Added CPU GOV Backported from linux 3.10 (Only for chill and relaxed)
NOTE: BUILD THAT INCLUDED THIS FEATURE ISN'T RECOMMENDED TO USE FOR LONG TERM USAGE
- Compatibility with AOSP,CM And LineageOS based ROM [Android 7.0+]
- Added custom CPU Governor 
: Alucard,Chill,Relaxed,Conservative & Lionheart
- Added custom I/O Sched Governor 
: zen,sio,sioplus,tripndroid,fifo,fiops,maple,bfq and vr
- Added custom TCP Modules
: Advanced,Bic,Westwood,Hybla,Vegas,Veno,Yeah,Ascarex and more
- Implement Power Efficient Workqueues
- Added Interface of Gentle Fair Sleepers,CPU Boost,Wakelock Toggles, Android Logger, Fsync, Dynamic Fsync and Arch Power (Option Disable by Default)
- Added Frandom Support
- Entropy Tweaks
- Hard for CPU and Hard Floating Point for GPU (neon-vfpv4)
- Added Intelli Plug v4.0 
- Added Simplified Thermal by @fransiscofranco
- Added Headset High Perfomance Mode (HPF)
- Added Adreno Idler v1.1
- Added Faux Sound Control v3.6
- Added Powersuspend Driver v1.6 
- Added State Notifier Driver
- Compiled using GCC Google Toolchains 4.9.4
- Added Compatibility with AnyKernel v1 Script
- Added EXFAT File System Support (Ported from https://github.com/dorimanx/exfat-nofuse)
- Added F2FS File System Support (Ported from https://git.kernel.org/pub/scm/linux/kernel/git/jaegeuk/f2fs-stable.git/)
- Added Dynamic Management Page of Dirty Writeback
- And other more, see github commit for other optimizations

#Private PROJECT

Build status :

- First Build Done 20190101 [ Uploaded ]
- Second Build Done 20190102 [ Uploaded ]
- Other Build Done 20190201 [ Uploaded ] 
- Third Build Done 20190203 [ Uploaded] [This build already include chill & relaxed]

Download link :

- Google Drive :
https://drive.google.com/open?id=1kdSeZS2e5vzp7YTazy9EPV_RA26Ni-8h

Thanks to:
- @rmnhg [For kernel source]
- @infus38 [For some kernel patch]
- @vinay94185 [For Overclock]

# Codename: Kaori || Project Yume (2018 - 2019)

