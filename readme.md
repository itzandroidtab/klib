# C++ cmake toolchain for cortex hardware

![minimal build](https://github.com/itzandroidtab/klib/actions/workflows/cmake.yml/badge.svg)

Klib uses `cmake` and `arm-none-eabi-gcc` to build excecutables for cortex targets. Currently supported targets can be found at [target folder](./targets/).

Support for devices varies. Currently there is no specific target startup implementation added. This means the coprocessors are not enabled and the cpu clock may not be initialized to the full clock speed.

---

## Starting your first project

### Selecting target cpu
To select a target cpu the target cpu needs to be added to the commandline when configuring cmake. 

(e.g. To configure cmake for the max32660, run the following command)
```sh
cmake -B ./build -DTARGET_CPU=max32660
```
This configures the project for the specific target cpu. To change to a different target, the project has to be reconfigured.

### Selecting board 
To make it easier to use the pins of a specific development board a target board can be selected. To include a board another option needs to be added to the commandline. 

(e.g. To configure cmake for the max32660 evsys board, run the following command)
```sh
cmake -B ./build -DTARGET_CPU=max32660 -DTARGET_BOARD=evsys
```

#### Setup VSCode
(When using vscode with the cmake plugin the following can be added to the `settings.json` to configure cmake for the max32660 evsys board)
```json
{
    "cmake.configureArgs": [
        "-DTARGET_CPU=max32660",
        "-DTARGET_BOARD=evsys"
    ],
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
}
```

### Main file
By default klib has no `main.cpp` file. This has to be created by the user. To use the klib target system `klib.hpp` needs to be included. 

When a specific board is not found within a target cpu this include will give a error the file `boards/TARGET_BOARD/pins.hpp` cannot be found.

Minimal main file:
```cpp
#include <klib.hpp>

int main() {
    return 0;
}
```

---

## Using klib
Klib has most library functions documented using doxygen (there is no config file). Please refrence the doxygen/doxywizzard documentation to generate the documentation. 

### Interrupt handling
By design klib has a default vector table that should not be changed. Instead it has a method to move the vector table to ram. This allows more flexibility to change the vector table at runtime. To use interrupts, use the following file as reference [irq.hpp](./klib/irq.hpp)
