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

#Kernel Logic Memory
CROSS_COMPILE_GOOGLE="$HOME/arm-linux-androideabi-4.9/bin"
CROSS_COMPILE_GCC_5="$HOME/arm-linux-androideabi-5.x/bin"
kernel_zImage="$HOME/kernel/arch/arm/boot"
kernel_source="$HOME/kernel"
kernel_zip="TEMP/Pre-built_ZIP/ZIP"
modules="$HOME/kernel/TEMP/modules"
zImage="$HOME/kernel/TEMP/modules/zImage"
log="$HOME/kernel/TEMP/logs"

#Cleaning kernel
clean(){
cd $kernel_source
make clean && make mrproper
end=$(date +%s)
seconds=$(echo "$end - $start" | bc)
awk -v t=$seconds 'BEGIN{t=int(t*1000); printf "##################################\n# Kernel Compiling Time: %d:%02d:%02d#\n##################################\n", t/3600000, t/60000%60, t/1000%60}'
awk -v t=$seconds 'BEGIN{t=int(t*1000); printf "Kernel Compiling Time: %d:%02d:%02d\n", t/3600000, t/60000%60, t/1000%60}' -> $log/time.log
}

#Compiling kernel
compile(){
export ARCH=arm
export CROSS_COMPILE=$CROSS_COMPILE_GCC_5/arm-linux-androideabi-
start=$(date +%s)
make ARCH=arm yume_flamingo_defconfig
make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE_GCC_5/arm-linux-androideabi- -j$(nproc --all) -> $log/yume.log
cd $kernel_source
cp $kernel_zImage/zImage $modules
}

#Anykernel builder
anykernel(){
cd $kernel_source
cp TEMP/Pre-built_ZIP/Template/Test_Kernel-AK1.zip TEMP/Pre-built_ZIP/ZIP/Test_Kernel.zip
cd $kernel_zip
unzip Test_Kernel.zip
cd $kernel_source
mv TEMP/modules/zImage TEMP/Pre-built_ZIP/ZIP/tmp/kernel/boot.img-zImage
mv TEMP/modules/dt.img TEMP/Pre-built_ZIP/ZIP/tmp/kernel/boot.img-dtb
cp TEMP/Pre-built_ZIP/Template/99yume TEMP/Pre-built_ZIP/ZIP/tmp/kernel/99yume
cd TEMP/Pre-built_ZIP/ZIP
rm Test_Kernel.zip
zip -r Test_Kernel *
rm -rfv META-INF
rm -rfv tmp
cd $HOME/kernel/TEMP/Pre-built_ZIP/ZIP
mv Test_Kernel.zip $HOME/kernel/TEMP/Pre-built_ZIP/Sign/Test_Kernel.zip
cd $HOME/kernel/TEMP/Pre-built_ZIP/Sign
java -jar signapk.jar signature-key.Nicklas@XDA.x509.pem signature-key.Nicklas@XDA.pk8 Test_Kernel.zip Test_Kernel-Flamingo-signed.zip
mv  Test_Kernel-Flamingo-signed.zip $HOME/kernel/Build/Yume_Kernel-Flamingo-signed-$(date +"%Y-%m-%d")-INTELLI.zip
rm Test_Kernel.zip
}

#DTB Builder
dtb(){
cd $kernel_source/TEMP/dtbtool
./dtbtool -s 2048 -o $kernel_source/arch/arm/boot/dt.img -p $kernel_source/scripts/dtc/ $kernel_source/arch/arm/boot/ -> $log/dtb.log
cd $kernel_source
cp arch/arm/boot/dt.img TEMP/modules/dt.img
rm arch/arm/boot/dt.img
}

#Notification Completed
kernel_completed(){
message=${1:-"虹色letters"}
notify-send -t 3000 -i $HOME/kernel/TEMP/Additional/cover-1.jpg "THE IDOLM@STER LIVE THE@TER SOLO COLLECTION 06 Angel Stars" "$message"
ffplay $HOME/kernel/TEMP/Additional/letters.flac
}

#Notification Failed
kernel_failed(){
message=${1:-"Riko's Piano Sonata"}
notify-send -t 3000 -i $HOME/kernel/TEMP/Additional/cover-2.jpg "想いよひとつになれ (ピアノバージョン)" "$message"
ffplay $HOME/kernel/TEMP/Additional/hitotsu.flac
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
	gedit $log/yume.log
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
#                    Yume Kernel                     #
#                                                    #
#                Nicklas Van Dam @XDA                #
#                                                    #
#	   	Codename: Kotoha (2019)	             #
#						     #
######################################################"
echo ""
echo "Welcome To Yume Kernel Builder"
echo "##Running GCC Toolchains 5.4.X (Hyper Toolchains)"
compile
checking
}

#Execute Program
menu_compile
