#!/bin/bash
#
# Copyright 2019, Dicky Herlambang "Nicklas373" <herlambangdicky5@gmail.com>
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

CROSS_COMPILE_GOOGLE="$HOME/arm-linux-androideabi-4.9/bin"
kernel_zImage="$HOME/kernel/arch/arm/boot"
kernel_source="$HOME/kernel"
kernel_zip="TEMP/Pre-built_ZIP/ZIP"
modules="$HOME/kernel/TEMP/modules"
zImage="$HOME/kernel/TEMP/modules/zImage"
log="$HOME/kernel/TEMP/logs"
export ARCH=arm
export CROSS_COMPILE=$CROSS_COMPILE_GOOGLE/arm-linux-androideabi-
start=$(date +%s)
make ARCH=arm yume_flamingo_defconfig
make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE_GOOGLE/arm-linux-androideabi- -j$(nproc --all) -> $log/yume.log
end=$(date +%s)
seconds=$(echo "$end - $start" | bc)
awk -v t=$seconds 'BEGIN{t=int(t*1000); printf "Kernel Compiling Time: %d:%02d:%02d\n", t/3600000, t/60000%60, t/1000%60}' -> $log/time.log
cd $kernel_source
cp $kernel_zImage/zImage $modules
