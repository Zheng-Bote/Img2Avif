/**
 * SPDX-FileComment: CLI for single image conversion
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 */

#include "Img2Avif.hpp"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " input.{jpg,png,...} output.avif [quality 0-100] [speed 0-10]\n";
    return 1;
  }
  
  std::string inPath = argv[1];
  std::string outPath = argv[2];
  
  img2avif::ConversionOptions options;
  if (argc >= 4) options.quality = std::stoi(argv[3]);
  if (argc >= 5) options.speed = std::stoi(argv[4]);
  
  auto result = img2avif::convert(inPath, outPath, options);
  if (!result.has_value()) {
    std::cerr << "Error: " << result.error() << "\n";
    return 1;
  }
  
  std::cout << "Wrote " << outPath << " (quality=" << options.quality << " speed=" << options.speed << ")\n";
  return 0;
}