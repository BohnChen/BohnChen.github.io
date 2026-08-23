---
title: "log4cpp"
date: 2026-08-23T13:05:49+08:00
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习", "开源项目"]
---

这个仓库中，包含了`cpp`可能用到的各种库，我们今天要用到的是`log4cpp`。

[awesome-cpp 中文开源项目](https://github.com/jobbole/awesome-cpp-cn)

[awesome-cpp 原版开源项目](https://github.com/fffaraz/awesome-cpp)


如果你要详细了解，可以到[log4cpp 官网](https://log4cpp.sourceforge.net/)查看详情。

## 环境与编译

本机通过 `brew` 安装了 log4cpp 1.1.6，头文件在 `/opt/homebrew/include`，库在 `/opt/homebrew/lib`。`~/.zshrc` 中已配置：

```zsh
export CPATH="/opt/homebrew/include:$CPATH"            # 头文件搜索路径
export LIBRARY_PATH="/opt/homebrew/lib:$LIBRARY_PATH"  # 库搜索路径
log4() { ... }                                          # 一键编译并运行
```

所以编译任意用到 log4cpp 的 `.cc` 文件：

```bash
log4 test.cc                      # 一键编译 + 运行（已带 -llog4cpp）
# 或
g++ test.cc -llog4cpp && ./a.out  # 需要头文件/库路径环境变量
```

## 一、整体认知：四个核心概念

log4cpp 仿照 Java 的 log4j，把日志拆成四个概念，**各管一件事**：

| 概念 | 管什么 | 常用类 |
|---|---|---|
| **Category** | 日志的"入口 / 分类"，你用它写日志 | `Category::getRoot()`、`Category::getInstance("名字")` |
| **Appender** | 日志**写到哪** | 终端 `OstreamAppender`、文件 `FileAppender` |
| **Layout** | 每条日志**长什么样**（格式） | `PatternLayout`、`SimpleLayout` |
| **Priority** | 哪些级别要输出（过滤） | `Priority::DEBUG/INFO/WARN/ERROR`… |

数据流：

```
调用 category.info(...)
        │
        ▼
category 挂着的 Appender（决定写到哪）
        │
        ▼
Appender 用 Layout 格式化（决定长什么样）
        │
        ▼
输出到 终端 / 文件
```

> 想输出到 cout 还是文件，只需换不同的 **Appender**，其他都不用动。

## 二、例 1：输出到终端（OstreamAppender）

```cpp
#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>

int main() {
    // 1. 建 Layout（决定格式）：%d 时间 %p 级别 %c 分类 %m 消息 %n 换行
    log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
    layout->setConversionPattern("%d %p %c: %m%n");

    // 2. 建 Appender（决定写到哪）：这里写到 std::cout
    log4cpp::OstreamAppender* console =
        new log4cpp::OstreamAppender("console", &std::cout);
    console->setLayout(layout);

    // 3. 拿 Category 并挂上 Appender
    log4cpp::Category& root = log4cpp::Category::getRoot();
    root.setPriority(log4cpp::Priority::DEBUG);  // 门槛放最低，全部输出
    root.addAppender(console);

    // 4. 写日志
    root.info("程序启动");
    root.warn("磁盘快满了，剩余 %d MB", 32);   // printf 风格变参
    root.error("出错了");
    return 0;
}
```

输出：

```
2026-08-23 12:00:00 INFO root: 程序启动
2026-08-23 12:00:00 WARN root: 磁盘快满了，剩余 32 MB
2026-08-23 12:00:00 ERROR root: 出错了
```

要点：

- `root.info(...)`、`warn`、`error` 等便捷方法都支持 **printf 变参**。
- 也支持流式写法：`root << log4cpp::Priority::INFO << "消息 " << 42;`
- `setLayout(layout)` 后 layout 归 appender 管理，不需要手动 `delete`。

## 三、例 2：输出到文件（FileAppender）

只换 Appender，其余不变：

```cpp
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>

int main() {
    log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
    layout->setConversionPattern("%d %p %c: %m%n");

    log4cpp::FileAppender* file = new log4cpp::FileAppender("file", "run.log");
    file->setLayout(layout);

    log4cpp::Category& root = log4cpp::Category::getRoot();
    root.setPriority(log4cpp::Priority::DEBUG);
    root.addAppender(file);

    root.info("写入文件成功");
    return 0;
}
```

- `FileAppender("名字", "文件名")`：第二个参数是文件路径，自动创建。
- 想按大小滚动 / 按天切分日志，用 `RollingFileAppender` / `DailyRollingFileAppender`，用法类似。

## 四、例 3：Priority 怎么过滤（级别表）

级别本质是整数，**数值越小越严重**：

```
FATAL=EMERG=0 < ALERT=100 < CRIT=200 < ERROR=300 < WARN=400 < NOTICE=500 < INFO=600 < DEBUG=700 < NOTSET=800
```

过滤规则：**只有"消息级别 ≤ 门槛"才输出**。

```cpp
root.setPriority(log4cpp::Priority::DEBUG);  // 门槛 700：≤700 全输出
root.setPriority(log4cpp::Priority::WARN);   // 门槛 400：只输出 WARN/ERROR 及更严重
//                                  ↓ 下面两行会被过滤掉
root.info("细节");   // 600 > 400 → 不输出
root.debug("调试");  // 700 > 400 → 不输出
```

实用习惯：

- 开发期：`setPriority(DEBUG)`，全看。
- 上线后：`setPriority(WARN)` 或 `ERROR`，只看警告和错误。

## 五、例 4：root 与 getInstance —— 分类树

### Category 是一棵"树"，root 是树根

log4cpp 里所有 Category 构成一棵树。**root 是唯一天然存在、不需要创建的根节点**，用 `getRoot()` 拿到它；其他分类都是它的后代。

### getInstance(name) 做什么

在树里**按名字查找分类，找不到就创建一个**，返回引用。名字支持点分，形成层次：

```
root
├── "db"          ← getInstance("db")
│   └── "db.sql"  ← getInstance("db.sql")，是 db 的子节点
└── "net"         ← getInstance("net")
```

### 为什么必须有 root

1. **root 是默认兜底**：即使一个分类都不建，`getRoot()` 也能随时写日志，这是 log4j/log4cpp 设计保证的"始终可用"入口。
2. **子分类继承父级**：`db`、`net` 没设置 priority / appender 时，自动**继承 root 的**。所以先在 root 上挂好 appender、设好级别，所有子分类默认就能用。

### 例 4 完整代码

```cpp
#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>

int main() {
    log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
    layout->setConversionPattern("%d %p %c: %m%n");

    log4cpp::OstreamAppender* console =
        new log4cpp::OstreamAppender("console", &std::cout);
    console->setLayout(layout);

    // root 挂 appender + 设级别 → 所有子分类默认继承
    log4cpp::Category& root = log4cpp::Category::getRoot();
    root.setPriority(log4cpp::Priority::DEBUG);
    root.addAppender(console);

    // 命名分类
    log4cpp::Category& db  = log4cpp::Category::getInstance("db");
    log4cpp::Category& net = log4cpp::Category::getInstance("net");

    root.info("根分类，%c 显示 root");
    db.info("数据库连接建立");          // %c 显示 db，继承 root 的输出
    net.warn("网络超时");              // %c 显示 net

    // 单独调子分类的级别，不影响别人
    net.setPriority(log4cpp::Priority::ERROR);  // 之后 net 只输出 ERROR
    net.info("这行不输出，被 net 自己的门槛挡了");
    net.error("但这行输出");
    return 0;
}
```

输出：

```
... INFO root: 根分类，%c 显示 root
... INFO db: 数据库连接建立
... WARN net: 网络超时
... ERROR net: 但这行输出
```

> `getInstance` 的价值：**不同模块用不同分类，可分别设级别和输出，互不干扰**；root 负责兜底和默认行为。

## 六、例 5：官方 Simple Example（同时输出终端 + 文件）

一个 Category 可以挂**多个 Appender**，一条日志同时走多处。官方例子（来源：[log4cpp 官网 Simple example](https://log4cpp.sourceforge.net/#simpleexample)）：

```cpp
// main.cpp
#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"

int main(int argc, char** argv) {
    log4cpp::Appender* appender1 = new log4cpp::OstreamAppender("console", &std::cout);
    appender1->setLayout(new log4cpp::BasicLayout());

    log4cpp::Appender* appender2 = new log4cpp::FileAppender("default", "program.log");
    appender2->setLayout(new log4cpp::BasicLayout());

    log4cpp::Category& root = log4cpp::Category::getRoot();
    root.setPriority(log4cpp::Priority::WARN);
    root.addAppender(appender1);

    log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("sub1"));
    sub1.addAppender(appender2);

    // 函数式写日志
    root.error("root error");
    root.info("root info");
    sub1.error("sub1 error");
    sub1.warn("sub1 warn");

    // printf 风格写变量
    root.warn("%d + %d == %s ?", 1, 1, "two");

    // 流式写日志
    root << log4cpp::Priority::ERROR << "Streamed root error";
    root << log4cpp::Priority::INFO << "Streamed root info";
    sub1 << log4cpp::Priority::ERROR << "Streamed sub1 error";
    sub1 << log4cpp::Priority::WARN << "Streamed sub1 warn";

    // 或这样：
    root.errorStream() << "Another streamed error";

    return 0;
}
```

控制台输出（注意 root 门槛是 WARN，所以 root 的 INFO 被过滤；sub1 继承 root 的 WARN 门槛，同时写入 program.log）：

```
1352973121 ERROR  : root error
1352973121 ERROR sub1 : sub1 error
1352973121 WARN sub1 : sub1 warn
1352973121 WARN  : 1 + 1 == two ?
1352973121 ERROR  : Streamed root error
1352973121 ERROR sub1 : Streamed sub1 error
1352973121 WARN sub1 : Streamed sub1 warn
1352973121 ERROR  : Another streamed error
```

## 七、PatternLayout 常用格式化字符

| 字符 | 含义 |
|---|---|
| `%c` | 分类名（category） |
| `%d` | 日期，可跟格式，如 `%d{%H:%M:%S,%l}`（`%l` 为毫秒） |
| `%m` | 消息内容 |
| `%n` | 平台换行符 |
| `%p` | 优先级（级别名） |
| `%%` | 一个百分号 |

## 八、小结

| 问题 | 答案 |
|---|---|
| 输出到 cout 还是文件 | 换 **Appender**：`OstreamAppender`(cout) / `FileAppender`(文件)，其余不变 |
| 为什么有 root | Category 是树，root 是根节点、天然存在、兜底入口，子分类默认继承它 |
| getInstance 干嘛 | 按名字查找/创建分类，支持 `a.b.c` 层次，实现"按模块分别控制日志" |
| 什么决定日志去哪 | Appender（到哪去）+ Layout（什么格式）+ Priority（要不要输出） |

补充几个关键点：

- **线程安全**：同一个 Category 可被多线程同时使用，内部有锁，日志不会交错；但多个分类共用**同一个 Appender 实例**不推荐，可能交错。
- **宏方式**：log4cpp 还提供 `LOG4CPP_DEBUG(logger, "...")`、`LOG4CPP_INFO_S(logger, ...)` 等宏，级别不满足时参数**不会求值**，可省开销。
- **所有权**：`addAppender` 之后 Category 拥有该 Appender 的所有权（会自动释放），layout 归 appender 管理，所以用 `new` 创建后不要自己 `delete`。

**作业**

1、熟悉log4cpp的用法，动手敲代码 
http://blog.csdn.net/liuhong135541/article/category/1496383
```c++
#include <log4cpp/Appender.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/LayoutAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>

void test() {
  log4cpp::PatternLayout *MyLayout = new log4cpp::PatternLayout();
  MyLayout->setConversionPattern("%d %p %c: %m%n");

  log4cpp::Appender *TestAppender_1 =
      new log4cpp::OstreamAppender("TestConsole", &std::cout);
  TestAppender_1->setLayout(MyLayout);

  log4cpp::Appender *TestAppender_2 =
      new log4cpp::FileAppender("defautl", "MyTestProgramm.log");
  TestAppender_2->setLayout(MyLayout);

  log4cpp::Category &root = log4cpp::Category::getRoot();
  log4cpp::Category &net = log4cpp::Category::getInstance("mynet");

  root.setPriority(log4cpp::Priority::DEBUG);
  net.setPriority(log4cpp::Priority::WARN);

  root.addAppender(TestAppender_1);
  net.addAppender(TestAppender_2);

  root.error("This is a root error.");
  root.info("终端已输出；写入文件成功！");
  root.emerg("终端应该输出 emerg 信息");
  root.debug("门槛最低，是debug 700, 所有信息都应该从终端输出");

  net.info("这句是 info 600 不应该输出");
  net.emerg("这句是最高优先级 emerg 0 , 应该输出到文件");
  net.error("这句是error < WARN 400, 应该输出");
  net.warn("这句是warn == 400, 应该输出");
  std::cout << "EMERG: " << log4cpp::Priority::EMERG << std::endl;
  std::cout << "ERROR: " << log4cpp::Priority::ERROR << std::endl;
}

int main(int argc, char *argv[]) {
  // std::cout << "hello world!" << std::endl;
  test();
  return 0;
}
 

```



2、封装log4cpp，使用起来比较方便，如下所示。输出的日志信息能同时输出到终端和文件 
要求：可以像使用printf一样，进行可变参数的传递，可以传任意个参数。

```C++
int main(void)
{
	cout << "hello,world" << endl;
	

	logInfo("The log is info message");	//或者   
	logError("The log is error message");
	logWarn("The log is warn message");
	logDebug("The log is debug message");

}

加分项：输出的日志信息中最好能有文件的名字，函数的名字及其所在的行号(这个在C/C++里面有对应的宏，可以查一下)

//使用单例模式
class Mylogger
{
public:
	void warn(const char *msg);
	void error(const char *msg);
	void debug(const char *msg);
	void info(const char *msg);
	
private:
	Mylogger();
	~Mylogger();
    
private:
  //......
};

Mylogger *log = Mylogger::getInstance();

log->info("The log is info message");
log->error("The log is error message");
log->fatal("The log is fatal message");
log->crit("The log is crit message");

```

提示：在记录日志的时候，可以把文件名、函数名、行号与最终需要打印的信息拼接起来，这其实就是字符串的拼接，可以使用string进行操作。但是每次使用log->info，log->error的时候，都要使用对象，这样的使用也是比较麻烦，所以可以吧log->info("The log is info message");一些信息再去进行封装即可，例如使用宏定义。

```c++

#include "Mylogger.h"
#include <cstdarg>
#include <cstdio>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>

Mylogger::Mylogger() : _cat(log4cpp::Category::getRoot()) { init(); }

Mylogger::~Mylogger() {}

Mylogger *Mylogger::getInstance() {
  static Mylogger instance;
  return &instance;
}

void Mylogger::init() {
  // 两个 appender 各用独立的 layout，避免共享导致双重释放
  log4cpp::PatternLayout *layout1 = new log4cpp::PatternLayout();
  layout1->setConversionPattern("%d %p %c: %m%n");
  log4cpp::Appender *app1 = new log4cpp::OstreamAppender("console", &std::cout);
  app1->setLayout(layout1);

  log4cpp::PatternLayout *layout2 = new log4cpp::PatternLayout();
  layout2->setConversionPattern("%d %p %c: %m%n");
  log4cpp::Appender *app2 = new log4cpp::FileAppender("default", "testlog.log");
  app2->setLayout(layout2);

  _cat.setPriority(log4cpp::Priority::DEBUG);
  _cat.addAppender(app1);
  _cat.addAppender(app2);
}

// 四个级别方法：只做"格式化 + 写日志"，不再做任何初始化
void Mylogger::info(const char *fmt, ...) {
  char buf[2048];
  va_list args;

  // args 作为游标指向第一个固定参数 fmt
  va_start(args, fmt);                      

  // 用数量控制安全；args 仍为游标一个一个的取出%d的运算符并配对，将配对好的内容输出到buf中
  vsnprintf(buf, sizeof(buf), fmt, args);   
  
  // 最后告诉编译器取完了
  va_end(args);

  _cat.info(buf);
}

void Mylogger::warn(const char *fmt, ...) {
  char buf[2048];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  _cat.warn(buf);
}

void Mylogger::error(const char *fmt, ...) {
  char buf[2048];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  _cat.error(buf);
}

void Mylogger::debug(const char *fmt, ...) {
  char buf[2048];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  _cat.debug(buf);
}



```



**附：可变参数各种写法对比**（示例代码见同目录 `variablePara1.cc` ~ `variablePara4.cc`）

| 写法 | 示例文件 | 代码形态 | 特点 | 适用场景 |
|---|---|---|---|---|
| ① C 风格变参（stdarg） | `variablePara1.cc` | `void f(const char* fmt, ...)` + `va_start/va_arg/va_end` | 最接近 printf，支持 `%d/%s` 格式串；**类型不检查**，个数靠约定 | 模仿 printf / 封装 log4cpp（`vsnprintf` 格式化） |
| ② 变参宏（`__VA_ARGS__`） | `variablePara2.cc` | `#define LOG(fmt, ...) ... ##__VA_ARGS__` | 能在调用点展开 `__FILE__`/`__LINE__`/`__FUNCTION__`；配合函数转发 | 注入位置信息（作业加分项） |
| ③ C++11 变参模板 | `variablePara3.cc` | `template<typename... Args> void f(Args... a)` | **类型安全**，编译期检查；递归展开；不支持 printf 格式串 | 直接拼接任意类型 |
| ④ C++17 折叠表达式 | `variablePara4.cc` | `((expr), ...)` | 一行替代递归，类型安全 | 简化③ |

编译运行方式：`g++ -std=c++17 variableParaX.cc -o variableParaX && ./variableParaX`

关键点：
- **①负责"按格式串取参数"**（`va_start`/`vsnprintf`/`va_end`）；**②负责"在调用点塞位置信息"**（必须是宏，函数内部展开 `__FILE__` 会指向库文件）；**③④类型安全但无格式串**。
- 作业的 `Mylogger` 封装 = **①（vsnprintf 格式化）+ ②（宏注入 `__FILE__`/`__LINE__`/`__FUNCTION__`）**。
- 宏里位置参数必须排在用户参数之前（`... ##__VA_ARGS__`），否则 `%s` 会拿到错误实参导致段错误。





