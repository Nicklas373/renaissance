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

cp TEMP/Pre-built_ZIP/Template/Matsuura_Kernel-AK1.zip TEMP/Pre-built_ZIP/ZIP/Matsuura_Kernel.zip
cd $kernel_zip
unzip Matsuura_Kernel.zip
cd $kernel_source
mv TEMP/modules/zImage TEMP/Pre-built_ZIP/ZIP/tmp/kernel/boot.img-zImage
mv TEMP/modules/dt.img TEMP/Pre-built_ZIP/ZIP/tmp/kernel/boot.img-dtb
cd TEMP/Pre-built_ZIP/ZIP
rm Matsuura_Kernel.zip
zip -r Matsuura_Kernel *
rm -rfv META-INF
rm -rfv tmp
cd $HOME/Matsuura-Kernel-Flamingo/TEMP/Pre-built_ZIP/ZIP
mv Matsuura_Kernel.zip $HOME/Matsuura-Kernel-Flamingo/TEMP/Pre-built_ZIP/Sign/Matsuura_Kernel.zip
cd $HOME/Matsuura-Kernel-Flamingo/TEMP/Pre-built_ZIP/Sign
java -jar signapk.jar signature-key.Nicklas@XDA.x509.pem signature-key.Nicklas@XDA.pk8 Matsuura_Kernel.zip Matsuura_Kernel-Flamingo-signed.zip
mv  Matsuura_Kernel-Flamingo-signed.zip $HOME/Matsuura-Kernel-Flamingo/Build/Matsuura_Kernel-Flamingo-signed.zip
rm Matsuura_Kernel.zip
