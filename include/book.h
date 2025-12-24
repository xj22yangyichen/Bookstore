#ifndef BOOK_H
#define BOOK_H

#include <utility>
#include <algorithm>
#include "block_list.h"

// id 存的是这本书在 MemoryRiver 里的位置
class Book {
public:
  my_string title_, author_, isbn_, keyword_;
  int id_, quantity_;
  double price_;

  void Print() const {
    printf("%s\t%s\t%s\t%s\t%.2f\t%d\n", isbn_.a, title_.a, author_.a, keyword_.a, price_, quantity_);
  }

  Book() : price_(-1), id_(-1), quantity_(0) {
    memset(title_.a, 0, sizeof(title_.a));
    memset(author_.a, 0, sizeof(author_.a));
    memset(isbn_.a, 0, sizeof(isbn_.a));
    memset(keyword_.a, 0, sizeof(keyword_.a));
  }
  Book(const int id, const my_string isbn) : id_(id), isbn_(isbn), price_(0), quantity_(0) {
    memset(title_.a, 0, sizeof(title_.a));
    memset(author_.a, 0, sizeof(author_.a));
    memset(keyword_.a, 0, sizeof(keyword_.a));
  }

  bool operator<(const Book &other) const {
    return isbn_ < other.isbn_;
  }
};

std::vector<my_string> parse_keywords(const my_string &keywords);

class Books {
private:
  // 存储书籍数据的 MemoryRiver
  MemoryRiver<Book> data_;
  BlockList<my_string, int> isbn_index_;
  BlockList<my_string, int> title_index_;
  BlockList<my_string, int> author_index_;
  BlockList<my_string, int> keyword_index_;
public:
  Books() : isbn_index_("isbn"), title_index_("title"), author_index_("author"), keyword_index_("keyword") {
    data_.initialise("book_data.dat");
  }

  int size() {
    return data_.size();
  }

  void add(Book &book);
  void ModifyIsbn(const my_string isbn, const my_string &new_isbn);
  void ModifyTitle(const my_string isbn, const my_string &new_title);
  void ModifyAuthor(const my_string isbn, const my_string &new_author);
  void ModifyKeyword(const my_string isbn, const my_string &new_keyword);
  void ModifyPrice(const my_string isbn, const double new_price);
  void ModifyQuantity(const my_string isbn, const int new_quantity);

  Book GetBooksById(const int id);
  std::vector<Book> GetBooksByIsbn(const my_string &isbn);
  std::vector<Book> GetBooksByTitle(const my_string &title);
  std::vector<Book> GetBooksByAuthor(const my_string &author);
  std::vector<Book> GetBooksByKeyword(const my_string &keyword);
  std::vector<Book> GetAllBooks();

  int GetBooksCount() {
    return isbn_index_.size();
  }
};

#endif // BOOK_H