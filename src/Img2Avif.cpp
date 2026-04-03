/**
 * SPDX-FileComment: Image to AVIF conversion library implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file Img2Avif.cpp
 * @brief Implementation for Img2Avif conversion library
 * @version 0.1.0
 * @date 2026-04-03
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include "Img2Avif.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include <avif/avif.h>
#include <exiv2/exiv2.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

namespace img2avif {

static void mapQualityToQuantizers(int quality, int &minQ, int &maxQ) {
  int q = std::clamp(quality, 0, 100);
  double norm = q / 100.0;
  double expFactor = 2.2;
  double mapped = std::pow(1.0 - norm, expFactor);
  minQ = static_cast<int>(std::round(mapped * 20.0));
  maxQ = static_cast<int>(std::round(mapped * 48.0));
  minQ = std::clamp(minQ, 0, 63);
  maxQ = std::clamp(maxQ, 0, 63);
}

std::expected<void, std::string> convert(const std::string& inPath, const std::string& outPath, const ConversionOptions& options) {
  int origW = 0, origH = 0, channels = 0;
  unsigned char *pixels = stbi_load(inPath.c_str(), &origW, &origH, &channels, 4);
  if (!pixels) {
    return std::unexpected("Failed to load input image: " + inPath);
  }

  int w = origW;
  int h = origH;

  if (options.targetWidth > 0 || options.targetHeight > 0) {
    if (options.targetWidth > 0 && options.targetHeight == 0) {
      w = options.targetWidth;
      h = static_cast<int>(std::round(static_cast<double>(origH) * w / origW));
    } else if (options.targetHeight > 0 && options.targetWidth == 0) {
      h = options.targetHeight;
      w = static_cast<int>(std::round(static_cast<double>(origW) * h / origH));
    } else {
      w = options.targetWidth;
      h = options.targetHeight;
    }
  }

  unsigned char *resizedPixels = nullptr;
  if (w != origW || h != origH) {
    resizedPixels = (unsigned char*)std::malloc(w * h * 4);
    if (!resizedPixels) {
      stbi_image_free(pixels);
      return std::unexpected("Failed to allocate memory for resized image");
    }
    stbir_resize_uint8_linear(pixels, origW, origH, 0, resizedPixels, w, h, 0, (stbir_pixel_layout)STBIR_RGBA);
    stbi_image_free(pixels);
    pixels = resizedPixels;
  }

  std::vector<uint8_t> exifBytes;
  std::vector<uint8_t> xmpBytes;

  if (options.keepMetadata) {
    try {
      auto imageExiv = Exiv2::ImageFactory::open(inPath);
      if (imageExiv.get() != nullptr) {
        imageExiv->readMetadata();
        auto &exifData = imageExiv->exifData();
        if (!exifData.empty()) {
          Exiv2::Blob blob;
          Exiv2::ExifParser::encode(blob, imageExiv->byteOrder(), exifData);
          exifBytes.assign(blob.begin(), blob.end());
        }
        auto &xmpData = imageExiv->xmpData();
        if (!xmpData.empty()) {
          std::string xmpPacket;
          if (Exiv2::XmpParser::encode(xmpPacket, xmpData) == 0) {
            xmpBytes.assign(xmpPacket.begin(), xmpPacket.end());
          }
        }
      }
    } catch (...) {
      // Ignore metadata read errors
    }
  }

  avifImage *image = avifImageCreate(w, h, 10, AVIF_PIXEL_FORMAT_YUV444);
  if (!image) {
    if (resizedPixels) std::free(pixels); else stbi_image_free(pixels);
    return std::unexpected("avifImageCreate failed");
  }

  if (channels == 4) {
    if (avifImageAllocatePlanes(image, AVIF_PLANES_ALL) != AVIF_RESULT_OK) {
      if (resizedPixels) std::free(pixels); else stbi_image_free(pixels);
      avifImageDestroy(image);
      return std::unexpected("avifImageAllocatePlanes(ALL) failed");
    }
  } else {
    if (avifImageAllocatePlanes(image, AVIF_PLANES_YUV) != AVIF_RESULT_OK) {
      if (resizedPixels) std::free(pixels); else stbi_image_free(pixels);
      avifImageDestroy(image);
      return std::unexpected("avifImageAllocatePlanes(YUV) failed");
    }
  }

  if (!exifBytes.empty()) {
    avifRWDataFree(&image->exif);
    image->exif.data = (uint8_t *)avifAlloc(exifBytes.size());
    if (image->exif.data) {
      std::memcpy(image->exif.data, exifBytes.data(), exifBytes.size());
      image->exif.size = exifBytes.size();
    } else {
      image->exif.size = 0;
    }
  }

  if (!xmpBytes.empty()) {
    avifRWDataFree(&image->xmp);
    image->xmp.data = (uint8_t *)avifAlloc(xmpBytes.size());
    if (image->xmp.data) {
      std::memcpy(image->xmp.data, xmpBytes.data(), xmpBytes.size());
      image->xmp.size = xmpBytes.size();
    } else {
      image->xmp.size = 0;
    }
  }

  avifRGBImage rgb;
  avifRGBImageSetDefaults(&rgb, image);
  rgb.format = AVIF_RGB_FORMAT_RGBA;
  rgb.depth = 8;
  rgb.pixels = pixels;
  rgb.rowBytes = w * 4;

  avifResult conv = avifImageRGBToYUV(image, &rgb);
  if (conv != AVIF_RESULT_OK) {
    avifImageDestroy(image);
    if (resizedPixels) std::free(pixels); else stbi_image_free(pixels);
    return std::unexpected(std::string("avifImageRGBToYUV failed: ") + avifResultToString(conv));
  }

  avifEncoder *encoder = avifEncoderCreate();
  if (!encoder) {
    avifImageDestroy(image);
    if (resizedPixels) std::free(pixels); else stbi_image_free(pixels);
    return std::unexpected("avifEncoderCreate failed");
  }

  unsigned int hwThreads = std::thread::hardware_concurrency();
  encoder->maxThreads = (hwThreads == 0) ? 1 : hwThreads;
  encoder->speed = std::clamp(options.speed, 0, 10);

  int minQ = 0, maxQ = 0;
  mapQualityToQuantizers(options.quality, minQ, maxQ);
  encoder->minQuantizer = minQ;
  encoder->maxQuantizer = maxQ;
  encoder->minQuantizerAlpha = std::max(0, minQ - 2);
  encoder->maxQuantizerAlpha = std::max(0, maxQ - 2);

  avifRWData output = AVIF_DATA_EMPTY;
  avifResult add = avifEncoderAddImage(encoder, image, 1, AVIF_ADD_IMAGE_FLAG_SINGLE);
  if (add != AVIF_RESULT_OK) {
    avifRWDataFree(&output);
    avifEncoderDestroy(encoder);
    avifImageDestroy(image);
    if (resizedPixels) std::free(pixels); else stbi_image_free(pixels);
    return std::unexpected(std::string("avifEncoderAddImage failed: ") + avifResultToString(add));
  }

  avifResult finish = avifEncoderFinish(encoder, &output);
  if (finish != AVIF_RESULT_OK) {
    avifRWDataFree(&output);
    avifEncoderDestroy(encoder);
    avifImageDestroy(image);
    if (resizedPixels) std::free(pixels); else stbi_image_free(pixels);
    return std::unexpected(std::string("avifEncoderFinish failed: ") + avifResultToString(finish));
  }

  FILE *f = std::fopen(outPath.c_str(), "wb");
  if (!f) {
    avifRWDataFree(&output);
    avifEncoderDestroy(encoder);
    avifImageDestroy(image);
    if (resizedPixels) std::free(pixels); else stbi_image_free(pixels);
    return std::unexpected("Failed to open output file: " + outPath);
  }

  if (std::fwrite(output.data, 1, output.size, f) != output.size) {
    std::fclose(f);
    avifRWDataFree(&output);
    avifEncoderDestroy(encoder);
    avifImageDestroy(image);
    if (resizedPixels) std::free(pixels); else stbi_image_free(pixels);
    return std::unexpected("Failed to write full output file: " + outPath);
  }

  std::fclose(f);
  avifRWDataFree(&output);
  avifEncoderDestroy(encoder);
  avifImageDestroy(image);
  if (resizedPixels) std::free(pixels); else stbi_image_free(pixels);

  return {};
}

} // namespace img2avif