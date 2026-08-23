#include <log4cpp/Appender.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/LayoutAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>

void warn(const char *str, ...) {
  char buf[2048];
  va_list args;
  va_start(args, str); // args作为游标对准第一个固定参数 str,
                       // 然后编译器直接从寄存器开始取可变参数，配对开始
  vsnprintf(
      buf, sizeof(buf), str,
      args);    // 逐个取出参数并输出，格式化到 buf，安全有上限2048，
                // args仍是游标，将可变参数str中的%d等控制符取出配对并输入到buf
  va_end(args); // 与va_start 必须配对使用，告诉编译器取完了。
  //
  log4cpp::PatternLayout *MyLayout = new log4cpp::PatternLayout();

  MyLayout->setConversionPattern("%d %p %c: %m%n");

  log4cpp::Appender *appender_1 =
      new log4cpp::OstreamAppender("console", &std::cout);
  log4cpp::Appender *appender_2 =
      new log4cpp::FileAppender("defautl", "testlog.log");

  // 不能用两个，delete时候会 double free
  appender_1->setLayout(MyLayout);
  appender_2->setLayout(MyLayout);

  log4cpp::Category &root = log4cpp::Category::getRoot();
  root.addAppender(appender_1);
  root.addAppender(appender_2);

  root.setPriority(log4cpp::Priority::DEBUG);
  root.warn(buf);
}

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
  // test();
  std::string str = "nice";
  warn("My name is %s", &str);
  return 0;
}
