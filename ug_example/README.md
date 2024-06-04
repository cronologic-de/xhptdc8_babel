# xHPTDC8 User Guide Example

This directory contains a project to compile the C++ example shows in the user guide of the cronologic xHPTDC8 time-to-digital converter. The project is a `Console` App.

You can download the user guide from the [product web page](https://www.cronologic.de/products/tdcs/xhptdc8-pcie).


## Directory Structure
```
    .
    ├── src/        # source code: .cpp file (or alternate .c file)
    ├── tools/      # files and scripts of the tools, e.g. building tools (CMake, etc...)
    └── build/      # (default) compiled output for any OS, configuration, platform
```

---

## Build the C++ Project
> **Note**<br>
> In most cases we recommend using the C++ version of the example `xhptdc8_user_guide_example.cpp`.<br>
> If, however, you require a pure C version, instructions on how to compile `xhptdc8_user_guide_example.c` are provided below.

The project is build using `CMake`.

If you don't have `CMake` installed, refer to [Installing CMake](https://cmake.org/install/) for instructions. You can check if `CMake` is installed by
running `cmake --version`.

On **Windows**, you can build the project using either `CMake` directly, or using Visual Studio 2019 (or later), see [Build Using `CMake`](#build-using-cmake) or [Build Using Visual Studio](#build-using-visual-studio), respectively.

On **Linux**, you can build the project using `CMake`, see [Build Using `CMake`](#build-using-cmake).

It is possible to run the example project without actual access to the xHPTDC8 hardware by utilizing the `dummy library`. In that case, copy the DLL corresponding to your particular platform from the `.lib/<platform>` directory to the _driver directory_. E.g., for the platform _x64 Release_ `.exe`, copy `xhptdc8_driver_64.dll` from [`.lib/x64dummy`](../lib/x64dummy/) to the _driver directory_.

---

### Build Using Visual Studio
It is mainly done using `CMakeSettings.json` file provided in [`tools` folder](/tools/CMakeSettings.json) package, that uses the projects [`CMakeLists.txt`](/tools/CMakeLists.tx).

**Prerequisites**
- Visual Studio 2019 or later
- [C++ CMake tools for Windows](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio#installation)

  You can install these from the _Visual Studio Installer_ under 
  _Desktop development with C++_.

**Open and configure the project**
1. Open Visual Studio.
2. Select `Open a local folder`.
3. Select the project folder, e.g., `ndigo5g_babel\ug_example`.
4. If a "CMake Integration" message _to enable Visual Studio's CMake support with this workspace_ is displayed
   1. Select `Enable and set source directory` button.
   2. In "Select CMakeLists.txt" Dialog, navigate to `<path\to\project\folder>\tools`, and open `CMakeLists.txt` file, e.g. `ug_example\tools\CMakeLists.txt`

**Compile and Link**

Select `Build -> Build All` from the menu bar (or any standard Visual Studio way to build the project).

**Project Environments and Configurations**

The target executable name is `xhptdc8_ugex.exe`.

| Configuration     | `CMakeSettings` | `Build root`                     | `CMake generator`     | Output Folder          |
| ----------------- | --------------- | -------------------------------- | --------------------- | ---------------------  |
| **x86_64 Debug**  | x64-Debug       | `${projectDir}\..\build\bfvsD`   | Visual Studio 17 2022 Win64 | `lib\x64\Release`   |
| **x86_64 Release**| x64-Release     | `${projectDir}\..\build\bfvsR`   | Visual Studio 17 2022 Win64 | `lib\x64\Debug`   |

> **Note**<br>
The provided file builds the project using Visual Studio 2022, however, you can change `generator` in [`CMakeSettings.json`](./tools/CMakeSettings.json) to any other Visual Studio generator you have on your machine.

---

### Build Using `CMake`

In a terminal, navigate to `./ug_example/tools` and run the following command

| Platform          | Configuration | Configure CMake command                                                         | Compile & Link Command                            | Output Folder          |
| ----------------- | ------------- | -------------------------------------------------     | ------------------------------------------------- | ---------------------  |
| **Windows x86_64**| Release       | `cmake -B ..\build\bfR -A x64`                                                  | `cmake --build ..\build\bfR --config Release`     | `lib\x64\Release`   |
| **Windows x86_64**| Debug         | `cmake -B ..\build\bfD -A x64`                                                  | `cmake --build ..\build\bfD --config Debug`       | `lib\x64\Debug`     |
| **Linux x86_64**  | Release       | `cmake -B ../build/bfR -DCMAKE_BUILD_TYPE=Release`                              | `cmake --build ../build/bfR`                      | `lib/x64`   |
| **Linux x86_64**  | Debug         | `cmake -B ../build/bfD -DCMAKE_BUILD_TYPE=Debug`                                | `cmake --build ../build/bfD`                      | `lib/x64`     |

> **Note**<br>
The default configuration is `Debug` on Windows, and `Release` on Linux.

> **Note**<br>
Linux `x86` is not supported.

---

---

## Build the C Project

To build the alternative C user guide example `xhptdc8_user_guide_example.c`, you can use `gcc` or `g++` as following:

| Platform          | Configure CMake command                               
| ----------------- | -------------------------------------------------     
| **Windows x86_64**| `cd ug_example\src`<br>`gcc xhptdc8_user_guide_example.c -o ug_example_c -L ..\..\lib\x64 -lxhptdc8_driver_64 -lstdc++ -shared-libgcc`<br>or,<br>`g++ xhptdc8_user_guide_example.c -o ug_example_c -L ..\..\lib\x64 -lxhptdc8_driver_64`
| **Linux x86_64**| `cd ug_example/src`<br>`gcc xhptdc8_user_guide_example.c -o ug_example_c -L ../../lib/x64 -lxhptdc8_driver -lstdc++ -shared-libgcc`<br>or,<br>`g++ xhptdc8_user_guide_example.c -o ug_example_c -L ../../lib/x64 -lxhptdc8_driver`

* For **Windows**, please copy the driver `.dll` from `.\lib\x64\xhptdc8_driver_64.dll` to `ug_example\src` to be able to run `ug_example_c.exe`.
* For **Linux**, to be able to run `ug_example_c` that uses `./lib/x64/xhptdc8_driver.so`, either add `./lib/x64` folder to path (e.g., `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/lib/x64`), or copy `xhptdc8_driver.so` to `/usr/lib` (needs `sudo`).

---

---

## Run/Debug the Sample

### Windows
In a terminal, navigate to the _Output Folder_ and run the executable. Alternatively, if you are using Visual Studio, you can run/debug the program from within Visual Studio.

### Linux
1. `Cronologic PCI Linux Kernel Module` needs to be installed on your machine. You can build it from [`cronologic_linux_kernel`](https://github.com/cronologic-de/cronologic_linux_kernel) on GitHub.
2. In a terminal, navigate to the _Output Folder_ and run the executable. `sudo` privileges may be necessary.