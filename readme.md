# C++ cortex-m hardware library

![klib library](https://github.com/itzandroidtab/klib/actions/workflows/cmake.yml/badge.svg)

Klib uses `cmake` and `arm-none-eabi-gcc` to build excecutables for cortex targets. Currently supported targets can be found in the [target folder](./targets/).

Support for devices varies. Currently some targets have a startup file that can be enabled by uncommenting line 6 in the project CMakeLists ([project/CMakeLists.txt](./project/CMakeLists.txt#L6)). This means by default the coprocessors are not enabled and the cpu clock may not be initialized to the full clock speed.

---

## Starting your first project

:warning: This project needs the header file generated from the `.svd` of the manufacturer. Header file should be placed in in the targets/target folder with the same name as the target folder and should be generated by the CMSIS `svdconv` utility. (the `.svd` files for klib can be found [here](https://github.com/itzandroidtab/klib-svd))

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

Minimal main file:
```cpp
#include <klib/klib.hpp>

int main() {
    return 0;
}
```
When a specific board is not found within a target cpu this include will give a error the file `boards/TARGET_BOARD/pins.hpp` cannot be found.


---

## Using klib
Klib has most library functions documented using doxygen (there is no config file). Please refrence the doxygen/doxywizzard documentation to generate the documentation. 

### Interrupt handling
By design klib has a default vector table that should not be changed. Instead one of the 3 vector table implementations should be used. 

Klib has the following implementations:
* Ram based vector table (used by default)
* Ram based vector table with entry and exit hooks (uses more ram and flash than the default implementation)
* Flash/custom vector table (allows a vector table stored in flash or for a custom ram based implementation)

When chosing one of the ram based vector table implementations all the code works by default as this allows the code to change the interrupt at runtime. When chosing the flash/custom vector table the user needs to create the vector table and pass it to the flash based vector table implementation. With this implementation the interrupts are not configured automaticly when needed and need to be changed by the user to the correct callback/handler. For more information about the different implementations see [irq.hpp](./klib/irq.hpp) as a reference.
