#include "Image.hpp"

#include "../../Utils/FileUtils.hpp"
#include "Utils.hpp"

using namespace ui;

SDL_Surface *Image::loadImageFromPath(const char *path, SDL_PixelFormat pixelFormat)
{
  const ImageFormat imageFormat = get_image_format_from_path(path);
  SDL_Surface *result = nullptr;

  switch (imageFormat) {
  case ImageFormat_PNG:
    result = SDL_LoadPNG(path);
    break;
  default:
    UI_LOG_MSG("Error loading image: unsupported format. Path: '%s'", path);
    return result;
  }

  if (result->format != pixelFormat) {
    SDL_Surface *next = SDL_ConvertSurface(result, pixelFormat);
    SDL_DestroySurface(result);
    result = next;
  }

  return result;
}

ImageFormat Image::get_image_format_from_path(const char *path)
{
  const char *ext = FileUtils::getFileExtension(path);

  if (strcmp(ext, ".png") == 0) {
    return ImageFormat_PNG;
  }

  return ImageFormat_Unsupported;
}
