// ============================================================
// 第 2 层演示：rcl —— 用纯 C 的 rcl API 写同一个 talker
// rclcpp 里一个 Node 类、几行代码的事，这里全是手动初始化/清理。
// 感受点：rcl 层没有任何"面向对象"，只有一堆结构体和 rcl_* 函数，
//         rclcpp 就是把这些包起来变成 C++ 类。
// ============================================================

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "rcl/rcl.h"
#include "rcl/error_handling.h"
#include "rosidl_typesupport_c/type_support.h"
#include "std_msgs/msg/string.h"

#define CHECK(fn)                                                              \
  do {                                                                         \
    rcl_ret_t __ret = (fn);                                                    \
    if (__ret != RCL_RET_OK) {                                                 \
      fprintf(stderr, "[rcl] %s 失败: %s\n", #fn, rcl_get_error_string().str); \
      return -1;                                                               \
    }                                                                          \
  } while (0)

int main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;

  rcl_allocator_t allocator = rcl_get_default_allocator();

  /* 1. 初始化运行时上下文（rclcpp 里 rclcpp::init 就干这事） */
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  CHECK(rcl_init_options_init(&init_options, allocator));
  rcl_context_t context = rcl_get_zero_initialized_context();
  CHECK(rcl_init(0, NULL, &init_options, &context));

  /* 2. 创建节点 */
  rcl_node_options_t node_options = rcl_node_get_default_options();
  rcl_node_t node = rcl_get_zero_initialized_node();
  CHECK(rcl_node_init(&node, "c_talker", "", &context, &node_options));

  /* 3. 创建发布者：注意消息类型是用 rosidl 生成的类型支持宏，没有模板 */
  rcl_publisher_options_t pub_options = rcl_publisher_get_default_options();
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  CHECK(rcl_publisher_init(&publisher, &node,
                           ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
                           "chatter", &pub_options));

  /* 4. 发消息（rcl 的消息就是个 C 结构体，序列化字段要自己维护） */
  std_msgs__msg__String msg;
  std_msgs__msg__String__init(&msg);
  char buffer[64];
  msg.data.data = buffer;
  msg.data.capacity = sizeof(buffer);

  for (int i = 0; i < 5; ++i) {
    int n = snprintf(buffer, sizeof(buffer), "hello from rcl #%d", i);
    msg.data.size = (n > 0 && (size_t)n < sizeof(buffer)) ? (size_t)n : strlen(buffer);
    CHECK(rcl_publish(&publisher, &msg, NULL));
    printf("[rcl] 发布了: %s\n", msg.data.data);
    sleep(1);
  }

  /* 5. 手动清理 —— rclcpp 靠 RAII 析构帮你做这些 */
  std_msgs__msg__String__fini(&msg);
  CHECK(rcl_publisher_fini(&publisher, &node));
  CHECK(rcl_node_fini(&node));
  CHECK(rcl_shutdown(&context));
  CHECK(rcl_context_fini(&context));
  return 0;
}
