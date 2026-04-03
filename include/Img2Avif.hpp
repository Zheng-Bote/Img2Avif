/**
 * SPDX-FileComment: Image to AVIF conversion library header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file Img2Avif.hpp
 * @brief Header for Img2Avif conversion library
 * @version 0.1.0
 * @date 2026-04-03
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#pragma once

#include <expected>
#include <string>

namespace img2avif {

/**
 * @brief Options for the image to AVIF conversion.
 */
struct ConversionOptions {
    int quality = 90;           ///< Quality level 0-100
    int speed = 3;              ///< Encoding speed 0-10
    int targetWidth = 0;        ///< Target width (0 = keep original aspect)
    int targetHeight = 0;       ///< Target height (0 = keep original aspect)
    bool keepMetadata = true;   ///< Whether to keep EXIF/XMP metadata
};

/**
 * @brief Converts an input image to AVIF format.
 *
 * @param inPath The input file path.
 * @param outPath The output file path.
 * @param options The conversion options.
 * @return std::expected<void, std::string> Success or error message.
 */
[[nodiscard]] std::expected<void, std::string> convert(
    const std::string& inPath, 
    const std::string& outPath, 
    const ConversionOptions& options = {}
);

} // namespace img2avif