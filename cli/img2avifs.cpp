/**
 * SPDX-FileComment: CLI for generating multiple sized images
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 */

#include "Img2Avif.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " input.{jpg,png,...} output_prefix [quality 0-100] [speed 0-10]\n";
    std::cerr << "Example: " << argv[0] << " in.jpg out (will generate out_480.avif, out_680.avif, ...)\n";
    return 1;
  }
  
  std::string inPath = argv[1];
  std::string outPrefix = argv[2];
  
  img2avif::ConversionOptions options;
  if (argc >= 4) options.quality = std::stoi(argv[3]);
  if (argc >= 5) options.speed = std::stoi(argv[4]);
  
  std::vector<int> targetWidths = {480, 680, 800, 1024, 1280};
  
  for (int w : targetWidths) {
    options.targetWidth = w;
    std::string outPath = outPrefix + "_" + std::to_string(w) + ".avif";
    
    auto result = img2avif::convert(inPath, outPath, options);
    if (!result.has_value()) {
      std::cerr << "Error converting to " << w << "px: " << result.error() << "\n";
    } else {
      std::cout << "Saved " << outPath << " (width=" << w << ", quality=" << options.quality << ")\n";
    }
  }
  
  return 0;
}