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

CROSS_COMPILE_5="$HOME/arm-linux-androideabi-5.x/bin"
kernel_zImage="$HOME/Matsuura-Kernel-Flamingo/arch/arm/boot"
kernel_source="$HOME/Matsuura-Kernel-Flamingo"
kernel_zip="TEMP/Pre-built_ZIP/ZIP"
modules="$HOME/Matsuura-Kernel-Flamingo/TEMP/modules"
zImage="$HOME/Matsuura-Kernel-Flamingo/TEMP/modules/zImage"

export ARCH=arm
export CROSS_COMPILE=$CROSS_COMPILE_5/arm-linux-androideabi-
make ARCH=arm matsuura_flamingo_defconfig
make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE_5/arm-linux-androideabi- -j4 -> matsuura.log
cd $kernel_source
cp $kernel_zImage/zImage $modules

