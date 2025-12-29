#include "parser.h"

bool Command::read() {
  pos = 0;
  cmd.clear();
  char ch;
  while (std::cin.get(ch)) {
    if (ch == '\r' || ch == '\n') {
      return true;
    }
    cmd += ch;
  }
  return false;
}

std::string Command::next_token() {
  while (pos < cmd.size() && cmd[pos] == ' ') {
    ++pos;
  }
  int start = pos;
  while (pos < cmd.size() && cmd[pos] != ' ') {
    ++pos;
  }
  return cmd.substr(start, pos - start);
}

my_string GetUserID(std::string &s) {
  if (s.size() > 30) {
    return my_string("\n");
  }
  my_string res = my_string();
  for (int i = 0; i < s.size(); ++i) {
    if (!isalnum(s[i]) && s[i] != '_') {
      return my_string("\n");
    }
    res.a[i] = s[i];
  }
  return res;
}

my_string GetPassword(std::string &s) {
  if (s.size() > 30) {
    return my_string("\n");
  }
  my_string res = my_string();
  for (int i = 0; i < s.size(); ++i) {
    if (!isalnum(s[i]) && s[i] != '_') {
      return my_string("\n");
    }
    res.a[i] = s[i];
  }
  return res;
}

my_string GetUsername(std::string &s) {
  if (s.size() > 30) {
    return my_string("\n");
  }
  my_string res = my_string();
  for (int i = 0; i < s.size(); ++i) {
    if (!isprint(s[i])) {
      return my_string("\n");
    }
    res.a[i] = s[i];
  }
  return res;
}

int GetPrivilege(std::string &s) {
  if (s == "0") return 0;
  if (s == "1") return 1;
  if (s == "3") return 3;
  if (s == "7") return 7;
  return -1;
}

my_string GetISBN(std::string s) {
  if (s.size() > 20) {
    return my_string("\n");
  }
  my_string res = my_string();
  for (int i = 0; i < s.size(); ++i) {
    if (!isprint(s[i])) {
      return my_string("\n");
    }
    res.a[i] = s[i];
  }
  return res;
}

my_string GetISBNForSearch(std::string &s) {
  if (s.size() < 7 || s.substr(0, 6) != "-ISBN=") {
    return my_string("\n");
  }
  return GetISBN(s.substr(6));
}

my_string GetBookName(std::string s) {
  if (s.size() > 60) {
    return my_string("\n");
  }
  my_string res = my_string();
  for (int i = 0; i < s.size(); ++i) {
    if (!isprint(s[i]) || s[i] == '\"') {
      return my_string("\n");
    }
    res.a[i] = s[i];
  }
  return res;
}

my_string GetBookNameForSearch(std::string &s) {
  if (s.size() < 9 || s.substr(0, 7) != "-name=\"" || s.back() != '\"') {
    return my_string("\n");
  }
  return GetBookName(s.substr(7, s.size() - 8));
}

my_string GetAuthor(std::string s) {
  if (s.size() > 60) {
    return my_string("\n");
  }
  my_string res = my_string();
  for (int i = 0; i < s.size(); ++i) {
    if (!isprint(s[i]) || s[i] == '\"') {
      return my_string("\n");
    }
    res.a[i] = s[i];
  }
  return res;
}

my_string GetAuthorForSearch(std::string &s) {
  if (s.size() < 11 || s.substr(0, 9) != "-author=\"" || s.back() != '\"') {
    return my_string("\n");
  }
  return GetAuthor(s.substr(9, s.size() - 10));
}

my_string GetKeyword(std::string s) {
  if (s.size() > 60 || s.back() == '|') {
    return my_string("\n");
  }
  std::set<std::string> keywords;
  keywords.insert("");
  int start = 0;
  for (int i = 0; i < s.size(); ++i) {
    if (!isprint(s[i]) || s[i] == '\"') {
      return my_string("\n");
    }
    if (i == s.size() - 1 || s[i] == '|') {
      if (i == s.size() - 1) {
        i++;
      }
      if (start == i) {
        return my_string("\n");
      }
      std::string kw = s.substr(start, i - start);
      if (keywords.count(kw)) {
        return my_string("\n");
      }
      keywords.insert(kw);
      start = i + 1;
    }
  }
  return my_string(s);
}

my_string GetKeywordForSearch(std::string &s) {
  if (s.size() < 12 || s.substr(0, 10) != "-keyword=\"" || s.back() != '\"') {
    return my_string("\n");
  }
  return GetKeyword(s.substr(10, s.size() - 11));
}

double GetPrice(std::string s) {
  if (s.size() > 13) {
    return -1.0;
  }
  if (s.find_first_of('.') != s.find_last_of('.')) {
    return -1.0;
  }
  if (s.find('.') == std::string::npos) {
    return GetQuantity(s);
  }

  int dot_pos = s.find('.');
  if (dot_pos == s.size() - 1 || dot_pos == 0) {
    return -1.0;
  }
  if (s.size() - dot_pos - 1 > 2) {
    return -1.0;
  }
  if (dot_pos > 1 && s[0] == '0') {
    return -1.0;
  }

  double price = 0.0;
  for (int i = 0; i < s.size(); ++i) {
    if (i == dot_pos) {
      continue;
    }
    if (!isdigit(s[i])) {
      return -1.0;
    }
    price = price * 10 + (s[i] - '0');
  }
  for (int i = dot_pos + 1; i < s.size(); ++i) {
    price /= 10.0;
  }
  return price;
}

double GetPriceForSearch(std::string &s) {
  if (s.size() < 8 || s.substr(0, 7) != "-price=") {
    return -1.0;
  }
  return GetPrice(s.substr(7));
}

int GetQuantity(std::string &s) {
  if (s.size() > 10) {
    return -1;
  }
  if (s.size() > 1 && s[0] == '0') {
    return -1;
  }
  long long quantity = 0;
  for (int i = 0; i < s.size(); ++i) {
    if (!isdigit(s[i])) {
      return -1;
    }
    quantity = quantity * 10 + (s[i] - '0');
  }
  if (quantity > __INT_MAX__) {
    return -1;
  }
  return static_cast<int>(quantity);
}