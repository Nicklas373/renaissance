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
kernel_source="$HOME/Matsuura-Kernel-Flamingo"

cd TEMP/dtbtool
./dtbtool -s 2048 -o $kernel_source/arch/arm/boot/dt.img -p $kernel_source/scripts/dtc/ $kernel_source/arch/arm/boot/
cd $kernel_source
cp arch/arm/boot/dt.img TEMP/modules/dt.img
rm arch/arm/boot/dt.img
