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
CROSS_COMPILE_4="/home/Matsuura/arm-linux-androideabi-4.9/bin"
CROSS_COMPILE_5="/home/Matsuura/arm-linux-androideabi-5.x/bin"
kernel_zImage="arch/arm/boot"
kernel_source="/home/Matsuura/renaissance"
# kernel_orig_dir="TEMP/orig_boot_img"
# kernel_build="TEMP/AIK-Linux"
modules="TEMP/modules"
zImage="TEMP/modules/zImage"

#Logic Answer Memory
answer(){
A="1"
B="2"
C="Yes"
D="No"
}

#Kernel Modules GCC4
modules_gcc_4(){
# echo "##Creating Temporary Modules kernel"
# cd $kernel_source
# cp arch/arm/boot/zImage TEMP/modules/zImage
# find . -name "*.ko" -exec cp {} modules \;
# cd modules
# $CROSS_COMPILE_4/arm-linux-androideabi-strip --strip-unneeded *.ko
cd $kernel_source
}

#Kernel Modules GCC5
modules_gcc_5(){
# echo "##Creating Temporary Modules kernel"
# cd $kernel_source
# cp arch/arm/boot/zImage TEMP/modules/zImage
# find . -name "*.ko" -exec cp {} modules \;
# cd modules
# $CROSS_COMPILE_5/arm-linux-androideabi-strip --strip-unneeded *.ko
cd $kernel_source
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
echo "1. GCC 4.9.X"
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
		# modules_gcc_4
fi
if [ "$choice" == "$B" ];
	then
		echo "##Running GCC Toolchains 5.4 (Hyper Toolchains)"
		export ARCH=arm
		export CROSS_COMPILE=$CROSS_COMPILE_5/arm-linux-androideabi-
		echo "##Building Matsuura Kernel"
		make ARCH=arm matsuura_flamingo_defconfig
		make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE_5/arm-linux-androideabi- -j4 -> matsuura.log
		# modules_gcc_5
	else
		invalid
fi
}

#Execute Program
menu_compile
