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

## Build the project
- The project is a `Console` App.
- The project is build using `CMake`.
- If you don't have `CMake` installed, refer to [Installing CMake](https://cmake.org/install/) for instructions. You can check if `CMake` is installed by running `cmake --version`.
- On **Windows**, you can build the project using either `CMake` directly, or using Visual Studio 2019 (or later), see [Build Using `CMake`](#build-using-cmake) or [Build Using Visual Studio](#build-using-visual-studio), respectively.
- On **Linux**, you can build the project using `CMake`, see [Build Using `CMake`](#build-using-cmake).
- It is possible to run the example project without actual access to the xHPTDC8 hardware by utilizing the `dummy library`. In that case, copy the DLL corresponding to your particular platform from the `.lib/<platform>` directory to the _driver directory_. E.g., for the platform _x64 Release_ `.exe`, copy `xhptdc8_driver_64.dll` from [`.lib/dummy`](../lib/dummy/) to the _driver directory_.

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
| **x86_64 Debug**  | x64-Debug       | `${projectDir}\..\build\bfvsD`   | Visual Studio 17 2022 Win64 | `<Build root>\Debug`   |
| **x86_64 Release**| x64-Release     | `${projectDir}\..\build\bfvsR`   | Visual Studio 17 2022 Win64 | `<Build root>\Release`   |

> **Notes**
> * The provided file builds the project using Visual Studio 2022, however, you can change `generator` in [`CMakeSettings.json`](./tools/CMakeSettings.json) to any other Visual Studio generator you have on your machine.
> * Driver library is copied automatically by the build script from `./lib` folder to the output folder.

---

### Build using `CMake`

In a terminal, navigate to `tools\` and run the following command:

| Platform          | Configuration | Configure CMake command                            | Compile & Link Command                       | Output Folder |
| ----------------- | ------------- | -------------------------------------------------  | -------------------------------------------- | -------  |
| **Windows x86_64**| Release       | `cmake -B ..\build\bfR -A x64`                     | `cmake --build ..\build\bfR --config Release`| `${projectDir}\..\build\bfR\Release` |
| **Windows x86_64**| Debug         | `cmake -B ..\build\bfD -A x64`                     | `cmake --build ..\build\bfD --config Debug`  | `${projectDir}\..\build\bfD\Debug` |
| **Linux x86_64**  | Release       | `cmake -B ../build/bfR -DCMAKE_BUILD_TYPE=Release` | `cmake --build ../build/bfR`                 | `${projectDir}/../build/bfR` |
| **Linux x86_64**  | Debug         | `cmake -B ../build/bfD -DCMAKE_BUILD_TYPE=Debug`   | `cmake --build ../build/bfD`                 | `${projectDir}/../build/bfD` |

> **Notes**
> * The default configuration is `Debug` on Windows, and `Release` on Linux.
> * For Windows: driver library is copied automatically by the build script from `./lib` folder to the output folder.
> * Linux `x86` is not supported.

---

## Run/Debug the Sample

### Windows
In a terminal, navigate to the _Output Folder_ and run the executable. Alternatively, if you are using Visual Studio, you can run/debug the program from within Visual Studio.

### Linux
1. `Cronologic PCI Linux Kernel Module` needs to be installed on your machine. You can build it from [`cronologic_linux_kernel`](https://github.com/cronologic-de/cronologic_linux_kernel) on GitHub.
2. In a terminal, navigate to the _Output Folder_ and run the executable. `sudo` privileges may be necessary.