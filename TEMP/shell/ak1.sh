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
kernel_zip="TEMP/Pre-built_ZIP/ZIP"
cd $kernel_source
cp TEMP/Pre-built_ZIP/Template/Test_Kernel-AK1.zip TEMP/Pre-built_ZIP/ZIP/Test_Kernel.zip
cd $kernel_zip
unzip Test_Kernel.zip
cd $kernel_source
mv TEMP/modules/zImage TEMP/Pre-built_ZIP/ZIP/tmp/kernel/boot.img-zImage
mv TEMP/modules/dt.img TEMP/Pre-built_ZIP/ZIP/tmp/kernel/boot.img-dtb
cp TEMP/Pre-built_ZIP/Template/99minori TEMP/Pre-built_ZIP/ZIP/tmp/kernel/99minori
cd TEMP/Pre-built_ZIP/ZIP
rm Test_Kernel.zip
zip -r Test_Kernel *
rm -rfv META-INF
rm -rfv tmp
cd $HOME/kernel/TEMP/Pre-built_ZIP/ZIP
mv Test_Kernel.zip $HOME/kernel/TEMP/Pre-built_ZIP/Sign/Test_Kernel.zip
cd $HOME/kernel/TEMP/Pre-built_ZIP/Sign
java -jar signapk.jar signature-key.Nicklas@XDA.x509.pem signature-key.Nicklas@XDA.pk8 Test_Kernel.zip Test_Kernel-Flamingo-signed.zip
mv  Test_Kernel-Flamingo-signed.zip $HOME/kernel/Build/Yume_Kernel-Flamingo-signed.zip
rm Test_Kernel.zip
