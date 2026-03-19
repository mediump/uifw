#pragma once

#include <cstring>

namespace ui {

class FileUtils
{
public:
  static const char *getFileExtension(const char *path)
  {
    const char *dotPosition = strrchr(path, '.');

    if (dotPosition && dotPosition != path) {
      return dotPosition;
    }

    return nullptr;
  }
};

}