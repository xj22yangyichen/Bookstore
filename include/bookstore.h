#ifndef BOOKSTORE_H
#define BOOKSTORE_H

#include <stack>
#include "log.h"
#include "book.h"
#include "account.h"
#include "parser.h"

class Bookstore {
private:
  int time;
  Logs logs_;
  Books books_;
  Accounts accounts_;
  std::stack<my_string> logged_in_users_;
  Account current_user_;
  std::stack<int> selected_books_id_;
  int current_selected_book_id_;
  Command current_command_;
public:
  Bookstore();
  ~Bookstore();

  void Run();

  void Login();
  void Logout(bool force = false);
  void Register();
  void PasswordChange();
  void UserAdd();
  void UserDelete();

  void Show();
  void Buy();
  void Select();
  void Modify();
  void Import();

  // due to the shitty command design, this function is merged with Show()
  // void ShowFinance();
  void Report();
  void Log();
};

#endif