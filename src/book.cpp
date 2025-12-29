#include "book.h"

std::vector<my_string> parse_keywords(const my_string &keywords) {
  std::vector<my_string> result;
  my_string keyword;
  int pos = 0;
  for (int i = 0; i < strlen(keywords.a); ++i) {
    if (keywords.a[i] == '|') {
      memset(keyword.a, 0, sizeof(keyword.a));
      for (int j = pos; j < i; ++j) {
        keyword.a[j - pos] = keywords.a[j];
      }
      result.push_back(keyword);
      pos = i + 1;
    } else if (i == strlen(keywords.a) - 1) {
      memset(keyword.a, 0, sizeof(keyword.a));
      for (int j = pos; j <= i; ++j) {
        keyword.a[j - pos] = keywords.a[j];
      }
      result.push_back(keyword);
    }
  }
  return result;
}

void Books::add(Book &book) {
  data_.write(book);
  isbn_index_.insert(std::make_pair(book.isbn_, book.id_));
  title_index_.insert(std::make_pair(book.title_, book.id_));
  author_index_.insert(std::make_pair(book.author_, book.id_));
  std::vector<my_string> keywords = parse_keywords(book.keyword_);
  for (const auto &keyword : keywords) {
    keyword_index_.insert(std::make_pair(keyword, book.id_));
  }
}

void Books::ModifyIsbn(const my_string isbn, const my_string &new_isbn) {
  std::vector<int> ids = isbn_index_.get(isbn);
  if (ids.size() == 0 || ids.size() > 1) {
    throw std::runtime_error("ModifyIsbn error: ISBN not found or duplicate ISBNs exist.");
  }
  Book book;
  data_.read_by_order(book, ids[0]);
  isbn_index_.remove(std::make_pair(isbn, book.id_));
  book.isbn_ = new_isbn;
  data_.update_by_order(book, book.id_);
  isbn_index_.insert(std::make_pair(new_isbn, book.id_));
}

void Books::ModifyTitle(const my_string isbn, const my_string &new_title) {
  std::vector<int> ids = isbn_index_.get(isbn);
  if (ids.size() == 0 || ids.size() > 1) {
    throw std::runtime_error("ModifyTitle error: ISBN not found or duplicate ISBNs exist.");
  }
  Book book;
  data_.read_by_order(book, ids[0]);
  title_index_.remove(std::make_pair(book.title_, book.id_));
  book.title_ = new_title;
  data_.update_by_order(book, book.id_);
  title_index_.insert(std::make_pair(new_title, book.id_));
}

void Books::ModifyAuthor(const my_string isbn, const my_string &new_author) {
  std::vector<int> ids = isbn_index_.get(isbn);
  if (ids.size() == 0 || ids.size() > 1) {
    throw std::runtime_error("ModifyAuthor error: ISBN not found or duplicate ISBNs exist.");
  }
  Book book;
  data_.read_by_order(book, ids[0]);
  author_index_.remove(std::make_pair(book.author_, book.id_));
  book.author_ = new_author;
  data_.update_by_order(book, book.id_);
  author_index_.insert(std::make_pair(new_author, book.id_));
}

void Books::ModifyKeyword(const my_string isbn, const my_string &new_keyword) {
  std::vector<int> ids = isbn_index_.get(isbn);
  if (ids.size() == 0 || ids.size() > 1) {
    throw std::runtime_error("ModifyKeyword error: ISBN not found or duplicate ISBNs exist.");
  }
  Book book;
  data_.read_by_order(book, ids[0]);
  std::vector<my_string> old_keywords = parse_keywords(book.keyword_);
  for (const auto &old_keyword : old_keywords) {
    keyword_index_.remove(std::make_pair(old_keyword, book.id_));
  }
  book.keyword_ = new_keyword;
  data_.update_by_order(book, book.id_);
  std::vector<my_string> new_keywords = parse_keywords(new_keyword);
  for (const auto &keyword : new_keywords) {
    keyword_index_.insert(std::make_pair(keyword, book.id_));
  }
}

void Books::ModifyPrice(const my_string isbn, const double new_price) {
  std::vector<int> ids = isbn_index_.get(isbn);
  if (ids.size() == 0 || ids.size() > 1) {
    throw std::runtime_error("ModifyPrice error: ISBN not found or duplicate ISBNs exist.");
  }
  Book book;
  data_.read_by_order(book, ids[0]);
  book.price_ = new_price;
  data_.update_by_order(book, book.id_);
}

void Books::ModifyQuantity(const my_string isbn, const int new_quantity) {
  std::vector<int> ids = isbn_index_.get(isbn);
  if (ids.size() == 0 || ids.size() > 1) {
    throw std::runtime_error("ModifyQuantity error: ISBN not found or duplicate ISBNs exist.");
  }
  Book book;
  data_.read_by_order(book, ids[0]);
  book.quantity_ = new_quantity;
  data_.update_by_order(book, book.id_);
}

Book Books::GetBooksById(const int id) {
  if (id < 0) {
    throw std::invalid_argument("Invalid book ID");
  }
  if (id >= data_.size()) {
    throw std::out_of_range("Book ID out of range");
  }
  Book book;
  data_.read_by_order(book, id);
  return book;
}

std::vector<Book> Books::GetBooksByIsbn(const my_string &isbn) {
  std::vector<int> ids = isbn_index_.get(isbn);
  std::vector<Book> books = {};
  for (const auto &id : ids) {
    Book book;
    data_.read_by_order(book, id);
    books.push_back(book);
  }
  std::sort(books.begin(), books.end());
  return books;
}

std::vector<Book> Books::GetBooksByTitle(const my_string &title) {
  std::vector<int> ids = title_index_.get(title);
  std::vector<Book> books = {};
  for (const auto &id : ids) {
    Book book;
    data_.read_by_order(book, id);
    books.push_back(book);
  }
  std::sort(books.begin(), books.end());
  return books;
}

std::vector<Book> Books::GetBooksByAuthor(const my_string &author) {
  std::vector<int> ids = author_index_.get(author);
  std::vector<Book> books = {};
  for (const auto &id : ids) {
    Book book;
    data_.read_by_order(book, id);
    books.push_back(book);
  }
  std::sort(books.begin(), books.end());
  return books;
}

std::vector<Book> Books::GetBooksByKeyword(const my_string &keyword) {
  std::vector<int> ids = keyword_index_.get(keyword);
  std::vector<Book> books = {};
  for (const auto &id : ids) {
    Book book;
    data_.read_by_order(book, id);
    books.push_back(book);
  }
  std::sort(books.begin(), books.end());
  return books;
}

std::vector<Book> Books::GetAllBooks() {
  std::vector<int> ids = isbn_index_.get_all();
  std::vector<Book> books = {};
  for (const auto &id : ids) {
    Book book;
    data_.read_by_order(book, id);
    books.push_back(book);
  }
  std::sort(books.begin(), books.end());
  return books;
}