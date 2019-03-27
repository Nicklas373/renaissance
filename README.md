# Yume Kernel || Private Build for Xperia E3 Kernel || Codename: Karin 

Yume Kernel is based on Lineage OS source (renaissance) with several optimization and followed by linux kernel upstream to 3.4.113 for Xperia E3 that use Lineage OS Nougat as base ROM. 

Yume Kernel offer kernel stability, smoothness process on cpu and This kernel is include several feature to increase performance, This kernel need kernel configuration app to control this kernel, so try to find kernel configuration app after flash this kernel.

This kernel already controlled by init.d as default, but kernel manager still need to downclock cpu clock speed at first boot. During stock binary mpdecision still enable, will check this later.

Recommended Kernel Configuration App:
- Kernel Adiutor 
- Device Control

This kernel feature is :
- Linux Kernel v3.4.113 (Upstream based on git.kernel.org)
- Added CPU overclock up to 1,6Ghz
- Added CPU underclock to 192Mhz (384Mhz as minimum idle freq)
- Added CPU GOV Backported from linux 3.10 (Only for chill and relaxed)
- Compatibility with AOSP,CM And LineageOS based ROM [Android 7.0+]
- Added custom CPU Governor 
: Chill,Relaxed & Conservative
- Added custom I/O Sched Governor 
: zen,sio,sioplus,tripndroid,fifo,fiops,bfq and vr
- Added custom TCP Modules
: Advanced,Bic,Westwood,Hybla,Vegas,Veno,Yeah,Ascarex and more
- Implement Power Efficient Workqueues
- Added Interface of Gentle Fair Sleepers,CPU Boost,Wakelock Toggles, Android Logger, Fsync and Arch Power (Option Disable by Default)
- NEON and Soft floating point optimizations for CPU and GPU
- Added Intelli Plug v3.8 
- Added Intelli Thermal v1.0
- Added Headset High Perfomance Mode (HPF)
- Added Adreno Idler v1.1
- Added Faux Sound Control v3.5
- Added Powersuspend Driver v1.5
- Added State Notifier Driver
- Added Wakelock bloker driver
- Compiled using Google Toolchains 4.9.4
- Added Compatibility with AnyKernel v1 Script
- Added EXFAT File System Support (Ported from https://github.com/dorimanx/exfat-nofuse)
- Added F2FS File System Support (Ported from https://git.kernel.org/pub/scm/linux/kernel/git/jaegeuk/f2fs-stable.git/)
- And other more, see github commit for other optimizations

#Private PROJECT

Build status :

- First Build Done 20190101 [ Uploaded ]
- Second Build Done 20190102 [ Uploaded ]
- Other Build Done 20190201 [ Uploaded ] 
- Third Build Done 20190203 [ Uploaded ] [This build already include chill & relaxed]
- Fourth Build Done 20190204 [ Uploaded ]
- Five Build Done 20190205 [ Uploaded ]
- Sixth Build Done 20190207 [ Uploaded ]
- Seventh Build Done 20190227 [ Uploaded ]
- Eighth Build Done 20190308 [ Uploaded ]
- Ninth Build Done 20190310 [ Uploaded ] // FINAL BUILD

Karin Build Status :

- First Build Done 20190327 [ Uploaded ]

Current Active branch :

Old Testing branch with intelli thermal [DEPRECATED]
- kotoha-intelli :
https://github.com/Nicklas373/renaissance/tree/kotoha-intelli

Old Stable branch with simplified thermal [DEPRECTED]
- Kaori :
https://github.com/Nicklas373/renaissance/tree/kaori

Stable branch with intelli thermal
- Karin :
https://github.com/Nicklas373/renaissance/tree/karin

Download link :

- Google Drive :
[Yume Build] https://drive.google.com/open?id=1kdSeZS2e5vzp7YTazy9EPV_RA26Ni-8h

[Karin Build] https://drive.google.com/drive/folders/15ShXF71SXOL1t0as3A7Ka1hzW-u8LGb_

Thanks to:
- @rmnhg [For kernel source]
- @infus38 [For some kernel patch]
- @vinay94185 [For Overclock]

# Codename: Kotoha || Project Yume (2018 - 2019)
