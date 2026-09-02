#include "Mylogger.h"

int main() {
  // 不用 getInstance() 也没关系，宏内部会自动拿单例
  LOG_INFO("程序启动");
  LOG_WARN("磁盘剩余 %d MB", 32);
  LOG_ERROR("连接 %s 失败", "db");
  LOG_DEBUG("x = %d, y = %.2f", 1, 3.14);

  Mylogger *log = Mylogger::getInstance(); // 作业里的写法
                                           //
  log->info("手动调用 info");
  log4cpp::Category::shutdown();
  return 0;
}
