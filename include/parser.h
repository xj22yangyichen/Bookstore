#ifndef PARSER_H
#define PARSER_H

#include <set>
#include <string>
#include <vector>
#include <ctype.h>
#include <iostream>
#include "mystring.h"

class Command {
private:
  std::string cmd;
  int pos;
public:
  Command() : cmd(""), pos(0) {}
  Command(const std::string &s) : cmd(s), pos(0) {}
  bool read();
  std::string next_token();
};

my_string GetUserID(std::string &s);
my_string GetPassword(std::string &s);
my_string GetUsername(std::string &s);
int GetPrivilege(std::string &s);
my_string GetISBN(std::string s);
my_string GetISBNForSearch(std::string &s);
my_string GetBookName(std::string s);
my_string GetBookNameForSearch(std::string &s);
my_string GetAuthor(std::string s);
my_string GetAuthorForSearch(std::string &s);
my_string GetKeyword(std::string s);
my_string GetKeywordForSearch(std::string &s);
double GetPrice(std::string s);
double GetPriceForSearch(std::string &s);
int GetQuantity(std::string &s);

#endif // PARSER_H