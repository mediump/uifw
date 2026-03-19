#pragma once

#include <string>
#include <vector>

class StringUtils
{
public:
  static std::vector<std::string> split(const std::string &s,
                                        const std::string &delimiter)
  {
    size_t last = 0;
    size_t next = 0;

    std::vector<std::string> results;

    while ((next = s.find(delimiter, last)) != std::string::npos) {
      std::string token = s.substr(last, next - last);
      results.emplace_back(token);

      last = next + 1;
    }

    results.emplace_back(s.substr(last));

    return results;
  }
};
