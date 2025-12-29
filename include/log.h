#ifndef LOG_H
#define LOG_H

#include <iostream>
#include "book.h"
#include "account.h"
#include "memory_river.h"

struct LogInfo {
  char info[1000];
  LogInfo(std::string s = "") {
    memset(info, 0, sizeof(info));
    strncpy(info, s.c_str(), sizeof(info) - 1);
  }

  bool operator<(const LogInfo &other) const {
    return strcmp(info, other.info) < 0;
  }
};

struct QuitLog {
  int time_;
  Account account_;
};
struct LoginLog {
  int time_;
  Account account_;
};
struct LogoutLog {
  int time_;
  Account account_;
};
struct RegisterLog {
  int time_;
  Account account_;
};
struct PasswordChangeLog {
  int time_;
  Account account_;
  my_string old_password_;
  my_string new_password_;
};
struct UserAddLog {
  int time_;
  Account admin_account_;
  Account new_account_;
};
struct UserDeleteLog {
  int time_;
  Account account_;
};
struct BuyLog {
  int time_;
  Account account_;
  Book book_;
  int quantity_;
};
struct ModifyLog {
  int time_;
  Account admin_account_;
  Book old_book_;
  Book new_book_;
};
struct ImportLog {
  int time_;
  Account admin_account_;
  Book book_;
  int quantity_;
  double total_cost_;
};

struct LogEntry {
  enum LogType {
    QUIT,
    LOGIN,
    LOGOUT,
    REGISTER,
    PASSWORD_CHANGE,
    USER_ADD,
    USER_DELETE,
    BUY,
    MODIFY,
    IMPORT
  } type_;
  union {
    QuitLog quit_log_;
    LoginLog login_log_;
    LogoutLog logout_log_;
    RegisterLog register_log_;
    PasswordChangeLog password_change_log_;
    UserAddLog user_add_log_;
    UserDeleteLog user_delete_log_;
    BuyLog buy_log_;
    ModifyLog modify_log_;
    ImportLog import_log_;
  };
};

std::string BookToString(const Book &book);

class Logs {
private:
  // prefix sum of finance changes
  MemoryRiver<std::pair<double, double>> finance_;
  MemoryRiver<LogInfo> logs_;
  MemoryRiver<LogInfo> finance_logs_;
  BlockList<my_string, LogInfo> employee_logs_;
public:
  Logs() : finance_("finance.dat"),
           logs_("logs.dat"),
           finance_logs_("finance_logs.dat"),
           employee_logs_("employee_logs") {
    finance_.initialise();
    logs_.initialise();
    finance_logs_.initialise();
  }

  int GetTime();
  void PutTime(const int time);
  void AddFinance(const double money);
  void QueryFinance();
  void QueryFinance(const int count);
  void AddLog(const LogEntry &log_entry);
  void QueryLog();
  void QueryFinanceLog();
  void QueryEmployeeLog();
};

#endif // LOG_H