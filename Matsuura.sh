#!/bin/bash
#
# Copyright 2018, Dicky Herlambang "Nicklas373" <herlambangdicky5@gmail.com>
#
# Matsuura Kernel Builder Script // Mimori Kernel Side Development Project
#
# This software is licensed under the terms of the GNU General Public
# License version 2, as published by the Free Software Foundation, and
# may be copied, distributed, and modified under those terms.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

#Logic Memory
CROSS_COMPILE_4="$HOME/arm-linux-androideabi-4.9/bin"
CROSS_COMPILE_5="$HOME/arm-linux-androideabi-5.x/bin"
kernel_zImage="$HOME/Matsuura-Kernel-Flamingo/arch/arm/boot"
kernel_source="$HOME/Matsuura-Kernel-Flamingo"
kernel_orig_dir="$HOME/Matsuura-Kernel-Flamingo/TEMP/orig_boot_img"
kernel_build="$HOME/Matsuura-Kernel-Flamingo/TEMP/AIK-Linux"
kernel_zip="TEMP/Pre-built_ZIP/ZIP"
modules="$HOME/Matsuura-Kernel-Flamingo/TEMP/modules"
zImage="$HOME/Matsuura-Kernel-Flamingo/TEMP/modules/zImage"

#Logic Answer Memory
answer(){
A="1"
B="2"
C="Yes"
D="No"
}

#Kernel Modules GCC4
modules_gcc_4(){
echo "##Creating Temporary Modules kernel"
cd $kernel_source
cp $kernel_zImage/zImage $modules
}

#Kernel Modules GCC5
modules_gcc_5(){
echo "##Creating Temporary Modules kernel"
cd $kernel_source
cp $kernel_zImage/zImage $modules
}

#Notification Completed
kernel_completed(){
message=${1:-"Riko's Piano Sonata"}
notify-send -t 10000 -i $HOME/Matsuura-Kernel-Nicki/TEMP/Additional/3.jpg "想いよひとつになれ (ピアノバージョン)" "$message"
ffplay $HOME/Matsuura-Kernel-Nicki/TEMP/Additional/3.flac
echo "Cleaning up"
cd $kernel_source
make clean && make mrproper
exit
}

#Notification Failed
kernel_failed(){
message=${1:-"AZALEA"}
notify-send -t 10000 -i $HOME/Matsuura-Kernel-Nicki/TEMP/Additional/2.jpg "Tokimeki Bunruigaku" "$message"
ffplay $HOME/Matsuura-Kernel-Nicki/TEMP/Additional/2.flac
echo "Cleaning up"
cd $kernel_source
make clean && make mrproper
echo "Try to fix error"
exit
}

#DTB Tool Builder
dtb(){
echo "## Building new dt.img"
cd TEMP/dtbtool
./dtbtool -s 2048 -o $kernel_source/arch/arm/boot/dt.img -p $kernel_source/scripts/dtc/ $kernel_source/arch/arm/boot/
echo "## Copy new dt.img"
cd $kernel_source
cp arch/arm/boot/dt.img TEMP/modules/dt.img
rm arch/arm/boot/dt.img
echo "## dt.img created"
}

#Kernel Build New Method
build(){
echo "## Building anykernel file"
cp TEMP/Pre-built_ZIP/Template/Matsuura_Kernel.zip TEMP/Pre-built_ZIP/ZIP/Matsuura_Kernel.zip
cd $kernel_zip
unzip Matsuura_Kernel.zip
cd $kernel_source
mv TEMP/modules/zImage TEMP/Pre-built_ZIP/ZIP/zImage
mv TEMP/modules/dt.img TEMP/Pre-built_ZIP/ZIP/zImage-dtb
cd TEMP/Pre-built_ZIP/ZIP
rm Matsuura_Kernel.zip
zip -r Matsuura_Kernel *
rm -rfv META-INF
rm -rfv modules
rm -rfv patch
rm -rfv ramdisk
rm -rfv tools
rm -rfv anykernel.sh
rm -rfv README.md
rm -rvf zImage
rm -rvf zImage-dtb
mv Matsuura_Kernel.zip $HOME/Matsuura-Kernel-Flamingo/Build/Matsuura_Kernel-Flamingo.zip
echo "Matsuura Kernel Completed to build"
echo "Thanks to XDA - Developers"
echo "プロジェクト　ラブライブ | Project MIMORI (2018)"
echo "ありがとう　ございます μ's !!!"
}

#Kernel Checking
checking(){
echo "Checking kernel..."
if [ -f "$zImage" ]
then
	echo "Kernel found"
	echo "Continue to build kernel"
	dtb
	build
	kernel_completed
else
	echo "Kernel not found"
	echo "Cancel kernel to build"
	gedit matsuura.log
	cd $kernel_source
	kernel_failed
fi
}

#Invalid Option
invalid(){
echo "Your Option Is Invalid"
echo "Return to main menu ?"
echo "1. Yes"
echo "2. No"
echo "(Yes / No)"
read option
answer
if [ "$option" == "$C" ];
	then
		menu_compile
fi
if [ "$option" == "$D" ];
	then
		echo "See You Later"
		exit
fi
}

#Main Program
menu_compile(){
echo "
######################################################
#                                                    #
#                   Matsuura Kernel                  #
#                                                    #
#                Nicklas Van Dam @XDA                #
#                                                    #
#	   Side DEVELOPMENT OF Mimori Kernel	     #
#						     #
######################################################"
echo "Welcome To Matsuura Kernel Builder"
echo "Select Which GCC To Use ?"
echo "1. GCC 4.9.X (for old build only)"
echo "2. GCC 5.4.X"
echo "( 1 / 2)"
read choice
answer
if [ "$choice" == "$A" ];
	then
		echo "##Running GCC Toolchains 4.9 (Hyper Toolchains)"
		export ARCH=arm
		export CROSS_COMPILE=$CROSS_COMPILE_4/arm-linux-androideabi-
		echo "##Building Matsuura Kernel"
		make ARCH=arm matsuura_flamingo_defconfig
		make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE_4/arm-linux-androideabi- -j4 -> matsuura.log
		modules_gcc_4
		checking
fi
if [ "$choice" == "$B" ];
	then
		echo "##Running GCC Toolchains 5.4 (Hyper Toolchains)"
		export ARCH=arm
		export CROSS_COMPILE=$CROSS_COMPILE_5/arm-linux-androideabi-
		echo "##Building Matsuura Kernel"
		make ARCH=arm matsuura_flamingo_defconfig
		make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE_5/arm-linux-androideabi- -j4 -> matsuura.log
		modules_gcc_5
		checking
	else
		invalid
fi
}

#Execute Program
menu_compile
