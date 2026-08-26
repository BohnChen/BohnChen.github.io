---
title: "rclcpp / rcl / rmw / DDS 这四层分别是什么"
date: 2026-08-25T15:11:03+08:00
draft: false
categories: ["ROS2"]
tags: ["ROS2", "技术学习"]
---

> 配套演示代码：`~/02_Personal/06_JobMaterial/ROS2_layers_demo`（4 个文件逐层降级演示同一个功能）

## 0. 前置：ROS2 基础概念怎么区分（何时用哪个）

| 概念 | 通信特征 | 适合的场景 | 典型例子 |
|---|---|---|---|
| **Node 节点** | 无通信，是"人"本身 | 一个计算单元，拥有话题/服务/动作/参数 | 导航节点、相机节点 |
| **Topic 话题** | 单向、持续流、一对多广播、**发布者不等回应** | 传感器流、状态上报，只管发不管收没收到 | `/image_raw`、`/odom`、`/clock`、TF |
| **Service 服务** | 短请求-应答、一次性、**同步等结果** | 查询、触发一次性操作，需要立刻拿到返回值 | 调参、请求 `get_plan`、设置状态 |
| **Action 动作** | 长任务、**可中途取消、带反馈 + 最终结果** | 耗时任务，边做边报进度，能取消 | 导航到目标点、机械臂运动、相机标定 |
| **Parameter 参数** | 键值对配置 | 节点启动配置、运行时调参 | 速度上限、话题频率 |

**选型口诀**：持续单向流 → **Topic**；问一句答一句、立刻返回 → **Service**；干一件很久的活、要进度能取消 → **Action**。

关键区分点：**Service 是"同步短事务"，Action 是"异步长事务"**。两者底层都是 Topic 拼出来的（见 §5），但语义完全不同——这就是 ROS 层加在 DDS 之上的"约定"。

---

## 1. 四层总览与调用链

ROS2 分层设计，发布一条消息会沿这条链一路沉到底：

```
rclcpp (C++ 客户端库)      ← 你平时写的代码在这一层
   │  rclcpp::create_publisher
   ▼
rcl    (C 接口层)          ← 无面向对象，一堆 rcl_* 函数
   │  rcl_publisher_init
   ▼
rmw    (中间件抽象层)      ← 纯接口，只定义函数签名
   │  rmw_publish
   ▼
DDS    (通信中间件)        ← 真正干活的，组网/发现/传输
```

| 层 | 本质 | 一句话职责 |
|---|---|---|
| rclcpp | C++ 客户端库（厚封装） | 面向对象 API：Node / Publisher / Executor / Timer |
| rcl | C 客户端库 | 把 DDS 能力翻译成"ROS 语义"，一次实现 C++/Python 共享 |
| rmw | 中间件抽象接口 | 屏蔽不同 DDS 差异，换中间件不用改代码 |
| DDS | 底层通信中间件 | 真正的网络传输、发现、QoS 执行者 |

---

## 2. 第 1 层：rclcpp —— 你平时写的就是它

面向对象封装，资源管理全自动（RAII）。`src/01_rclcpp_talker.cpp` 关键片段：

```cpp
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
}
```

**感受点**：`Node` 类、模板类型、智能指针、`spin()` 的线程分发、析构自动清理——这些全是 rclcpp 层的东西。你感知不到底层任何细节。它的兄弟实现是 **rclpy**（Python），与 rclcpp 平级。

---

## 3. 第 2 层：rcl —— 纯 C 接口层

没有类、没有模板，只有结构体 + `rcl_*` 函数，**所有资源手动初始化/清理**。`src/02_rcl_talker.c` 关键片段：

```c
/* 1. 初始化运行时上下文（rclcpp 里 rclcpp::init 就干这事） */
rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
CHECK(rcl_init_options_init(&init_options, allocator));
rcl_context_t context = rcl_get_zero_initialized_context();
CHECK(rcl_init(0, NULL, &init_options, &context));

/* 2. 创建节点 */
rcl_node_t node = rcl_get_zero_initialized_node();
CHECK(rcl_node_init(&node, "c_talker", "", &context, &node_options));

/* 3. 创建发布者：消息类型用 rosidl 生成的类型支持宏，没有模板 */
rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
CHECK(rcl_publisher_init(&publisher, &node,
                         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
                         "chatter", &pub_options));

/* 4. 发消息（rcl 的消息就是个 C 结构体） */
std_msgs__msg__String msg;
std_msgs__msg__String__init(&msg);
...
CHECK(rcl_publish(&publisher, &msg, NULL));

/* 5. 手动清理 —— rclcpp 靠 RAII 帮你做这些 */
CHECK(rcl_publisher_fini(&publisher, &node));
CHECK(rcl_node_fini(&node));
CHECK(rcl_shutdown(&context));
```

**感受点**：rclcpp 一个 `Node` 类、几行代码的事，这里全手动。**rcl 用 C 写一次，rclcpp 和 rclpy 都能共用它**，这就是分层复用。消息类型这层只做"类型分发"，真正的序列化在后面。

---

## 4. 第 3 层：rmw —— 中间件抽象层（纯接口）

不实现任何通信，只定义 `rmw_*` 函数签名。它的价值体现在：**代码一行不改，只改环境变量 `RMW_IMPLEMENTATION` 就切换底层 DDS**。`src/03_rmw_probe.cpp`：

```cpp
#include "rmw/rmw.h"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("rmw_probe");

  RCLCPP_INFO(node->get_logger(), "rmw 实现:   %s", rmw_get_implementation_identifier());
  RCLCPP_INFO(node->get_logger(), "序列化格式: %s", rmw_get_serialization_format());
}
```

运行 `ROS_DOMAIN_ID=0 RMW_IMPLEMENTATION=rmw_fastrtps_cpp ros2 run layers_demo rmw_probe` 与 `RMW_IMPLEMENTATION=rmw_cyclonedds_cpp ...`，打印的中间件实现不同，但**你写的代码完全没变**——这就是 rmw 存在的意义。

```bash
export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp   # Humble 起默认
# export RMW_IMPLEMENTATION=rmw_fastrtps_cpp  # 切换只需这一行
```

---

## 5. 第 4 层：DDS —— 独立完整的通信中间件

DDS 本身就能自己组网、发现、传输，完全不依赖 ROS。`dds_demo/01_cyclonedds_pub.py` 全程无 ROS API：

```python
from cyclonedds import type as ddstype
from cyclonedds.domain import DomainParticipant
from cyclonedds.pub import DataWriter
from cyclonedds.topic import Topic

# 定义消息类型：DDS 里的类型就是一个结构体，跟 ROS 的消息定义无关
Hello = ddstype.struct("Hello", {
    "index": ddstype.uint32,
    "message": ddstype.str,
})

dp = DomainParticipant(0)            # 领域 0：等价于 ROS_DOMAIN_ID=0
topic = Topic(dp, "HW_Topic", Hello)
pub = DataWriter(dp, topic)          # DataWriter = DDS 里的"发布者端点"
pub.write(Hello(index=0, message="hello from raw DDS"))
```

`02_cyclonedds_sub.py` 里配一个 `DataReader`，两端用**裸 DDS** 就能互相通信，全程没有 ROS——证明 DDS 独立于 ROS 工作。ROS2 只是坐在它上面，加了消息类型约定（rosidl）和工具链。

---

## 6. QoS 详解（面试高频追问）

QoS = Quality of Service，一组传输服务质量策略。来自 DDS 原生，ROS2 只是透传。`create_publisher("chatter", 10)` 里的 `10` 就是 History 深度。

核心策略：

| 策略 | 选项 | 含义 |
|---|---|---|
| History | `KEEP_LAST(n)` / `KEEP_ALL` | 缓存最近 n 条 / 全部缓存 |
| Reliability | `RELIABLE` / `BEST_EFFORT` | 丢包重传保证送达 / 尽力而为，丢了不补 |
| Durability | `TRANSIENT_LOCAL` / `VOLATILE` | 晚到的订阅者能否收到"最后一条旧数据" |
| Deadline / Lifespan | 时间值 | 超时未收到 / 数据过期作废 |

**必背洞察**：收发两端 QoS 必须**兼容**才能建连。订阅要 RELIABLE 而发布是 BEST_EFFORT 就连不上。

**面试组合拳**：
- 传感器（图像、点云、激光）：`BEST_EFFORT + KEEP_LAST(1)` —— 用最新帧，晚一点没关系
- 控制/任务指令：`RELIABLE + KEEP_LAST` —— 必须送达
- 映射/latch 型（TF、地图）：加 `TRANSIENT_LOCAL` —— 后上线的订阅者也能拿到旧值

代码示例：

```cpp
rclcpp::QoS qos(rclcpp::KeepLast(1));   // History: 只留最新 1 条
qos.reliability(rclcpp::ReliabilityPolicy::BestEffort);  // 传感器数据
qos.durability(rclcpp::DurabilityPolicy::TransientLocal); // 晚来也能收到
auto pub = this->create_publisher<sensor_msgs::msg::Image>("/image_raw", qos);
```

---

## 7. "rcl 把 DDS 能力封装成 ROS 语义"怎么理解

**核心矛盾**：DDS 不知道什么是"节点、话题、服务、参数"。DDS 世界里只有 4 样东西：`DomainParticipant`、`Topic`、`DataWriter`、`DataReader`。它是通用数据总线。

rcl 这层做的是**翻译 + 补约定**：

| DDS 概念 | rcl 映射成 | 说明 |
|---|---|---|
| `DataWriter` / `DataReader` | `rcl_publisher` / `rcl_subscription` | 一个 ROS 话题在 DDS 里是一对端点 |
| `Topic(name,type,QoS)` | ROS topic + 命名空间 + remap | 加名字符串语义、`node/topic` 层级 |
| 裸字节流 | rosidl 类型支持宏 | 强制 pub/sub 类型一致才能对接 |

**ROS 专属语义全是 rcl 用 DDS 拼出来的**：

- **Service**：底层是 2 组 topic（request topic + reply topic）+ 请求/响应消息约定。DDS 没有"服务"概念。
- **Action**：用 3 组 topic 拼成——goal、cancel、result/feedback。
- **参数、节点图（node graph）、命名空间/remap** 同理：rcl 解释 DDS 的发现数据，组装成 ROS 语义。

一句话：**DDS 只负责把字节高效送到；"这些字节是什么话题、谁是节点、怎么组服务"是 rcl 加上的语义。**

---

## 8. "rclcpp 是语言绑定"怎么理解

对，但不完全——这是面试区分点：

- **rclpy 是"轻绑定"**：把 rcl 的 C 函数用 Cython 几乎 1:1 包一层（`rcl_init` → `rclpy.init`），不加新东西。
- **rclcpp 是"厚封装"**，在 rcl 之上额外实现了 C 层没有的东西：
  - **Executor 线程模型**：C 层只有 `rcl_wait` 这种"等事件"原语，`spin()` 和单/多线程 Executor 是 rclcpp 自己实现的；
  - **RAII 生命周期**：构造自动建、析构自动 `fini`（对比 §3 里 5 步手动清理）；
  - **模板类型系统 + 智能指针**：`rclcpp::Publisher<std_msgs::msg::String>::SharedPtr`；
  - **Timer 封装**：`create_wall_timer` 在 rcl 里只是句柄，配回调挂进 executor 是 C++ 层的活。

准确说法：**rcl 用 C 实现一次，rclcpp/rclpy 让你分别用 C++/Python 调用；其中 rclpy 是轻量绑定，rclcpp 是绑定之上更厚、更强的面向对象封装**——所以 rclcpp 是一份代码量很大的独立仓库。

---

## 9. 澄清：DDS 明明有 Topic，为什么说它"不知道话题"？

**同名异物**——两个层级的 "Topic" 含义不同，这是个很好的面试区分点。

- **DDS 的 Topic** 是**底层原语**：只是 `(名字, 类型, QoS)` 三元组，唯一作用是**把 `DataWriter` 和 `DataReader` 匹配起来**。它就像一根水管，自己不知道水是热水还是咖啡。`Topic(dp, "HW_Topic", Hello)` 就只是个名字+类型，没有"节点、命名空间、话题/服务区分"的含义。
- **ROS 的 topic** 是**节点间通信的语义概念**：发布/订阅模式、命名空间、remap、节点图里"这个 publisher 属于哪个 node"。

准确说法：**DDS 不认识"ROS 话题"这个高层概念**。DDS 的 Topic 只是无语义的通道标签；ROS 里 `create_publisher("chatter", 10)` 在 DDS 下会建出带 namespace 的 DDS Topic，但 **"这是节点 X 的 publisher、承载什么 ROS 消息"这套语义是 rcl 层记下来的**。

一句话：DDS 的 Topic = 水管（名字+类型+QoS），ROS 的 topic = 你对这根水管的语义约定（属于谁、传什么、怎么暴露给 graph）。

---

## 面试一句话版

> "四层从下往上：DDS 是底层通信中间件，负责真正传输数据；rmw 是抽象层，把不同 DDS 统一成同一套接口，所以 ROS2 能换中间件；rcl 是 C 接口层，把 DDS 能力封装成 ROS 语义；rclcpp/rclpy 是最上层的语言绑定，给用户友好的面向对象 API。"
