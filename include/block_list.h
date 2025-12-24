#ifndef BLOCK_LIST_H
#define BLOCK_LIST_H

#include <filesystem>
#include <vector>
#include <utility>
#include <iostream>
#include "memory_river.h"
#include "mystring.h"

template<typename index_t, typename value_t>
class BlockList {
private:
  const static int BLOCK_SIZE = 500;
  struct Block {
    int size;
    std::pair<index_t, value_t> records[BLOCK_SIZE];
  };
  struct Index {
    std::pair<index_t, value_t> max_record;
    int block_id, next;
  };
  int head;
  // 存链表内块的索引和第一个数据
  MemoryRiver<Index> list_index;
  // 存链表内块的数据
  MemoryRiver<Block> data;
public:
  BlockList(std::string name) {
    bool first_run = !std::filesystem::exists(name + "_index.dat");
    list_index.initialise(name + "_index.dat");
    data.initialise(name + "_data.dat");
    if (!first_run) {
      list_index.get_info(head, 1);
    } else {
      head = -1;
    }
  }
  ~BlockList() {
    list_index.write_info(head, 1);
  }

  // 在链表中插入记录
  void insert(std::pair<index_t, value_t> record) {
    Index idx;
    Block target;
    // std::cerr << "Inserting record: " << record.first.a << std::endl;

    // 链表中还没有数据，新建一个节点
    if (head == -1) {
      // std::cerr << "Inserting first record.\n";
      target.size = 1;
      target.records[0] = record;
      int block_index = data.write(target);
      idx.max_record = record;
      idx.block_id = block_index;
      idx.next = -1;
      head = list_index.write(idx);
      // std::cerr << "First record inserted at head: " << head << std::endl;
      return;
    }
    
    // 找到要插入的位置
    int pos = head;
    list_index.read(idx, pos);
    while (idx.next != -1) {
      if (idx.max_record < record) {
        pos = idx.next;
        list_index.read(idx, pos);
      } else {
        break;
      }
    }

    // 读取目标块并插入数据
    data.read(target, idx.block_id);
    int insert_pos = target.size;
    for (int i = 0; i < target.size; ++i) {
      if (target.records[i] >= record) {
        insert_pos = i;
        break;
      }
    }
    for (int i = target.size; i > insert_pos; --i) {
      target.records[i] = target.records[i - 1];
    }
    target.records[insert_pos] = record;
    target.size++;
    idx.max_record = target.records[target.size - 1];

    // 如果块满了，裂块
    if (target.size >= BLOCK_SIZE) {
      Block new_block;
      new_block.size = 0;
      for (int i = BLOCK_SIZE / 2; i < target.size; ++i) {
        new_block.records[new_block.size++] = target.records[i];
      }
      target.size = BLOCK_SIZE / 2;
      int new_block_index = data.write(new_block);
      Index new_idx;
      new_idx.max_record = new_block.records[new_block.size - 1];
      new_idx.block_id = new_block_index;
      new_idx.next = idx.next;
      idx.next = list_index.write(new_idx);
      idx.max_record = target.records[target.size - 1];
    }
    data.update(target, idx.block_id);
    list_index.update(idx, pos);
  }

  // 从链表中删除记录。如果记录不存在，则不进行任何操作
  void remove(std::pair<index_t, value_t> record) {
    // 1. 找到包含该记录的块
    // 2. 删除记录
    Index idx;
    Block target;

    // 链表为空，直接返回
    if (head == -1) return;

    int pos = head;
    list_index.read(idx, pos);

    // 找到要删除的记录所在的块
    while (idx.next != -1) {
      if (idx.max_record < record) {
        pos = idx.next;
        list_index.read(idx, pos);
      } else {
        break;
      }
    }

    // 读取目标块并删除数据
    data.read(target, idx.block_id);
    int delete_pos = -1;
    for (int i = 0; i < target.size; ++i) {
      if (target.records[i] == record) {
        delete_pos = i;
        break;
      } else if (target.records[i] > record) {
        return;
      }
    }
    // 记录不存在，直接返回
    if (delete_pos == -1) return;
    for (int i = delete_pos; i < target.size - 1; ++i) {
      target.records[i] = target.records[i + 1];
    }
    target.size--;
    data.update(target, idx.block_id);

    // 如果块为空，删除该块
    if (target.size == 0) {
      if (pos == head) {
        head = idx.next;
      } else {
        Index prev_idx;
        int prev_pos = head;
        list_index.read(prev_idx, prev_pos);
        while (prev_idx.next != pos) {
          prev_pos = prev_idx.next;
          list_index.read(prev_idx, prev_pos);
        }
        prev_idx.next = idx.next;
        list_index.update(prev_idx, prev_pos);
      }
      list_index.Delete(pos);
      data.Delete(idx.block_id);
    } else {
      // 更新块的最大记录
      idx.max_record = target.records[target.size - 1];
      list_index.update(idx, pos);
    }
  };

  // 查找记录并返回其值
  std::vector<value_t> get(index_t index) {
    // std::cerr << "Getting record for index: " << index.a << std::endl;
    std::vector<value_t> result = {};
    Index idx;
    Block target;

    // 链表为空，直接返回空结果
    if (head == -1) {
      return result;
    }

    // std::cerr << "Starting search from head: " << head << std::endl;
    // 找到包含该记录的块
    int pos = head;
    list_index.read(idx, pos);
    while (idx.next != -1) {
      if (idx.max_record.first < index) {
        pos = idx.next;
        list_index.read(idx, pos);
      } else {
        break;
      }
    }

    // std::cerr << "Searching in block with max record: " << idx.max_record.first.a << std::endl;
    // 读取目标块并查找记录
    while (true) {
      data.read(target, idx.block_id);
      for (int i = 0; i < target.size; ++i) {
        if (target.records[i].first == index) {
          result.push_back(target.records[i].second);
        } else if (target.records[i].first > index) {
          return result;
        }
      }
      if (idx.next == -1) break;
      pos = idx.next;
      list_index.read(idx, pos);
    }
    return result;
  }

  std::vector<value_t> get_all() {
    std::vector<value_t> result;
    Index idx;
    Block target;

    // 链表为空，直接返回空结果
    if (head == -1) {
      return result;
    }

    // 遍历所有块并收集记录
    int pos = head;
    while (pos != -1) {
      list_index.read(idx, pos);
      data.read(target, idx.block_id);
      for (int i = 0; i < target.size; ++i) {
        result.push_back(target.records[i].second);
      }
      pos = idx.next;
    }
    return result;
  }

  int size() {
    int count = 0;
    Index idx;
    Block target;

    // 链表为空，直接返回0
    if (head == -1) {
      return 0;
    }

    // 遍历所有块并计数
    int pos = head;
    while (pos != -1) {
      list_index.read(idx, pos);
      data.read(target, idx.block_id);
      count += target.size;
      pos = idx.next;
    }
    return count;
  }
};

#endif // BLOCK_LIST_H