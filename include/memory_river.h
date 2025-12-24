#ifndef MEMORY_RIVER_H
#define MEMORY_RIVER_H

#include <string>
#include <fstream>
#include <filesystem>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T, int info_len = 2>
class MemoryRiver {
private:
  fstream file;
  string file_name;
  int sizeofT = sizeof(T);
  int size_ = 0;
public:
  MemoryRiver() = default;

  MemoryRiver(const string& file_name) : file_name(file_name) {}

  ~MemoryRiver() {
    write_info(size_, 0);
    if (file.is_open()) {
      file.close();
    }
  }

  int size() {
    return size_;
  }

  void initialise(string FN = "") {
    // std::cerr << "Initializing MemoryRiver with file: " << (FN == "" ? file_name : FN) << std::endl;
    if (FN != "") file_name = FN;
    if (!std::filesystem::exists(file_name)) {
      // std::cerr << "File does not exist. Creating new file: " << file_name << std::endl;
      std::ofstream create_file(file_name, std::ios::binary);
      if (!create_file.is_open()) {
        throw std::runtime_error("Failed to create file: " + file_name);
      }
      int tmp = 0;
      for (int i = 0; i < info_len; ++i)
        create_file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
      create_file.close();
    }
    get_info(size_, 0);
    file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
  }

  // 读出第n个int的值赋给tmp，0_base
  void get_info(int &tmp, int n) {
    if (n >= info_len) return;
    file.seekg(n * sizeof(int), std::ios::beg);
    file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
  }

  // 将tmp写入第n个int的位置，0_base
  void write_info(int tmp, int n) {
    if (n >= info_len) return;
    file.seekp(n * sizeof(int), std::ios::beg);
    file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
  }

  // 在文件合适位置写入类对象t，并返回写入的位置索引index
  // 位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
  // 位置索引index可以取为对象写入的起始位置
  int write(T &t) {
    file.seekp(0, std::ios::end);
    int index = file.tellp();
    file.write(reinterpret_cast<char *>(&t), sizeofT);
    ++size_;
    return index;
  }

  // 用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
  void update(T &t, const int index) {
    file.seekp(index, std::ios::beg);
    file.write(reinterpret_cast<char *>(&t), sizeofT);
  }

  void update_by_order(T &t, const int order) {
    int index = info_len * sizeof(int) + order * sizeofT;
    file.seekp(index, std::ios::beg);
    file.write(reinterpret_cast<char *>(&t), sizeofT);
  }

  // 读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
  void read(T &t, const int index) {
    file.seekg(index, std::ios::beg);
    file.read(reinterpret_cast<char *>(&t), sizeofT);
  }

  void read_by_order(T &t, const int order) {
    int index = info_len * sizeof(int) + order * sizeofT;
    file.seekg(index, std::ios::beg);
    file.read(reinterpret_cast<char *>(&t), sizeofT);
  }

  // 删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
  void Delete(int index) {
  }
};

#endif // MEMORY_RIVER_H