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

kernel_source="$HOME/kernel"
log="$HOME/kernel/TEMP/logs"
cd $kernel_source/TEMP/dtbtool
./dtbtool -s 2048 -o $kernel_source/arch/arm/boot/dt.img -p $kernel_source/scripts/dtc/ $kernel_source/arch/arm/boot/ -> $log/clean.log
cd $kernel_source
cp arch/arm/boot/dt.img TEMP/modules/dt.img
rm arch/arm/boot/dt.img
