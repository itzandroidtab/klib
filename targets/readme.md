## Targets

Folder for all the targets. Every target needs to have a `CMakeLists.txt` that can be included in the main `CMakeLists.txt` using `add_subdirectory()`. 

To create new target the following need to be added:
* `linkerscript.ld` linkerscript with at least one rom and one ram section
* `CMakeLists.txt` with at least the following defines:
    * `TARGET_CPU` the namespace of the target cpu (e.g. `atsam3x8e` for the atsam3x8e cpu)
    * `TARGET_LINKERSCRIPT` path to the linkerscript using the `PARENT_SCOPE`
