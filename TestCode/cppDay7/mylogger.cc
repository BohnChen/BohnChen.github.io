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
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
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
