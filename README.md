# Bookstore Management System (书店管理系统)

## 📖 项目简介

这是一个基于命令行交互 (CLI) 的高性能书店管理系统。该项目旨在模拟高并发、大数据量场景下的图书管理业务。

与传统基于内存的系统不同，本系统严格遵循**“主体数据存储于外存”的性能要求。底层采用自行设计的带内存索引的块状链表 (Indexed Unrolled Linked List)** 结构，实现了在低内存占用下对海量数据的高效增删改查。

## ✨ 核心特性

### 业务功能

- 多级权限管理：支持游客、顾客、销售人员、店长四级权限体系。

- 嵌套登录机制：支持登录栈，允许用户在不退出的情况下切换账户执行操作。

- 混合查询：支持通过 ISBN、书名、作者、关键字（多关键字）组合检索图书。

- 日志与报表：提供交易流水记录、员工工作日志及财务报表生成功能。

### 技术亮点

- 持久化存储：所有数据实时写入磁盘，系统崩溃不丢失数据。

- 块状链表 (BlockList)：替代传统的 B+ 树，使用分块存储维持数据有序性。

- 内存索引优化：引入轻量级内存索引（Index Table），记录数据块的 MinKey 和偏移量，将磁盘 I/O 降低至常数级。

- 空间回收：支持文件空间的复用与管理（部分实现）。

## 🛠️ 构建与运行

### 环境依赖

操作系统: Linux (推荐 Ubuntu) 或 Windows (WSL)

编译器: g++ (需支持 C++17 或更高版本)

构建工具: CMake

### 编译步骤

1. 克隆仓库

```shell
git clone https://github.com/xj22yangyichen/Bookstore
cd Bookstore
```

2. 编译项目

```shell
mkdir build
cmake -B build
cmake --build build
```

### 运行方式

```
./build/code
```

程序首次运行时，会自动初始化文件存储结构，并创建默认超级管理员账户：

账号: `root`

密码: `sjtu`

## 📂 项目结构

```
.
├── CMakeLists.txt
├── README.md
├── docs
│   ├── 总体设计.md
│   └── 需求分析.md
├── include
│   ├── account.h
│   ├── block_list.h
│   ├── book.h
│   ├── bookstore.h
│   ├── log.h
│   ├── memory_river.h
│   ├── mystring.h
│   └── parser.h
└── src
    ├── book.cpp
    ├── bookstore.cpp
    ├── log.cpp
    ├── main.cpp
    └── parser.cpp
```

## 💻 命令手册

使用命令行（Windows CMD 或 Linux Terminal）进行操作，输入数据以换行符或回车符为分隔为若干指令。

指令合法字符集为标准 ASCII 字符；允许的空白符仅为空格，单个指令被空格分割为多个部分。多个连续空格效果与一个空格等价；行首行末允许出现多余空格；如无特殊说明禁止省略或另增空格。

指令中第一个部分必须为指令关键词，指令中关键词部分必须与指令格式完全匹配。

本文档中以弱化的正则语法说明指令文法。合法指令的文法相关说明如下：

- `[x]` 表示一串有特定限制的用户自定义字符串；
- `(a|b|c)` 表示此处仅能出现 `a`, `b`, `c` 中其一；
- `(x)?` 表示此处可以出现零次或一次 `x`；
- `(x)+` 表示此处可以出现一次或多次 `x`。

合法的指令如下所示：（`#` 开头的行是注释）

```
# 基础指令
quit
exit

# 帐户系统指令
su [UserID] ([Password])?
logout
register [UserID] [Password] [Username]
passwd [UserID] ([CurrentPassword])? [NewPassword]
useradd [UserID] [Password] [Privilege] [Username]
delete [UserID]

# 图书系统指令
show (-ISBN=[ISBN] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]")?
buy [ISBN] [Quantity]
select [ISBN]
modify (-ISBN=[ISBN] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]" | -price=[Price])+
import [Quantity] [TotalCost]

# 日志系统指令
show finance ([Count])?
log
report finance
report employee
```

在用户输入一条指令后，如果合法则执行对应操作，如果有则输出操作结果；如果指令非法或操作失败则输出 `Invalid\n`。仅有空格的指令合法且无输出内容。

除非有特殊说明，如果输入指令对应的输出内容非空，则结尾有 `\n` 字符；输出内容为空则不输出任何字符。

`quit` 和 `exit` 指令功能为正常退出系统。

## 👥 作者与致谢

Author: 杨奕辰

Email: yangyichen25@sjtu.edu.cn

Institution: 上海交通大学

感谢[程序设计](https://acm.sjtu.edu.cn/wiki/Programming_2025)课程翁惠玉老师与助教的指导。