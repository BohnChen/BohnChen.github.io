// ============================================================
// 第 1 层演示：rclcpp —— 你平时写的就是这一层
// 面向对象封装：Node / Publisher / Subscriber / Timer / 回调
// 你不用关心底层任何东西，对象创建、线程、清理全自动。
// ============================================================

#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class Talker : public rclcpp::Node
{
public:
  explicit Talker() : Node("talker")
  {
    // create_publisher 内部一路下沉：rclcpp -> rcl -> rmw -> DDS
    // 第 2 个参数 10 是 QoS 的 History 深度（队列大小）
    pub_ = this->create_publisher<std_msgs::msg::String>("chatter", 10);

    timer_ = this->create_wall_timer(
      std::chrono::seconds(1),
      [this]() {
        auto msg = std_msgs::msg::String();
        msg.data = "hello from rclcpp";
        pub_->publish(msg);
        RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", msg.data.c_str());
      });
  }

private:
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);                        // 内部调 rcl_init
  rclcpp::spin(std::make_shared<Talker>());        // executor 循环分发回调
  rclcpp::shutdown();                              // 内部调 rcl_shutdown
  return 0;
}
