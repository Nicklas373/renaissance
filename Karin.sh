#!/bin/bash
#
# Copyright 2019, Dicky Herlambang "Nicklas373" <herlambangdicky5@gmail.com>
#
# Karin Kernel Builder Script
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

#Kernel Logic Memory
CROSS_COMPILE_GCC_4="$HOME/arm-linux-androideabi-4.9/bin"
# CROSS_COMPILE_GCC_5="$HOME/arm-linux-androideabi-5.x/bin"
kernel_dir="$HOME/kernel"
kernel_zImage="$kernel_dir/arch/arm/boot"
kernel_zip="TEMP/Pre-built_ZIP/ZIP"
modules="$kernel_dir/TEMP/modules"
zImage="$kernel_dir/TEMP/modules/zImage"
log="$kernel_dir/TEMP/logs"

#Cleaning kernel
clean(){
cd $kernel_dir
make clean -> $log/clean.log && make mrproper > $log/clean_mrproper.log
end=$(date +%s)
seconds=$(echo "$end - $start" | bc)
awk -v t=$seconds 'BEGIN{t=int(t*1000); printf "##################################\n# Kernel Compiling Time: %d:%02d:%02d#\n##################################\n", t/3600000, t/60000%60, t/1000%60}'
awk -v t=$seconds 'BEGIN{t=int(t*1000); printf "Kernel Compiling Time: %d:%02d:%02d\n", t/3600000, t/60000%60, t/1000%60}' -> $log/time.log
}

#Compiling kernel
compile(){
export ARCH=arm
export CROSS_COMPILE=$CROSS_COMPILE_GCC_4/arm-linux-androideabi-
start=$(date +%s)
make ARCH=arm karin_8926ss_ap_defconfig
make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE_GCC_4/arm-linux-androideabi- -j$(nproc --all) -> $log/karin.log
cd $kernel_dir
cp $kernel_zImage/zImage $modules
}

#Anykernel builder
anykernel(){
cd $kernel_dir
cp TEMP/Pre-built_ZIP/Template/Test_Kernel-AK1.zip TEMP/Pre-built_ZIP/ZIP/Test_Kernel.zip
cd $kernel_zip
unzip Test_Kernel.zip
cd $kernel_dir
mv TEMP/modules/zImage TEMP/Pre-built_ZIP/ZIP/tmp/kernel/boot.img-zImage
mv TEMP/modules/dt.img TEMP/Pre-built_ZIP/ZIP/tmp/kernel/boot.img-dtb
cp TEMP/Pre-built_ZIP/Template/99karin TEMP/Pre-built_ZIP/ZIP/tmp/kernel/99karin
cd TEMP/Pre-built_ZIP/ZIP
rm Test_Kernel.zip
zip -r Test_Kernel *
rm -rfv META-INF
rm -rfv tmp
cd $kernel_dir/TEMP/Pre-built_ZIP/ZIP
mv Test_Kernel.zip $kernel_dir/TEMP/Pre-built_ZIP/Sign/Test_Kernel.zip
cd $kernel_dir/TEMP/Pre-built_ZIP/Sign
java -jar signapk.jar signature-key.Nicklas@XDA.x509.pem signature-key.Nicklas@XDA.pk8 Test_Kernel.zip Test_Kernel-Flamingo-signed.zip
mv  Test_Kernel-Flamingo-signed.zip $kernel_dir/Build/Karin_Kernel-Flamingo-signed-$(date +"%Y-%m-%d").zip
rm Test_Kernel.zip
}

#DTB Builder
dtb(){
cd $kernel_dir/TEMP/dtbtool
./dtbtool -s 2048 -o $kernel_dir/arch/arm/boot/dt.img -p $kernel_dir/scripts/dtc/ $kernel_dir/arch/arm/boot/ -> $log/dtb.log
cd $kernel_dir
cp arch/arm/boot/dt.img TEMP/modules/dt.img
rm arch/arm/boot/dt.img
}

#Notification Completed
kernel_completed(){
message=${1:-"虹色letters"}
notify-send -t 3000 -i $HOME/Media/cover-1.png "THE IDOLM@STER MILLION THE@TER GENERATION 06 Cleasky" "$message"
ffplay $HOME/Media/letters.wav
}

#Notification Failed
kernel_failed(){
message=${1:-"Riko's Piano Sonata"}
notify-send -t 3000 -i $HOME/Media/cover-2.jpg "想いよひとつになれ (ピアノバージョン)" "$message"
ffplay $HOME/Media/hitotsu.flac
}

#Kernel Checking
checking(){
echo "Checking kernel..."
if [ -f "$zImage" ]
then
	echo "Kernel found"
	echo "Continue to build kernel"
	dtb
	anykernel
	clean
	kernel_completed
else
	echo "Kernel not found"
	echo "Cancel kernel to build"
	gedit $log/karin.log
	cd $kernel_source
	kernel_failed
	end=$(date +%s)
	seconds=$(echo "$end - $start" | bc)
	awk -v t=$seconds 'BEGIN{t=int(t*1000); printf "##################################\n# Kernel Compiling Time: %d:%02d:%02d#\n##################################\n", t/3600000, t/60000%60, t/1000%60}'
	awk -v t=$seconds 'BEGIN{t=int(t*1000); printf "Kernel Compiling Time: %d:%02d:%02d\n", t/3600000, t/60000%60, t/1000%60}' -> $log/time.log
fi
}

#Main Program
menu_compile(){
echo "
######################################################
#                                                    #
#                   Karin Kernel                     #
#                                                    #
#                Nicklas Van Dam @XDA                #
#                                                    #
#	   	Codename: Karin (2019)	             #
#						     #
######################################################"
echo ""
echo "Welcome To Karin Kernel Builder"
echo "##Running GCC Toolchains 4.9.4 (Google Toolchains)"
compile
checking
}

#Execute Program
menu_compile
