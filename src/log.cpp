#include "log.h"

int Logs::GetTime() {
  int time;
  logs_.get_info(time, 1);
  return time;
}

void Logs::PutTime(const int time) {
  logs_.write_info(time, 1);
}

void Logs::AddFinance(const double money) {
  std::pair<double, double> last;
  if (finance_.size() == 0) {
    last = {0.0, 0.0};
  } else {
    finance_.read_by_order(last, finance_.size() - 1);
  }
  if (money >= 0) {
    last.first += money;
  } else {
    last.second += -money;
  }
  finance_.write(last);
}

void Logs::QueryFinance() {
  std::pair<double, double> last;
  if (finance_.size() == 0) {
    last = {0.0, 0.0};
  } else {
    finance_.read_by_order(last, finance_.size() - 1);
  }
  printf("+ %.2f - %.2f\n", last.first, last.second);
}

void Logs::QueryFinance(const int count) {
  if (count == 0) {
    printf("\n");
    return;
  }
  if (count > finance_.size()) {
    printf("Invalid\n");
    return;
  }
  if (count == finance_.size()) {
    QueryFinance();
    return;
  }
  std::pair<double, double> last, prev;
  finance_.read_by_order(last, finance_.size() - 1);
  finance_.read_by_order(prev, finance_.size() - count - 1);
  printf("+ %.2f - %.2f\n", last.first - prev.first, last.second - prev.second);
}

std::string BookToString(const Book &book) {
  return "(ISBN: " + std::string(book.isbn_.a)
       + " Title: " + std::string(book.title_.a)
       + " Author: " + std::string(book.author_.a)
       + " Keywords: " + std::string(book.keyword_.a)
       + " Price: " + std::to_string(book.price_) + ")";
}

void Logs::AddLog(const LogEntry &log_entry) {
  std::string log_str;
  switch (log_entry.type_) {
    case LogEntry::QUIT:
      log_str = "[QUIT/EXIT] User " + std::string(log_entry.quit_log_.account_.getID().a)
              + " exited the system at " + std::to_string(log_entry.quit_log_.time_);
      break;
    case LogEntry::LOGIN:
      log_str = "[LOGIN] User " + std::string(log_entry.login_log_.account_.getID().a)
              + " logged in at " + std::to_string(log_entry.login_log_.time_);
      break;
    case LogEntry::LOGOUT:
      log_str = "[LOGOUT] User " + std::string(log_entry.logout_log_.account_.getID().a)
              + " logged out at " + std::to_string(log_entry.logout_log_.time_);
      break;
    case LogEntry::REGISTER:
      log_str = "[REGISTER] User " + std::string(log_entry.register_log_.account_.getID().a)
              + " registered at " + std::to_string(log_entry.register_log_.time_);
      break;
    case LogEntry::PASSWORD_CHANGE:
      log_str = "[PASSWORD_CHANGE] User " + std::string(log_entry.password_change_log_.account_.getID().a)
              + "'s password changed from " + std::string(log_entry.password_change_log_.old_password_.a)
              + " to " + std::string(log_entry.password_change_log_.new_password_.a)
              + " at " + std::to_string(log_entry.password_change_log_.time_);
      break;
    case LogEntry::USER_ADD:
      log_str = "[USER_ADD] User " + std::string(log_entry.user_add_log_.admin_account_.getID().a)
              + " added user " + std::string(log_entry.user_add_log_.new_account_.getID().a)
              + " with privilege " + std::to_string(log_entry.user_add_log_.new_account_.getPrivilege())
              + " at " + std::to_string(log_entry.user_add_log_.time_);
      break;
    case LogEntry::USER_DELETE:
      log_str = "[USER_DELETE] Deleted user " + std::string(log_entry.user_delete_log_.account_.getID().a)
              + " at " + std::to_string(log_entry.user_delete_log_.time_);
      break;
    case LogEntry::BUY:
      log_str = "[BUY] User " + std::string(log_entry.buy_log_.account_.getID().a)
              + " bought " + std::to_string(log_entry.buy_log_.quantity_)
              + " book(s) with ISBN= " + std::string(log_entry.buy_log_.book_.isbn_.a)
              + " spending " + std::to_string(log_entry.buy_log_.book_.price_ * log_entry.buy_log_.quantity_)
              + " at " + std::to_string(log_entry.buy_log_.time_);
      break;
    case LogEntry::MODIFY:
      log_str = "[MODIFY] User " + std::string(log_entry.modify_log_.admin_account_.getID().a)
              + " modified book from " + BookToString(log_entry.modify_log_.old_book_)
              + " to " + BookToString(log_entry.modify_log_.new_book_)
              + " at " + std::to_string(log_entry.modify_log_.time_);
      break;
    case LogEntry::IMPORT:
      log_str = "[IMPORT] User " + std::string(log_entry.import_log_.admin_account_.getID().a)
              + " imported " + std::to_string(log_entry.import_log_.quantity_)
              + " book(s) with ISBN= " + std::string(log_entry.import_log_.book_.isbn_.a)
              + " costing " + std::to_string(log_entry.import_log_.total_cost_)
              + " at " + std::to_string(log_entry.import_log_.time_);
      break;
    default:
      log_str = "[UNKNOWN LOG TYPE]";
      break;
  }
  LogInfo log_info = LogInfo(log_str);

  logs_.write(log_info);

  if (log_entry.type_ == LogEntry::BUY || log_entry.type_ == LogEntry::IMPORT) {
    finance_logs_.write(log_info);
  }

  if (log_entry.type_ == LogEntry::BUY) {
    AddFinance(log_entry.buy_log_.book_.price_ * log_entry.buy_log_.quantity_);
  } else if (log_entry.type_ == LogEntry::IMPORT) {
    AddFinance(-log_entry.import_log_.total_cost_);
  }

  if (log_entry.type_ == LogEntry::USER_ADD ||
      log_entry.type_ == LogEntry::MODIFY ||
      log_entry.type_ == LogEntry::IMPORT) {
    my_string emp_id;
    if (log_entry.type_ == LogEntry::USER_ADD) {
      emp_id = log_entry.user_add_log_.admin_account_.getID();
    } else if (log_entry.type_ == LogEntry::MODIFY) {
      emp_id = log_entry.modify_log_.admin_account_.getID();
    } else if (log_entry.type_ == LogEntry::IMPORT) {
      emp_id = log_entry.import_log_.admin_account_.getID();
    }
    employee_logs_.insert(std::make_pair(emp_id, log_info));
  }
}

void Logs::QueryLog() {
  freopen("../data/logs.txt", "w", stdout);
  printf("Logs:\n");
  int total_logs = logs_.size();
  for (int i = 0; i < total_logs; ++i) {
    LogInfo log_info;
    logs_.read_by_order(log_info, i);
    printf("%s\n", log_info.info);
  }
  fclose(stdout);
}

void Logs::QueryFinanceLog() {
  freopen("../data/finance_logs.txt", "w", stdout);
  printf("Finance Logs:\n");
  int total_logs = finance_logs_.size();
  for (int i = 0; i < total_logs; ++i) {
    LogInfo log_info;
    finance_logs_.read_by_order(log_info, i);
    printf("%s\n", log_info.info);
  }
  fclose(stdout);
}

void Logs::QueryEmployeeLog() {
  freopen("../data/employee_logs.txt", "w", stdout);
  printf("Employee Logs Sorted by Employee's ID:\n");
  std::vector<LogInfo> all_logs = employee_logs_.get_all();
  for (const auto &log_info : all_logs) {
    printf("%s\n", log_info.info);
  }
  fclose(stdout);
}