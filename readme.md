# C++ cortex-m hardware library

![klib library](https://github.com/itzandroidtab/klib/actions/workflows/build.yml/badge.svg)

Klib uses `cmake` and `arm-none-eabi-gcc` to build excecutables for cortex targets. Currently supported targets can be found in the [target folder](./targets/).

Support for devices varies. Currently some targets have a startup file that can be enabled by uncommenting line 6 in the project CMakeLists ([project/CMakeLists.txt](./project/CMakeLists.txt#L6)). This means by default the coprocessors are not enabled and the cpu clock may not be initialized to the full clock speed.

---

## Starting your first project

The easiest way to start a your first project is using Github codespaces. Github codespaces will download all the required programs and will generate the required [header files](https://github.com/itzandroidtab/klib-svd) for all supported microcontrollers. If you want to create a project on your own machine the header files need to be generated manually (this can be done using the CMSIS 'svdconv' utility). The header files should be placed in the targets/chip/target folder with the same name as the target folder. Another way to get the header files is to start a github codespace and download the required headers when it is done converting all the svd files to headers.

For example the svd files for the LPC1756 can be generated using:
```bash
svdconv.exe lpc1756.svd --generate=header
```
The output of the svdconv utility needs to be moved to: `targets/chip/lpc1756/lpc1756.h`

### Selecting target cpu
To select a target cpu the target cpu needs to be added to the commandline when configuring cmake. 

(e.g. To configure cmake for the max32660, run the following command)
```sh
cmake -B ./build -DTARGET_CPU=max32660
```
This configures the project for the specific target cpu. To change to a different target, the project has to be reconfigured.

#### Setup VSCode
(When using vscode with the cmake plugin the following can be added to the `settings.json` to configure cmake for the max32660 evsys board)
```json
{
    "target_cpu": "max32660",
    "cmake.configureArgs": [
        "-DTARGET_CPU=max32660"
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

### Constructors before running main
C++ supports global constructors for objects. These objects will be constructed before main is executed. This is supported by klib. If you want to initialize hardware before these global constructors are called you need to add the attribute `__attribute__((__constructor__(x)))` to your function definition where x is a number above 101 (the first 100 are reserved by gcc and 101 is reserved for startup code by klib). This will make sure your function will be executed before any constructor is called.

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

## Tests
Klib has tests for the microcontroller indepented code. The resuls of the tests can be found [here](https://github.com/itzandroidtab/klib-x86/actions)