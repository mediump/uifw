#pragma once

#include <fstream>
#include <cstring>
#include <sstream>
#include <string>

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

  static std::string loadTextFile(const char *path)
  {
    std::ifstream file(path);

    if (!file.is_open()) {
      return "";
    }

    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
  }
};

}