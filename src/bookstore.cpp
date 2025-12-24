#include "bookstore.h"

Bookstore::Bookstore() : logs_(), books_(), accounts_("accounts") {
  time = logs_.GetTime();
  // std::cerr << "Current time: " << time << std::endl;
  if (time == 0) {
    // std::cerr << "First time running, initializing root account." << std::endl;
    accounts_.insert(Account(my_string("Administrator"), my_string("root"), my_string("sjtu"), 7));
    // std::cerr << accounts_.find(my_string("root")).getName().a << std::endl;
  }
}

Bookstore::~Bookstore() {
  while (!logged_in_users_.empty()) {
    Logout(true);
  }
  ++time;
  logs_.PutTime(time);
}

void Bookstore::Run() {
  while (true) {
    bool has_command = current_command_.read();
    std::string command = current_command_.next_token();
    // std::cerr << "Command: " << command << std::endl;
    if (command.empty()) {
      if (!has_command) {
        break;
      }
      continue;
    }

    if (command == "exit" || command == "quit") {
      if (!current_command_.next_token().empty()) {
        printf("Invalid\n");
        if (!has_command) {
          break;
        }
        continue;
      }
      QuitLog log;
      log.time_ = ++time;
      log.account_ = current_user_;
      LogEntry log_entry{};
      log_entry.type_ = LogEntry::QUIT;
      log_entry.quit_log_ = log;
      logs_.AddLog(log_entry);
      return;
    }
    
    if (command == "su") {
      Login();
    } else if (command == "logout") {
      Logout();
    } else if (command == "register") {
      Register();
    } else if (command == "passwd") {
      PasswordChange();
    } else if (command == "useradd") {
      UserAdd();
    } else if (command == "delete") {
      UserDelete();
    } else if (command == "show") {
      Show();
    } else if (command == "buy") {
      Buy();
    } else if (command == "select") {
      Select();
    } else if (command == "modify") {
      Modify();
    } else if (command == "import") {
      Import();
    } else if (command == "report") {
      Report();
    } else if (command == "log") {
      Log();
    } else {
      printf("Invalid\n");
    }

    if (!has_command) {
      break;
    }
  }
  if (!logged_in_users_.empty()) {
    exit(1);
  }
}

void Bookstore::Login() {
  // std::cerr << "Login command invoked." << std::endl;
  std::string token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  my_string user_id = GetUserID(token);
  // std::cerr << "User ID: " << user_id.a << std::endl;
  if (user_id.a[0] == '\n') {
    printf("Invalid\n");
    return;
  }

  Account account = accounts_.find(user_id);
  if (account == Account()) {
    // std::cerr << "Account not found." << std::endl;
    printf("Invalid\n");
    return;
  }

  token = current_command_.next_token();
  if (token.empty()) {
    // std::cerr << "No password provided, checking privilege." << std::endl;
    if (account.getPrivilege() >= current_user_.getPrivilege()) {
      printf("Invalid\n");
      return;
    }
  } else {
    my_string password = GetPassword(token);
    // std::cerr << "Password: " << password.a << std::endl;
    if (password.a[0] == '\n' || !account.checkPassword(password)) {
      printf("Invalid\n");
      return;
    }
  }

  if (!current_command_.next_token().empty()) {
    printf("Invalid\n");
    return;
  }

  account.updateCount(1);
  accounts_.update(user_id, account);
  logged_in_users_.push(user_id);
  current_user_ = account;
  selected_books_id_.push(-1);
  current_selected_book_ = Book();

  LoginLog log;
  log.time_ = ++time;
  log.account_ = account;
  LogEntry log_entry{};
  log_entry.type_ = LogEntry::LOGIN;
  log_entry.login_log_ = log;
  logs_.AddLog(log_entry);
}

void Bookstore::Logout(bool force) {
  if (logged_in_users_.empty()) {
    printf("Invalid\n");
    return;
  }
  if (!force && !current_command_.next_token().empty()) {
    printf("Invalid\n");
    return;
  }

  LogoutLog log;
  log.time_ = ++time;
  log.account_ = current_user_;
  LogEntry log_entry{};
  log_entry.type_ = LogEntry::LOGOUT;
  log_entry.logout_log_ = log;
  logs_.AddLog(log_entry);

  current_user_.updateCount(-1);
  accounts_.update(current_user_.getID(), current_user_);
  logged_in_users_.pop();
  selected_books_id_.pop();
  if (logged_in_users_.empty()) {
    current_user_ = Account();
    current_selected_book_ = Book();
  } else {
    current_user_ = accounts_.find(logged_in_users_.top());
    current_selected_book_ = selected_books_id_.top() == -1 ? Book()
                           : books_.GetBooksById(selected_books_id_.top());
  }
}

void Bookstore::Register() {
  std::string token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  my_string user_id = GetUserID(token);
  if (user_id.a[0] == '\n' || accounts_.find(user_id) != Account()) {
    printf("Invalid\n");
    return;
  }

  token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  my_string password = GetPassword(token);
  if (password.a[0] == '\n') {
    printf("Invalid\n");
    return;
  }

  token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  my_string name = my_string(token);
  if (name.a[0] == '\n') {
    printf("Invalid\n");
    return;
  }
  
  if (!current_command_.next_token().empty()) {
    printf("Invalid\n");
    return;
  }

  Account new_account(name, user_id, password, 1);
  accounts_.insert(new_account);

  RegisterLog log;
  log.time_ = ++time;
  log.account_ = new_account;
  LogEntry log_entry{};
  log_entry.type_ = LogEntry::REGISTER;
  log_entry.register_log_ = log;
  logs_.AddLog(log_entry);
}

void Bookstore::PasswordChange() {
  if (current_user_.getPrivilege() < 1) {
    printf("Invalid\n");
    return;
  }

  std::string token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  my_string user_id = GetUserID(token);
  if (user_id.a[0] == '\n') {
    printf("Invalid\n");
    return;
  }

  Account account = accounts_.find(user_id);
  if (account == Account()) {
    printf("Invalid\n");
    return;
  }

  token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  my_string old_password = GetPassword(token);
  if (old_password.a[0] == '\n') {
    printf("Invalid\n");
    return;
  }

  token = current_command_.next_token();
  my_string new_password;
  if (token.empty()) {
    if (current_user_.getPrivilege() < 7) {
      printf("Invalid\n");
      return;
    }
    new_password = old_password;
    old_password = account.getPassword();
  } else {
    new_password = GetPassword(token);
    if (new_password.a[0] == '\n' || !account.checkPassword(old_password)) {
      printf("Invalid\n");
      return;
    }
    if (!current_command_.next_token().empty()) {
      printf("Invalid\n");
      return;
    }
  }

  account.setPassword(new_password);
  if (user_id == current_user_.getID()) {
    current_user_ = account;
  }
  accounts_.update(user_id, account);

  PasswordChangeLog log;
  log.time_ = ++time;
  log.account_ = account;
  log.old_password_ = old_password;
  log.new_password_ = new_password;
  LogEntry log_entry{};
  log_entry.type_ = LogEntry::PASSWORD_CHANGE;
  log_entry.password_change_log_ = log;
  logs_.AddLog(log_entry);
}

void Bookstore::UserAdd() {
  if (current_user_.getPrivilege() < 3) {
    printf("Invalid\n");
    return;
  }

  std::string token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  my_string user_id = GetUserID(token);
  if (user_id.a[0] == '\n' || accounts_.find(user_id) != Account()) {
    printf("Invalid\n");
    return;
  }

  token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  my_string password = GetPassword(token);
  if (password.a[0] == '\n') {
    printf("Invalid\n");
    return;
  }

  token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  int privilege = GetPrivilege(token);  
  if (privilege == -1 || privilege >= current_user_.getPrivilege()) {
    printf("Invalid\n");
    return;
  }

  token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  my_string name = my_string(token);
  if (name.a[0] == '\n') {
    printf("Invalid\n");
    return;
  }

  if (!current_command_.next_token().empty()) {
    printf("Invalid\n");
    return;
  }

  Account new_account(name, user_id, password, privilege);
  accounts_.insert(new_account);

  UserAddLog log;
  log.time_ = ++time;
  log.admin_account_ = current_user_;
  log.new_account_ = new_account;
  LogEntry log_entry{};
  log_entry.type_ = LogEntry::USER_ADD;
  log_entry.user_add_log_ = log;
  logs_.AddLog(log_entry);
}

void Bookstore::UserDelete() {
  if (current_user_.getPrivilege() < 7) {
    printf("Invalid\n");
    return;
  }

  std::string token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  my_string user_id = GetUserID(token);
  if (user_id.a[0] == '\n') {
    printf("Invalid\n");
    return;
  }

  Account account = accounts_.find(user_id);
  if (account == Account() || account.getCount()) {
    printf("Invalid\n");
    return;
  }

  if (!current_command_.next_token().empty()) {
    printf("Invalid\n");
    return;
  }

  accounts_.remove(account);

  UserDeleteLog log;
  log.time_ = ++time;
  log.account_ = account;
  LogEntry log_entry{};
  log_entry.type_ = LogEntry::USER_DELETE;
  log_entry.user_delete_log_ = log;
  logs_.AddLog(log_entry);
}

void Bookstore::Show() {
  if (current_user_.getPrivilege() < 1) {
    printf("Invalid\n");
    return;
  }

  std::string token = current_command_.next_token();
  if (token.empty()) {
    std::vector<Book> all_books = books_.GetAllBooks();
    if (all_books.empty()) {
      printf("\n");
    }
    for (const Book &book : all_books) {
      book.Print();
    }
  }
  
  else if (token == "finance") {
    if (current_user_.getPrivilege() < 7) {
      printf("Invalid\n");
      return;
    }
    token = current_command_.next_token();
    if (token.empty()) {
      logs_.QueryFinance();
    } else {
      int count = GetQuantity(token);
      if (count < 0 || !current_command_.next_token().empty()) {
        printf("Invalid\n");
        return;
      }
      logs_.QueryFinance(count);
    }
    return;
  }
  
  else {
    bool found = false;
    my_string ISBN = GetISBNForSearch(token);
    if (ISBN.a[0] != '\n') {
      if (!current_command_.next_token().empty()) {
        printf("Invalid\n");
        return;
      }
      auto books = books_.GetBooksByIsbn(ISBN);
      if (books.empty()) {
        printf("\n");
      } else {
        for (auto &book : books) {
          book.Print();
        }
      }
      found = true;
    }
    my_string book_name = GetBookNameForSearch(token);
    if (!found && book_name.a[0] != '\n') {
      if (!current_command_.next_token().empty()) {
        printf("Invalid\n");
        return;
      }
      auto books = books_.GetBooksByTitle(book_name);
      if (books.empty()) {
        printf("\n");
      } else {
        for (auto &book : books) {
          book.Print();
        }
      }
      found = true;
    }
    my_string author = GetAuthorForSearch(token);
    if (!found && author.a[0] != '\n') {
      if (!current_command_.next_token().empty()) {
        printf("Invalid\n");
        return;
      }
      auto books = books_.GetBooksByAuthor(author);
      if (books.empty()) {
        printf("\n");
      } else {
        for (auto &book : books) {
          book.Print();
        }
      }
      found = true;
    }
    my_string keyword = GetKeywordForSearch(token);
    if (!found && keyword.a[0] != '\n') {
      if (!current_command_.next_token().empty()) {
        printf("Invalid\n");
        return;
      }
      auto books = books_.GetBooksByKeyword(keyword);
      if (books.empty()) {
        printf("\n");
      } else {
        for (auto &book : books) {
          book.Print();
        }
      }
      found = true;
    }
    if (!found) {
      printf("Invalid\n");
      return;
    }
  }
}

void Bookstore::Buy() {
  if (current_user_.getPrivilege() < 1) {
    // std::cerr << "Privilege too low to buy." << std::endl;
    printf("Invalid\n");
    return;
  }

  std::string token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  my_string ISBN = GetISBN(token);
  if (ISBN.a[0] == '\n') {
    // std::cerr << "Invalid ISBN format." << std::endl;
    printf("Invalid\n");
    return;
  }

  token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  int quantity = GetQuantity(token);
  if (quantity <= 0) {
    printf("Invalid\n");
    return;
  }

  if (!current_command_.next_token().empty()) {
    printf("Invalid\n");
    return;
  }

  auto books = books_.GetBooksByIsbn(ISBN);
  if (books.empty()) {
    // std::cerr << "Book with ISBN not found: " << ISBN.a << std::endl;
    printf("Invalid\n");
    return;
  }
  Book book = books[0];

  // std::cerr << "Attempting to buy book ISBN: " << ISBN.a << std::endl;
  // std::cerr << "Requested: " << quantity << ", Available: " << book.quantity_ << std::endl;
  if (book.quantity_ < quantity) {
    // std::cerr << "Not enough quantity for book: " << ISBN.a << std::endl;
    printf("Invalid\n");
    return;
  }
  double total_price = book.price_ * quantity;
  std::cout << std::fixed << std::setprecision(2) << total_price << std::endl;
  books_.ModifyQuantity(ISBN, book.quantity_ - quantity);
  if (current_selected_book_.isbn_ == ISBN) {
    current_selected_book_.quantity_ -= quantity;
  }

  BuyLog log;
  log.time_ = ++time;
  log.account_ = current_user_;
  log.book_ = book;
  log.quantity_ = quantity;
  LogEntry log_entry{};
  log_entry.type_ = LogEntry::BUY;
  log_entry.buy_log_ = log;
  logs_.AddLog(log_entry);
}

void Bookstore::Select() {
  if (current_user_.getPrivilege() < 3) {
    printf("Invalid\n");
    return;
  }

  std::string token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  my_string ISBN = GetISBN(token);
  if (ISBN.a[0] == '\n') {
    printf("Invalid\n");
    return;
  }

  if (!current_command_.next_token().empty()) {
    printf("Invalid\n");
    return;
  }

  auto books = books_.GetBooksByIsbn(ISBN);
  Book book;
  // std::cerr << "Find " << books.size() << " books with ISBN: " << ISBN.a << std::endl;
  if (books.empty()) {
    book = Book(books_.size(), ISBN);
    books_.add(book);
    // std::cerr << "Added new book with ISBN: " << ISBN.a << std::endl;
  } else {
    book = books[0];
    // std::cerr << "Selected existing book with ID: " << book.id_ << std::endl;
  }
  // std::cerr << "Selected book ID: " << book.id_ << std::endl;
  selected_books_id_.pop();
  selected_books_id_.push(book.id_);
  current_selected_book_ = book;
}

void Bookstore::Modify() {
  Book old_book = current_selected_book_;
  if (current_user_.getPrivilege() < 3 || old_book.id_ == -1) {
    // std::cerr << "Invalid modify attempt by user: " << current_user_.getID().a << std::endl;
    printf("Invalid\n");
    return;
  }

  std::string token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }

  // std::cerr << "Modifying book with ISBN: " << old_book.isbn_.a << std::endl;
  my_string new_isbn = my_string(), new_title = my_string(),
            new_author = my_string(), new_keyword = my_string();
  new_isbn.a[0] = new_title.a[0] = new_author.a[0] = new_keyword.a[0] = '\n';
  double new_price = -1;
  while (!token.empty()) {
    my_string ISBN = GetISBNForSearch(token);
    if (ISBN.a[0] != '\n') {
      // std::cerr << "Found new ISBN: " << ISBN.a << std::endl;
      if (new_isbn.a[0] != '\n') {
        printf("Invalid\n");
        return;
      }
      new_isbn = ISBN;
      token = current_command_.next_token();
      continue;
    }
    my_string book_name = GetBookNameForSearch(token);
    if (book_name.a[0] != '\n') {
      if (new_title.a[0] != '\n') {
        printf("Invalid\n");
        return;
      }
      new_title = book_name;
      token = current_command_.next_token();
      continue;
    }
    my_string author = GetAuthorForSearch(token);
    if (author.a[0] != '\n') {
      // std::cerr << "Found new author: " << author.a << std::endl;
      if (new_author.a[0] != '\n') {
        printf("Invalid\n");
        return;
      }
      new_author = author;
      token = current_command_.next_token();
      continue;
    }
    my_string keyword = GetKeywordForSearch(token);
    if (keyword.a[0] != '\n') {
      // std::cerr << "Found new keyword: " << keyword.a << std::endl;
      if (new_keyword.a[0] != '\n') {
        printf("Invalid\n");
        return;
      }
      new_keyword = keyword;
      token = current_command_.next_token();
      continue;
    }
    double price = GetPriceForSearch(token);
    if (price >= 0) {
      // std::cerr << "Found new price: " << price << std::endl;
      if (new_price >= 0) {
        printf("Invalid\n");
        return;
      }
      new_price = price;
      token = current_command_.next_token();
      continue;
    }
    // std::cerr << "Invalid modification token: " << token << std::endl;
    printf("Invalid\n");
    return;
  }

  if (!books_.GetBooksByIsbn(new_isbn).empty()) {
    // std::cerr << "ISBN already exists: " << new_isbn.a << std::endl;
    printf("Invalid\n");
    return;
  }

  Book new_book = old_book;
  if (new_title.a[0] != '\n') {
    books_.ModifyTitle(old_book.isbn_, new_title);
    new_book.title_ = new_title;
  }
  if (new_author.a[0] != '\n') {
    books_.ModifyAuthor(old_book.isbn_, new_author);
    new_book.author_ = new_author;
  }
  if (new_keyword.a[0] != '\n') {
    books_.ModifyKeyword(old_book.isbn_, new_keyword);
    new_book.keyword_ = new_keyword;
  }
  if (new_price >= 0) {
    books_.ModifyPrice(old_book.isbn_, new_price);
    new_book.price_ = new_price;
  }
  if (new_isbn.a[0] != '\n') {
    books_.ModifyIsbn(old_book.isbn_, new_isbn);
    new_book.isbn_ = new_isbn;
  }
  current_selected_book_ = new_book;

  ModifyLog log;
  log.time_ = ++time;
  log.admin_account_ = current_user_;
  log.old_book_ = old_book;
  log.new_book_ = new_book;
  LogEntry log_entry{};
  log_entry.type_ = LogEntry::MODIFY;
  log_entry.modify_log_ = log;
  logs_.AddLog(log_entry);
}

void Bookstore::Import() {
  if (current_user_.getPrivilege() < 3 || current_selected_book_.id_ == -1) {
    printf("Invalid\n");
    return;
  }

  std::string token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  int quantity = GetQuantity(token);
  if (quantity <= 0) {
    printf("Invalid\n");
    return;
  }
  token = current_command_.next_token();
  if (token.empty()) {
    printf("Invalid\n");
    return;
  }
  double total_cost = GetPrice(token);
  if (total_cost <= 0) {
    printf("Invalid\n");
    return;
  }

  if (!current_command_.next_token().empty()) {
    printf("Invalid\n");
    return;
  }

  books_.ModifyQuantity(current_selected_book_.isbn_, current_selected_book_.quantity_ + quantity);
  current_selected_book_.quantity_ += quantity;

  ImportLog log;
  log.time_ = ++time;
  log.admin_account_ = current_user_;
  log.book_ = current_selected_book_;
  log.quantity_ = quantity;
  log.total_cost_ = total_cost;
  LogEntry log_entry{};
  log_entry.type_ = LogEntry::IMPORT;
  log_entry.import_log_ = log;
  logs_.AddLog(log_entry);
}

void Bookstore::Report() {
  // TODO
}

void Bookstore::Log() {
  // TODO
}