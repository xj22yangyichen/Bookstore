#ifndef MYSTRING_H
#define MYSTRING_H

#include <string.h>

struct my_string {
  char a[80];

  my_string() {
    memset(a, 0, sizeof(a));
  }
  my_string(const char* str) {
    memset(a, 0, sizeof(a));
    strncpy(a, str, sizeof(a) - 1);
  }
  my_string(const std::string &str) {
    memset(a, 0, sizeof(a));
    strncpy(a, str.c_str(), sizeof(a) - 1);
  }

  inline bool operator<(const my_string &other) const {
    return strcmp(a, other.a) < 0;
  }
  inline bool operator>(const my_string &other) const {
    return strcmp(a, other.a) > 0;
  }
  inline bool operator==(const my_string &other) const {
    return strcmp(a, other.a) == 0;
  }
  inline bool operator!=(const my_string &other) const {
    return strcmp(a, other.a) != 0;
  }
  inline bool operator<=(const my_string &other) const {
    return strcmp(a, other.a) <= 0;
  }
  inline bool operator>=(const my_string &other) const {
    return strcmp(a, other.a) >= 0;
  }
};

#endif // MYSTRING_H