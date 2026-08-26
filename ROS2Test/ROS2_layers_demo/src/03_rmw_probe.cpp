// ============================================================
// 第 3 层演示：rmw —— 中间件抽象层
// 这份代码从 rclcpp 一路往下"问"到 rmw：底层到底是谁在干活。
// 感受点：代码完全不用改，只改环境变量 RMW_IMPLEMENTATION，
//         底层中间件就换成另一个 DDS 实现（Fast DDS <-> Cyclone DDS）。
// 这就是 rmw 层存在的意义：屏蔽不同 DDS 的差异。
// ============================================================

#include "rclcpp/rclcpp.hpp"
#include "rmw/rmw.h"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("rmw_probe");

  RCLCPP_INFO(node->get_logger(), "rmw 实现:      %s", rmw_get_implementation_identifier());
  RCLCPP_INFO(node->get_logger(), "序列化格式:    %s", rmw_get_serialization_format());
  RCLCPP_INFO(node->get_logger(), "我可以从 rclcpp 直接调用 rmw 的函数，证明它们相邻");

  rclcpp::shutdown();
  return 0;
}
