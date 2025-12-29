#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <utility>
#include "block_list.h"

class Account {
private:
  int count_, privilege_;
  my_string name_, id_, password_;
public:
  Account() : count_(0), privilege_(0) {
    memset(name_.a, 0, sizeof(name_.a));
    memset(id_.a, 0, sizeof(id_.a));
    memset(password_.a, 0, sizeof(password_.a));
  }
  Account(const my_string &name, const my_string &id, const my_string &password, int privilege = 0)
    : count_(0), privilege_(privilege), name_(name), id_(id), password_(password) {}

  inline int getCount() const { return count_; }
  inline int getPrivilege() const { return privilege_; }
  inline const my_string &getName() const { return name_; }
  inline const my_string &getID() const { return id_; }
  inline const my_string &getPassword() const { return password_; }
  bool checkPassword(const my_string &password) const { return password == password_; }
  void setPassword(const my_string &password) { password_ = password; }
  void updateCount(int d) {
    if (d != 1 && d != -1) {
      throw "Invalid count update value";
    }
    if (count_ + d < 0) {
      throw "Online count cannot be negative";
    }
    count_ += d;
  }

  bool operator<(const Account &other) const {
    if (id_ != other.id_) return id_ < other.id_;
    if (password_ != other.password_) return password_ < other.password_;
    if (name_ != other.name_) return name_ < other.name_;
    if (privilege_ != other.privilege_) return privilege_ < other.privilege_;
    return count_ < other.count_;
  }
  bool operator==(const Account &other) const {
    return id_ == other.id_ && password_ == other.password_ &&
           name_ == other.name_ && privilege_ == other.privilege_ &&
           count_ == other.count_;
  }
  bool operator!=(const Account &other) const {
    return !(*this == other);
  }
};

class Accounts {
private:
  BlockList<my_string, Account> accounts_;
public:
  Accounts(const std::string &filename) : accounts_(filename) {}

  void insert(const Account &account) {
    accounts_.insert(std::make_pair(account.getID(), account)); 
  }

  void remove(const Account &account) {
    accounts_.remove(std::make_pair(account.getID(), account));
  }

  Account find(const my_string &id) {
    std::vector<Account> vals = accounts_.get(id);
    if (vals.empty()) return Account();
    if (vals.size() > 1) throw "Multiple accounts with same ID found";
    return vals[0];
  }

  void update(const my_string &id, const Account &account) {
    Account old_account = find(id);
    accounts_.remove(std::make_pair(id, old_account));
    accounts_.insert(std::make_pair(id, account));
  }
};

#endif // ACCOUNT_H