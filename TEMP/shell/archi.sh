#!/bin/bash
#
# Copyright 2018, Dicky Herlambang "Nicklas373" <herlambangdicky5@gmail.com>
#
# Yume Kernel Builder Script
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

CROSS_COMPILE_ARCHI="$HOME/architoolchain/bin"
kernel_zImage="$HOME/kernel/arch/arm/boot"
kernel_source="$HOME/kernel"
kernel_zip="TEMP/Pre-built_ZIP/ZIP"
modules="$HOME/kernel/TEMP/modules"
zImage="$HOME/kernel/TEMP/modules/zImage"
export ARCH=arm
export CROSS_COMPILE=$CROSS_COMPILE_ARCHI/arm-architoolchain-linux-gnueabihf-
make ARCH=arm yume_flamingo_defconfig
make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE_ARCHI/arm-architoolchain-linux-gnueabihf- -j$(nproc --all) -> minori.log
cd $kernel_source
cp $kernel_zImage/zImage $modules
