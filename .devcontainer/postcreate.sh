mkdir -p ../arm-none-eabi/
cd ../arm-none-eabi/

if [ ! -f ./arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi.tar.xz ]
then
    echo "Downloading arm-none-eabi-gcc"
    wget https://developer.arm.com/-/media/Files/downloads/gnu/12.3.rel1/binrel/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi.tar.xz
fi

echo "Unpacking arm-none-eabi-gcc"
tar xf arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi.tar.xz

sudo rm -f /usr/bin/arm-none-eabi-gcc || true
sudo rm -f /usr/bin/arm-none-eabi-g++ || true
sudo rm -f /usr/bin/arm-none-eabi-gdb || true
sudo rm -f /usr/bin/arm-none-eabi-size || true
sudo rm -f /usr/bin/arm-none-eabi-objcopy || true
sudo rm -f /usr/bin/arm-none-eabi-objdump || true

echo "Linking arm-none-eabi"
sudo ln -s `pwd`/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc 
sudo ln -s `pwd`/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-g++ /usr/bin/arm-none-eabi-g++
sudo ln -s `pwd`/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gdb /usr/bin/arm-none-eabi-gdb
sudo ln -s `pwd`/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-size /usr/bin/arm-none-eabi-size
sudo ln -s `pwd`/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-objcopy /usr/bin/arm-none-eabi-objcopy
sudo ln -s `pwd`/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-objdump /usr/bin/arm-none-eabi-objdump

echo "Getting arm headers"
git clone https://github.com/ARM-software/CMSIS_5 -b develop --depth=1 CMSIS
cp CMSIS/CMSIS/Core/Include/* klib/targets/arm/

echo "Generating headers from SVD file"
wget https://github.com/Open-CMSIS-Pack/devtools/releases/download/tools%2Fsvdconv%2F3.3.44/svdconv-3.3.44-linux64-amd64.tbz2 -O svdconv.tbz2
tar -xf svdconv.tbz2 
chmod +x svdconv

git clone https://github.com/itzandroidtab/klib-svd
for i in ./klib-svd/*.svd; do
    [ -f "$i" ] || break

    target=$(basename ${i%.*})
    ./svdconv $i --suppress-warnings --generate=header -o klib/targets/$target/ || true
    sed -i '/#include "system_/d' klib/targets/$target/$target.h
done
