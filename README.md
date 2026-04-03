<div id="top" align="center">
<h1>Img2Avif</h1>

<p>C++23 library and command-line toolset for converting images to the modern AVIF format. </p>

[Report Issue](https://github.com/Zheng-Bote/Img2Avify/issues) · [Request Feature](https://github.com/Zheng-Bote/Img2Avif/pulls)

![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-lightgrey.svg)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/Zheng-Bote/Img2Avif?logo=GitHub)](https://github.com/Zheng-Bote/Img2Avif/releases)

</div>

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

**Table of Contents**

- [Description](#description)
  - [Features](#features)
  - [Dependencies](#dependencies)
  - [Build Instructions](#build-instructions)
  - [Integration](#integration)
    - [Using FetchContent (CMake 3.11+)](#using-fetchcontent-cmake-311)
    - [Using find_package](#using-find_package)
  - [Example Usage](#example-usage)
    - [1:1 Conversion (`img2avif`)](#11-conversion-img2avif)
    - [Multi-Size Responsive Generation (`img2avifs`)](#multi-size-responsive-generation-img2avifs)
    - [Using the C++ Library](#using-the-c-library)
  - [📜 License](#-license)
  - [📄 Changelog](#-changelog)
  - [Author](#author)
  - [Code Contributors](#code-contributors)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Description

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)]()
[![CMake](https://img.shields.io/badge/CMake-3.28+-blue.svg)]()
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-lightgrey.svg)

A high-performance C++23 library and command-line toolset for converting images to the modern AVIF format.

This project leverages `stb_image` for broad input format support, `stb_image_resize2` for high-quality scaling, `libavif` for AVIF encoding, and `Exiv2` for preserving metadata (EXIF, IPTC, XMP).

## Features

- **C++23 Library (`img2avif_lib`)**: A reusable library providing a clean `std::expected` interface for AVIF conversion and resizing.
- **`img2avif` CLI**: A fast 1:1 image to AVIF converter.
- **`img2avifs` CLI**: A batch resizer that outputs multiple responsive target sizes (480, 680, 800, 1024, 1280) from a single input image.
- **Adjustable Quality & Speed**: Exposes AVIF encoding parameters.
- **Metadata Preservation**: Optionally keeps EXIF and XMP data intact in the resulting AVIF files.

## Dependencies

- **C++23 Compiler** (GCC, Clang, or MSVC)
- **CMake** 3.28+
- **libavif** (for AVIF encoding)
- **Exiv2** (for metadata processing)

_(Note: `stb_image` and `stb_image_resize2` are included in the repository as header-only libraries)._

## Build Instructions

1. Ensure you have the required dependencies installed (e.g., via `apt`, `brew`, or `vcpkg`).
2. Clone the repository and run CMake:

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j"$(nproc)"
```

## Integration

### Using FetchContent (CMake 3.11+)

You can easily integrate `img2avif` into your own CMake project using `FetchContent`:

```cmake
include(FetchContent)

FetchContent_Declare(
  img2avif
  GIT_REPOSITORY https://github.com/youruser/img2avif.git
  GIT_TAG main
)

FetchContent_MakeAvailable(img2avif)

# Link against the library
target_link_libraries(your_app PRIVATE img2avif::img2avif_lib)
```

### Using find_package

If you have installed the library on your system:

```cmake
find_package(img2avif REQUIRED)

# Link against the library
target_link_libraries(your_app PRIVATE img2avif::img2avif_lib)
```

## Example Usage

### 1:1 Conversion (`img2avif`)

Convert an image with default quality (90) and speed (3):

```bash
./build/img2avif input.jpg output.avif
```

Convert with specific quality (0-100) and speed (0-10):

```bash
./build/img2avif input.png output.avif 85 4
```

### Multi-Size Responsive Generation (`img2avifs`)

Generate AVIFs at 480w, 680w, 800w, 1024w, and 1280w sizes:

```bash
./build/img2avifs input.jpg out_prefix
# This will output:
# out_prefix_480.avif
# out_prefix_680.avif
# ...
```

### Using the C++ Library

```cpp
#include "Img2Avif.hpp"
#include <iostream>

int main() {
    img2avif::ConversionOptions options;
    options.quality = 80;
    options.targetWidth = 1024; // 0 = keep original

    auto result = img2avif::convert("input.jpg", "output.avif", options);
    if (!result) {
        std::cerr << "Failed: " << result.error() << "\n";
    }
    return 0;
}
```

---

## 📜 License

This project is licensed under the MIT License - see the LICENSE file for details.

Copyright (c) 2026 ZHENG Robert

## 📄 Changelog

For a detailed history of changes, see the [CHANGELOG.md](CHANGELOG.md).

## Author

[![Zheng Robert - Core Development](https://img.shields.io/badge/Github-Zheng_Robert-black?logo=github)](https://www.github.com/Zheng-Bote)

## Code Contributors

![Contributors](https://img.shields.io/github/contributors/Zheng-Bote/Img2Avif?color=dark-green)

---

**Happy coding! 🚀** :vulcan_salute:

<p align="right">(<a href="#top">back to top</a>)</p>
