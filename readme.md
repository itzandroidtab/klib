# C++ cmake toolchain for cortex hardware

Klib uses `cmake` and `arm-none-eabi-gcc` to build excecutables for cortex targets. Currently supported targets can be found at [target folder](./targets/).

Support for devices varies. Currently there is no specific target startup implementation added. This means the coprocessors are not enabled and the cpu clock may not be initialized to the full clock speed.

## Selecting target cpu
To select a target cpu the main `CMakeLists.txt` needs to be changed to include the corresponding target folder (e.g. for the max32660 the following subdirectory has to be added)
```
add_subdirectory(${CMAKE_SOURCE_DIR}/targets/max32660)
``` 

## Selecting board 
To make it easier to use the pins of a specific development board a target board can be selected. To include a board a define has to be added to the main `CMakeLists.txt`. (e.g. for the max32660 evsys the following define has to be added)
```
target_compile_definitions(target_cpu PUBLIC "TARGET_BOARD=evsys")
```
This needs to be added after the include of the target cpu.

## Using klib
Klib has most library functions documented using doxygen (there is no config file). Please refrence the doxygen/doxywizzard documentation to generate the documentation. 

### Interrupt handling
By design klib has a default vector table that should not be changed. Instead it has a method to move the vector table to ram. This allows more flexibility to change the vector table at runtime. To use interrupts, use the following file as reference [irq.hpp](./klib/irq.hpp)
