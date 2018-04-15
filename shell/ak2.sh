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
kernel_zip="TEMP/Pre-built_ZIP/ZIP"

cp TEMP/Pre-built_ZIP/Template/Matsuura_Kernel-AK2.zip TEMP/Pre-built_ZIP/ZIP/Matsuura_Kernel.zip
cd $kernel_zip
unzip Matsuura_Kernel.zip
cd $kernel_source
mv TEMP/modules/zImage TEMP/Pre-built_ZIP/ZIP/zImage
mv TEMP/modules/dt.img TEMP/Pre-built_ZIP/ZIP/dtb
cp TEMP/Pre-built_ZIP/Template/99matsuura TEMP/Pre-built_ZIP/ZIP/tmp/kernel/99matsuura
cd TEMP/Pre-built_ZIP/ZIP
rm Matsuura_Kernel.zip
zip -r9 Matsuura_Kernel-Flamingo.zip * -x README Matsuura_Kernel-Flamingo.zip
rm -rfv META-INF
rm -rfv modules
rm -rfv patch
rm -rfv ramdisk
rm -rfv tools
rm -rfv anykernel.sh
rm -rfv README.md
rm -rvf zImage
rm -rvf dtb
mv Matsuura_Kernel-Flamingo.zip $HOME/Matsuura-Kernel-Flamingo/Build/Matsuura_Kernel-Flamingo.zip
