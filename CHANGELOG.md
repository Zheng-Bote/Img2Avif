# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] - 2026-04-03

### Added
- **Img2Avif C++23 Library**: Extracted core conversion logic into a reusable `img2avif_lib` library offering a clean API (`std::expected`).
- **CMake Package Configuration**: Added `img2avifConfig.cmake` and export rules, enabling integration via `FetchContent` or `find_package(img2avif)`.
- **Image Resizing**: Integrated `stb_image_resize2.h` to enable high-quality linear scaling of images during conversion.
- **`img2avif` CLI Tool**: Migrated the existing `src/main.cpp` logic into `cli/img2avif.cpp` to serve as a standalone 1:1 conversion tool.
- **`img2avifs` CLI Tool**: Introduced a new executable (`cli/img2avifs.cpp`) that automatically generates multiple responsive AVIF sizes (480, 680, 800, 1024, 1280) from a single input file.
- **Adjustable Parameters**: The library now accepts `ConversionOptions` allowing the customization of target dimensions, encoding quality, encoding speed, and metadata retention.
- **Project Documentation**: Added comprehensive `README.md` and `CHANGELOG.md` to document building, dependencies, and usage.

### Changed
- Reorganized directory structure by creating `include/`, `src/`, and `cli/` to accommodate the library-first architecture.
- Modernized Exiv2 metadata handling to be compatible with version 0.28+ (using `ExifParser::encode` and `XmpParser::encode`).
- Re-configured `CMakeLists.txt` to expose `img2avif_lib` publicly and link it against the separate CLI executables.

### Removed
- Removed the monolithic `src/main.cpp` script in favor of the modular `cli/` tools.