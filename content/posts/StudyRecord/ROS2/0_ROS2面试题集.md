# ROS2 面试题集
---
title: "ROS2 面试题集"
date: 2026-08-26T13:31:48+08:00
draft: false
categories: ["ROS2"]
tags: ["ROS2", "技术学习"]
---

> 面向：机器人软件 / 嵌入式（ROS2）方向实习面试
> 配套项目：灵犀 X2 巡检机器人、Dummy 六轴机械臂（ros2_control 插件）
> 使用方法：先自己答，再看答案；最后遮住答案复述。

---

## 一、ROS2 基础

### Q1. ROS2 相比 ROS1 最大的架构变化是什么？

**答案：**

ROS1 有一个 **Master 节点**（`roscore`），所有节点启动时必须先找 Master 注册，节点之间通过 Master 完成发现和通信握手。**Master 挂了，整个系统瘫痪**。

ROS2 **去掉了 Master 节点**，采用**分布式架构**：

- 每个节点启动时通过 DDS 的 **Discovery 机制**（默认 UDP 组播）在网络上互相发现
- 节点之间的通信由底层 DDS 中间件直接完成，不经过任何中心节点
- 任意节点挂掉不影响其他节点

**面试延伸：**
- 为什么 ROS1 要 Master？——中心化便于管理，但单点故障 + 不适用多机部署
- ROS2 为什么能去掉？——因为选了 DDS 这种本就支持分布式自动发现的中间件

**对应项目：** 你的灵犀 X2 和机械臂项目都是 ROS2，可说「我平时就是 `ros2 run` 直接起节点，不需要 roscore」。

---

### Q2. rclcpp / rcl / rmw / DDS 四层分别是什么？

**答案：**

| 层 | 全称 | 作用 |
|---|---|---|
| **rclcpp** | ROS Client Library for C++ | 给 C++ 开发者用的高级 API（`Node`、`Publisher`、`Subscriber`） |
| **rcl** | ROS Client Library（C 层） | 统一的抽象层，供 rclcpp/rclpy 等语言绑定调用 |
| **rmw** | ROS Middleware Interface | ROS 到 DDS 的中间层抽象接口，屏蔽不同 DDS 实现的差异 |
| **DDS** | Data Distribution Service | 真正的底层通信中间件（Fast DDS / Cyclone DDS 等） |

**调用链：**
```
rclcpp (你的 C++ 代码)
   ↓
rcl (通用 ROS API)
   ↓
rmw (中间件抽象层)
   ↓
DDS (Fast DDS / Cyclone DDS / RTI Connext)
   ↓
UDP/TCP 网络栈
```

**面试延伸：**
- 为什么加 rmw 层？——为了可以**切换不同 DDS 实现**（`RMW_IMPLEMENTATION` 环境变量），也便于测试
- 你现在用的哪个 DDS？——`ros2 doctor` 或 `echo $RMW_IMPLEMENTATION` 可查，默认通常是 Fast DDS 或 Cyclone DDS

---

### Q3. ros2 run、ros2 node list、ros2 topic list、ros2 doctor 分别干什么？

**答案：**

| 命令 | 作用 |
|---|---|
| `ros2 run <pkg> <executable>` | 运行某个包里的一个节点（可执行文件） |
| `ros2 node list` | 列出当前发现的**所有节点** |
| `ros2 topic list` | 列出所有**话题**（加 `-t` 显示类型，`-v` 显示详细） |
| `ros2 doctor` | 检查 ROS2 环境健康状况（DDS 配置、网络、环境变量），排障首选 |

**常用调试三件套：**
```bash
ros2 node info /node_name          # 看节点的订阅/发布/服务
ros2 topic echo /topic_name        # 实时看消息内容
ros2 topic pub /topic std_msgs/msg/String "{data: 'hello'}" --once
rqt_graph                          # 图形化看节点话题连接关系
```

---

## 二、通信机制

### Q4. Topic / Service / Action 三者的本质区别？各自典型应用场景？

**答案：**

| | Topic | Service | Action |
|---|---|---|---|
| 模型 | 发布-订阅（发布者/订阅者） | 请求-响应（客户端/服务端） | 目标-反馈-结果（客户端/服务端） |
| 方向 | 单向、持续 | 双向、一次一问一答 | 双向、长时间任务 |
| 同步性 | 异步（发布者不等接收者） | 同步（客户端阻塞等待响应） | 异步，但能跟踪进度 |
| 底层实现 | DDS Publisher/Subscriber | DDS Request/Reply（内部是 Service Topic 对） | 底层包了 Service + 3 个 Topic |
| 适用 | 传感器数据、状态广播 | 查询、设置、一次性计算 | 导航、抓取、轨迹执行 |

**典型场景：**
- Topic：相机图像 `/image_raw`、机器人速度指令 `/cmd_vel`、关节状态 `/joint_states`
- Service：请求地图、设置参数、查询某个值
- Action：MoveIt 抓取、Nav2 导航到目标点

**对应项目：**
- 灵犀 X2：你用 Service 调度运动/表情（短操作）、TTS 播报
- 机械臂：MoveIt 用 Action 下发轨迹（`/follow_joint_trajectory`）

**一句记忆：**
> 需要**持续流**用 Topic，需要**立刻要结果**用 Service，需要**长时间干活还能看进度/取消**用 Action。

---

### Q5. "Topic 是异步的，Service 是同步的" 这句话对不对？

**答案：** 基本对，但要严谨。

- **Topic**：发布者 `publish()` 后立刻返回，不关心有没有订阅者、订阅者收到没有——**异步**。
- **Service**：客户端 `call()` 会**阻塞等待**服务端响应——**同步**。

**但是：** ROS2 里 Service 也有**异步调用接口**（`call_async()`），不会阻塞主线程，用 Future 拿到结果。所以严格说：

> Topic 一定是异步的；Service 既有同步调用也有异步调用，但通信模式本质是「请求-响应」。

---

### Q6. Action 内部是用什么实现的？

**答案：**

Action 的 `.action` 文件（Goal / Feedback / Result 三部分）在编译时会生成：

- **3 个 Topic：**
  - `.../goal` → 发目标
  - `.../feedback` → 服务端回报进度
  - `.../status` → 目标状态（PENDING/ACCEPTED/EXECUTING/SUCCEEDED/CANCELED）

- **3 个 Service：**
  - `.../send_goal` → 提交目标
  - `.../cancel_goal` → 取消目标
  - `.../get_result` → 获取结果

**一句话：Action = Service（请求-响应框架）+ Topic（反馈与状态流）的封装。**

**面试延伸：** 面试官问「Action 为什么是异步的？」——因为反馈是走 Topic 的，不是阻塞等待。

---

### Q7. 一条消息从 Publisher 到 Subscriber，中间经历了什么？

**答案（全链路）：**

```
Publisher (C++ 对象)
  → 序列化 (message → 二进制，rosidl 生成代码)
  → rclcpp → rcl → rmw → DDS DataWriter
  → DDS 协议（分片/组包）
  → UDP 网络
  → DDS DataReader
  → 反序列化 (二进制 → message)
  → rcl → rclcpp → 回调函数 (callback)
```

**关键点：**
- 消息跨进程/跨机时是**二进制序列化**传输的，不是直接传对象
- 同一进程内（intra-process）通信有优化通道（`intra-process` 模式可跳过序列化）
- QoS 决定这条消息**可靠性**和**时效性**

---

### Q8. 回调函数默认在哪个线程执行？多个回调如何并发？

**答案：**

- ROS2 节点**默认只有一个线程**（单线程 executor），所有回调（订阅、定时器、服务）在这个线程里**串行**执行。
- 一个回调阻塞（比如 `sleep`、慢逻辑），其他回调全部卡住。

**多回调并发的方式：**
1. **多线程 executor**（`MultiThreadedExecutor`）：用线程池并行处理回调
2. **回调组（Callback Group）**：把互不依赖的回调分到不同组，可并行
3. 单线程时用**多个 node + 多个 executor** 或 `rclcpp::spin` 多个

**面试延伸（重要）：**
> 如果你在回调里做耗时操作（比如调相机、写数据库），会阻塞其他回调 → 常见做法：**回调里只入队，worker 线程处理**。

---

## 三、DDS 与 QoS

### Q9. DDS 是什么？ROS2 为什么选 DDS？

**答案：**

**DDS（Data Distribution Service）** 是 OMG 制定的**分布式实时通信中间件标准**，核心模型是「全局数据空间（Global Data Space）」：所有参与者往空间里写数据/订阅数据，互相不直接连接。

**ROS2 选 DDS 的原因：**

| 需求 | DDS 提供的 |
|---|---|
| 去中心化、无单点故障 | DDS 自动发现，无中心节点 |
| 实时性 | 支持实时 QoS、低延迟 |
| 可靠性 | 可配置可靠/尽力传输、历史缓存 |
| 多机分布 | 开箱即用，跨机器自动发现 |
| 工业验证 | 军工/航天/工业界广泛使用 |
| 灵活性 | 可换实现（Fast DDS / Cyclone DDS） |

**面试延伸：** 代价是引入 DDS 概念复杂性（QoS、Discovery 配置），以及组播在网络受限环境的问题（需要配置 `ROS_DOMAIN_ID`、静态发现）。

---

### Q10. QoS 的 Reliability 和 Durability 策略各是什么意思？什么时候用哪种？

**答案：**

**Reliability（可靠性）：**
- **RELIABLE**：保证送达，丢失会重传（TCP 类似）
- **BEST_EFFORT**：尽力而为，丢了就丢了（UDP 类似），延迟低

**Durability（持久性）：**
- **VOLATILE**：只有订阅之后发布的消息才收到，之前的不缓存
- **TRANSIENT_LOCAL**：缓存最近发布的数据，**新订阅者**能立刻收到历史数据（比如 `/map`、`/robot_description`）

**使用建议：**

| 数据类型 | Reliability | 原因 |
|---|---|---|
| 相机图像 / 点云 / 激光 | **BEST_EFFORT** | 丢几帧没事，要低延迟，实时性优先 |
| 运动指令 / 服务 / 任务 | **RELIABLE** | 指令不能丢，否则机器人行为错误 |
| `/map`、`/robot_description`、静态 TF | **TRANSIENT_LOCAL** | 后启动的订阅者要能拿到 |

**对应项目：** 你的 D435i 相机图像节点应该设 BEST_EFFORT + VOLATILE；机械臂位置指令必须 RELIABLE。

---

### Q11. 什么是 DDS 的 Discovery？

**答案：**

DDS 用**自动发现**机制建立连接：

1. **Participant Discovery**：每个节点启动时发 UDP 组播宣告自己存在，交换 Participant 信息（端口、地址）
2. **Endpoint Discovery**：发现彼此后，交换各自的 Topic/端点信息，建立发布-订阅连接

**面试延伸：**
- 默认用组播（DDS domain 的 7400 端口附近），跨网段可能发现不到 → 需要配置静态发现或用 `ROS_DOMAIN_ID` 隔离
- 多机部署时，不同机器要保证 DDS 能互相发现（同一网段 + 防火墙放行组播）

---

### Q12. 两个节点 QoS 不匹配会发生什么？

**答案：**

- Topic 的 **Reliability / Durability 等 QoS 策略不兼容**时，**订阅端无法建立连接**，subscriber 收不到任何消息
- 典型报错：`New publisher discovered on topic ... , offering incompatible QoS` / `No compatible QoS found`

**排查方式：**
```bash
ros2 topic info /topic_name -v   # 看双方的 QoS 设置
```

**面试延伸：** 为什么图像容易连不上？—— 发布端 BEST_EFFORT，订阅端默认 RELIABLE，不匹配。所以订阅图像要显式设置 `best_effort`。

---

## 四、TF2

### Q13. TF 树是什么？一棵典型机械臂的 TF 树长什么样？

**答案：**

TF 树是机器人所有**坐标系之间变换关系**的有向图（树状，无环）。每个 frame 之间通过 parent-child 关系连接。

**机械臂 TF 树示例：**
```
world / base_link
   └─ joint1 → link1
        └─ joint2 → link2
             └─ joint3 → link3
                  └─ joint4 → link4
                       └─ joint5 → link5
                            └─ joint6 → link6 (tool0)
                                 └─ camera_link (相机装在末端)
```

**关键点：**
- 每个 link 一个 frame
- 变换来自两部分：URDF 里的**静态**变换（固定关节）和**动态**变换（运动关节，由关节角度实时计算）
- 在 RViz2 的 `TF` 插件里能看到完整的树

**对应项目：** 你的机械臂 ROS2 工作区里 `rqt_tf_tree` / `view_frames` 就能看到这棵树。

---

### Q14. lookupTransform 的三个参数是什么？

**答案：**

```cpp
tf_buffer_->lookupTransform(
    target_frame,     // 目标坐标系
    source_frame,     // 源坐标系
    tf2::TimePointZero // 时间点，TimePointZero 表示最新
);
```

**语义：** 返回一个 `TransformStamped`，包含 **source 坐标系下的一个点，变换到 target 坐标系** 所需的平移 + 旋转。

**面试延伸（方向要讲对）：**
- `lookupTransform("base_link", "camera_link", ...)` 意思是「把 camera_link 系的坐标转到 base_link 系」
- 记住：**第一个参数是目标，第二个是源，第三个是时间**。写反了结果是反变换，这是最常见的 bug。

---

### Q15. 静态变换和动态变换的区别？

**答案：**

| | 静态变换 | 动态变换 |
|---|---|---|
| 发布者 | `static_transform_publisher` 节点 | 机器人状态节点（`robot_state_publisher`） |
| 频率 | 一次发布，永久有效 | 随关节运动持续更新 |
| 例子 | 相机固定在基座上、传感器固定安装 | 机械臂各关节 `link_i → link_{i+1}` |
| 工具 | `ros2 run tf2_ros static_transform_publisher ...` | 由 URDF + 关节状态自动生成 |

**对应项目：** 机械臂的关节变换是动态的（读关节角度算出来）；相机如果固定装在基座上就是静态变换。

---

### Q16. view_frames 是干什么的？

**答案：**

```bash
ros2 run tf2_tools view_frames
```

生成当前 TF 树的**图形文件**（frames.pdf），用于：
- 检查 TF 树结构是否完整
- 找出**断链**（某个 frame 没发布）
- 排查「frame not found」问题

**面试延伸：** 你可以在机械臂联调时用它确认相机、tool0、base_link 是否都在树上，是**机械臂标定的第一步**。

---

### Q17. lookupTransform 报错「frame not found」/「never published」可能是什么原因？

**答案：**

1. **frame_id 拼写错误**（大小写、下划线不一致）——最常见
2. **某个 frame 没有 TF 发布**：比如 `robot_state_publisher` 没起、URDF 里没有该 link
3. **时间戳问题**：查询的时间点早于 TF 发布的历史 → 用 `tf2::TimePointZero` 或给足够缓冲
4. **树断裂**：parent-child 链中间某处断了 → 用 `view_frames` 检查

**排查口诀：** 先 `view_frames` 看图 → 再 `grep` 检查 frame_id 拼写 → 再查时间戳。

---

## 五、URDF / Xacro

### Q18. URDF 里 link 和 joint 的区别？joint 有哪些类型？

**答案：**

- **link**：刚体（一个部件），有惯性、碰撞、视觉属性
- **joint**：连接两个 link 的关节，定义父 link 和子 link 之间的变换关系

**joint 类型：**

| 类型 | 含义 | 例子 |
|---|---|---|
| `revolute` | 旋转关节（有限制角度） | 机械臂各关节（J1-J6） |
| `continuous` | 无限旋转关节 | 轮子 |
| `prismatic` | 平移关节 | 直线导轨 |
| `fixed` | 固定 | 相机安装在支架上 |
| `floating` / `planar` | 6D/平面自由 | 少见 |

**对应项目：** Dummy 机械臂 6 个关节全是 `revolute`，相机安装用 `fixed`。

---

### Q19. Xacro 相比纯 URDF 有什么好处？

**答案：**

- **宏（macro）**：定义可复用的部件（比如 6 个相同电机关节只需写一个宏）
- **参数化**：用 `<xacro:property>` 定义尺寸/颜色，一处修改全局生效
- **数学运算**：在 XML 里做计算（`${pi/2}`）
- **减少重复**：代码量大幅减少，易维护

**对应项目：** 你的 `dummy-ros2.urdf.xacro` 就是用 xacro 宏生成的，6 个关节由宏展开。

---

### Q20. URDF 里 `<origin>` 的 xyz 和 rpy 是什么意思？

**答案：**

`<origin>` 描述 **joint 父坐标系到子坐标系的变换**：

```xml
<origin xyz="0.1 0 0.2" rpy="0 0 1.57"/>
```
- `xyz`：父系原点到子系原点的**平移**（米）
- `rpy`：绕 X、Y、Z 轴的**旋转**（弧度，Roll-Pitch-Yaw）

**面试延伸（机械臂标定核心）：**
> 机械臂实际装配位置和 URDF 里的 origin 有误差 → 末端定位不准。**标定**就是测出真实 origin 并修正。你的手眼标定第一步就是确认 camera 的 origin 与机械臂 tool0 的关系。

---

### Q21. 一个关节的「正方向」怎么定义？

**答案：**

由两点决定：
1. **joint 的旋转轴 `axis`**：`<axis xyz="0 0 1"/>` 定义绕哪个轴转
2. **`rpy` 的方向约定**：用右手定则，正方向 = 沿轴方向看逆时针

**对应项目（关键）：**
> 你写 ros2_control 插件时，`#GETJPOS` 读到的角度符号、`>J1..J6` 下发角度的符号，**必须和 URDF 里的正方向定义一致**。否则机械臂会往反方向动。联调第一步就是验证这个正负号。

---

## 六、ros2_control

### Q22. ros2_control 是干嘛的？ControllerManager、Controller、HardwareInterface 的关系？

**答案：**

ros2_control 是 ROS2 的**机器人控制框架**，统一管理「控制器」和「硬件」的对接。

**三大组件：**

| 组件 | 职责 |
|---|---|
| **ControllerManager** | 中央管理者，加载/激活/停用控制器，驱动控制循环 |
| **Controller** | 算法层：接收目标轨迹 → 计算关节期望 → 发布命令（如 joint_trajectory_controller） |
| **HardwareInterface** | 硬件层：`read()` 读传感器状态，`write()` 写执行器命令（你的插件） |

**数据流（一个控制周期）：**
```
ControllerManager.update()
  → 调用各 Controller.update()
  → 控制器读 state_interfaces、算命令
  → 调用 HardwareInterface.write() 下发到硬件
  → 调用 HardwareInterface.read() 读回实际状态
```

**面试画图（必会）：**
```
MoveIt (轨迹目标)
  ↓ Action
joint_trajectory_controller (Controller)
  ↓ command_interfaces (期望位置)
DummyArmSystem (你的 HardwareInterface 插件)
  ↓ read()/write()
USB 串口 → Dummy 机械臂
```

---

### Q23. SystemInterface 和 actuator_interface 的区别？你的机械臂该用哪个？

**答案：**

| | actuator_interface | system_interface |
|---|---|---|
| 粒度 | 单个执行器 | 整个机器人系统（多个关节 + 多传感器） |
| 适用 | 一个电机/一个舵机 | 多关节机械臂、移动底盘 |
| 接口 | 通常一个 joint | 一次暴露所有 joint |

**你的机械臂 → 用 `SystemInterface`**：因为它有 6 个关节，作为一个整体系统控制。

---

### Q24. read() 和 write() 在一个控制周期里谁先谁后？为什么？

**答案：**

**顺序是：write() 在前，read() 在后**（每个控制周期内）。

```
控制周期 (例如 10ms)：
  1. write()：把控制器算出的期望位置/命令写到硬件（下发 >J1..J6）
  2. （硬件执行，运动）
  3. read()：从硬件读回实际关节状态（#GETJPOS）
  4. 状态反馈给控制器 → 继续计算下一周期命令
  5. 循环
```

**为什么 write 先于 read？**
- 让硬件先收到命令开始动作
- 本周期 read 到的是**上一周期命令执行后的实际结果**，反馈给控制器作为误差计算的输入
- 这样才能形成「指令 → 执行 → 反馈 → 纠偏」的闭环

**面试延伸（高赞回答点）：** read 读到的状态用于控制器，write 把控制器的输出写给硬件。顺序错了，控制逻辑会滞后一个周期甚至震荡。

---

### Q25. control period（控制周期）设为多少？为什么不能太慢/太快？

**答案：**

- 常见设置：**100Hz（10ms）**，运动控制常用 1kHz（1ms）甚至更高
- 你的配置文件 `ros2_controllers.yaml` 里 `update_rate: 100`

**不能太慢：** 控制周期长 → 反馈滞后大 → 机械臂跟踪轨迹误差大，甚至不稳定震荡。
**不能太快：** 周期太短要求：
- 硬件能跟上（串口往返时间、电机响应）
- CPU 能在周期内完成所有计算
- 若 `read()/write()` 阻塞超时，控制循环错过 deadline → 整个系统紊乱

**面试延伸：** 嵌入式面试官常问「你怎么保证控制周期实时性？」→ 答案：非阻塞串口、超时保护、实时线程/实时内核优先级。

---

### Q26. command_interface 和 state_interface 的区别？

**答案：**

| | command_interface | state_interface |
|---|---|---|
| 方向 | **控制器 → 硬件**（写命令） | **硬件 → 控制器**（读状态） |
| 例子 | 期望关节位置、速度、力矩 | 实际关节位置、速度、力矩 |
| 类型 | position / velocity / effort | position / velocity / effort |

**你的机械臂配置（xacro 里）：**
```xml
<joint name="Joint1">
  <command_interface name="position"/>   <!-- 下发期望位置 -->
  <state_interface name="position"/>      <!-- 读实际位置 -->
  <state_interface name="velocity"/>      <!-- 读实际速度 -->
</joint>
```

**面试延伸：** 如果你的臂只有位置命令接口，那 `joint_trajectory_controller` 就是纯位置跟踪，不涉及力矩控制。

---

### Q27. joint_trajectory_controller 和 joint_state_broadcaster 各负责什么？

**答案：**

| 控制器 | 作用 |
|---|---|
| **joint_trajectory_controller** | 接收轨迹（MoveIt 发的 Action 目标），按时间插值出每个关节的期望位置，写 command_interfaces |
| **joint_state_broadcaster** | 读取 state_interfaces（实际关节位置/速度），发布到 `/joint_states` 话题 |

**对应项目：** 你的 `ros2_controllers.yaml` 里同时配了这两个。MoveIt 的轨迹 → trajectory controller 执行；`/joint_states` → RViz 显示真实关节、robot_state_publisher 计算 TF。

---

### Q28. 硬件插件 read() 读到 NaN，或串口断开，怎么办？

**答案（安全设计，嵌入式面试高频）：**

**NaN 处理：**
1. 校验数值合法性（`std::isfinite`），发现 NaN 丢弃该周期数据
2. 连续 N 次读到非法数据 → 触发错误状态，通知控制器/安全机制

**串口断开：**
1. read/write 返回错误 → 设置硬件故障标志
2. **触发安全动作**：比如发送 `!STOP` 急停 / 断开使能 / 保持并报告
3. 通过日志和状态话题（如 `/diagnostics`）上报给上层
4. 重连机制：检测到串口恢复后重新初始化

**面试话术（很加分）：**
> 我在插件里会做三件事：数据合法性校验、连续故障计数、故障时触发 `!STOP` 急停并上报 diagnostics。机器人控制**安全优先**，宁可不动作也不能乱动作。

---

## 七、MoveIt

### Q29. MoveIt 的 move_group 是什么？

**答案：**

`move_group` 是 MoveIt 的**核心中央节点**，它：
- 持有机器人模型（URDF/SRDF）和规划场景（Planning Scene）
- 提供 Action/Service 接口给用户：发目标 → 内部做运动规划 → 返回轨迹
- 是「用户逻辑」和「运动规划/IK」之间的中间人

**调用流程：**
```
你的节点 (例如机械臂抓取节点)
  ↓ Action (plan + execute)
move_group
  ↓ 调用
OMPL 规划器 (规划关节轨迹)
  ↓ 轨迹
joint_trajectory_controller (ros2_control 执行)
```

---

### Q30. Planning Scene、Planning Group、Robot Model 分别是什么？

**答案：**

| 概念 | 含义 |
|---|---|
| **Robot Model** | 机器人 URDF/SRDF 模型，包含 link/joint/几何/碰撞 |
| **Planning Group** | 一组关节的集合，视为一个可规划的整体（如 arm 组 = 6 个关节） |
| **Planning Scene** | 当前环境状态：机器人在哪 + 障碍物在哪（世界模型） |

**面试延伸：** 抓取时你把检测到的障碍（树干、枝干）加进 Planning Scene，规划器就会避障。

---

### Q31. 关节空间规划 vs 笛卡尔空间规划的区别？

**答案：**

| | 关节空间规划 | 笛卡尔空间规划 |
|---|---|---|
| 规划对象 | 各关节角度变化 | 末端执行器的直线轨迹 |
| 路径 | 关节路径（末端的实际路径不是直线） | 末端走直线 |
| API | MoveIt 默认（`plan()`） | `compute_cartesian_path()` |
| 耗时 | 快 | 慢，要逐点 IK |
| 场景 | 一般移动 | 需要直线运动（如拧螺丝、画线） |

**对应项目：** 抓取柑橘时，接近果实用**笛卡尔直线**轨迹（`@X,Y,Z,A,B,C` 对应直线指令），避免碰撞。

---

### Q32. OMPL、RRT-Connect 是什么？

**答案：**

- **OMPL（Open Motion Planning Library）**：开源的采样式运动规划库，MoveIt 的默认规划器后端
- **RRT-Connect**：OMPL 里的一个规划算法——从起点和终点**同时扩展两棵树**，逐渐「双向连接」，快速找出一条可行路径
- 采样式 = 在高维空间随机采样 + 碰撞检测，不要求精确建模

**其他算法：** RRT、PRM、A*（栅格）、CHOMP/STOMP（轨迹优化）。

---

### Q33. MoveIt 怎么知道避开障碍？

**答案：**

1. **碰撞检测**：用规划场景中的几何模型（机器人的 link + 障碍物的形状）做碰撞检测（FCL 库）
2. **碰撞矩阵**：哪些 link 对之间允许碰撞（比如相邻关节默认不检测）
3. 规划器采样时，**落在障碍物里的采样点/路径段直接丢弃**
4. 通过 `/collision_object` 话题把障碍物加入 Planning Scene

**面试延伸：** 你现在没做避障也能跑（空的 planning scene），但抓取真实场景必须加障碍物，否则机械臂可能撞树干。

---

## 八、工程实践

### Q34. 怎么调试一个「节点启动但没反应」的 ROS2 程序？

**答案（排障顺序）：**

```bash
ros2 doctor                    # 1. 环境健康
ros2 node list                 # 2. 节点起来没
ros2 node info /my_node        # 3. 节点话题/服务有没有
ros2 topic list                # 4. 话题在不在
ros2 topic echo /my_topic      # 5. 有没有消息在流
rqt_graph                      # 6. 连接关系对不对
```

**常见原因：**
- 环境变量没 source（`source /opt/ros/humble/setup.bash`）
- 节点崩溃但日志被吞（看 `ros2 run` 输出 / `~/.ros/log`）
- 订阅了但 QoS 不匹配，收不到消息
- 回调里死循环/阻塞

**日志级别：** `rclcpp::set_logger_level` 或 `RCLCPP_DEBUG` 查看内部状态。

---

### Q35. ros2 bag record 是干嘛的？

**答案：**

录制/回放话题数据：
```bash
ros2 bag record /image_raw /joint_states   # 录制
ros2 bag play my_bag                        # 回放
```

**用途：**
- 采集真实数据做离线调试（你采集柑橘数据集就能用）
- 复现 bug：录下现场数据，回放给开发环境看
- 传感器数据回放做算法测试，不用重复跑实机

---

### Q36. Launch 文件里怎么传参数？Python launch vs XML launch？

**答案：**

**传参数：**
```python
# Python launch
Node(package='demo', executable='node1',
     parameters=[{'my_param': 42}])  # 直接传参数

# 或用 LaunchConfiguration 实现启动时传入
launch_arg = DeclareLaunchArgument('use_sim_time', default_value='false')
```

**Python vs XML：**

| | Python | XML |
|---|---|---|
| 灵活性 | 高（可写逻辑/循环/条件） | 低 |
| 复杂度 | 代码多 | 简洁 |
| 适用 | 复杂启动逻辑 | 简单启动 |

**结论：现代 ROS2 推荐 Python launch**，能用条件和循环处理多节点复杂启动（比如机械臂启动时同时起 robot_state_publisher、controller_manager、MoveIt）。

---

### Q37. 两个节点一个在 VENTUNO Q 上、一个在电脑上，怎么通信？

**答案：**

ROS2 天然支持分布式（跨机器），只要：
1. **同一网段**，能互相 ping 通
2. **同一个 ROS_DOMAIN_ID**：`export ROS_DOMAIN_ID=42`（两边一致）
3. 防火墙放行 DDS 组播端口（默认 UDP 7400-7450 附近）

```bash
# VENTUNO Q
export ROS_DOMAIN_ID=42
ros2 run demo talker

# 电脑
export ROS_DOMAIN_ID=42
ros2 topic echo /chatter
```

**面试延伸：**
- 用 `ROS_DOMAIN_ID` 隔离多组机器人（同一局域网互不干扰）
- 如果跨网段发现不到：配置 DDS 静态发现（`CycloneDDS`/`FastDDS` 的 XML 配置），或关掉组播用单播对点

---

### Q38. ROS2 节点内存/CPU 占用太高怎么排查？

**答案：**

```bash
ps aux | grep node_name      # 看 CPU/内存
top -Hp <pid>                # 看线程占用
ros2 topic hz /some_topic    # 话题频率是不是异常高
ros2 topic bw /some_topic    # 话题带宽
```

**常见原因：**
- 传感器话题频率过高（比如相机 30Hz 大图）
- 回调里做了重活（推理、文件 IO）
- 消息队列积压（QoS History 太大，处理不过来）
- 日志刷屏

**优化：** 降帧率、图像压缩、多线程 executor、减小缓存。

---

## 九、综合设计题

### Q39. 给你一台机械臂 + 一个 RGB-D 相机，设计一个抓取系统的完整 ROS2 架构？

**答案（可画图）：**

```
[相机节点] camera_node (realsense-ros)
  ├─ /image_raw (BEST_EFFORT)
  ├─ /depth (BEST_EFFORT)
  └─ /camera_info

[检测节点] detection_node (YOLO 柑橘检测)
  ├─ 订阅 /image_raw
  └─ 发布 /detection_bbox (目标包围框)

[定位节点] locate_node (3D 定位 + 坐标变换)
  ├─ 订阅 /detection_bbox + /depth
  ├─ 用 TF 把相机系 → base_link 系
  └─ 发布 /target_pose (目标在 base_link 系下的位姿)

[抓取节点] grasp_node (决策 + 调 MoveIt)
  ├─ 订阅 /target_pose
  └─ 通过 Action 调 move_group (MoveIt)

[MoveIt]
  ├─ move_group
  └─ 执行 → joint_trajectory_controller
        ↓
   [dummyarm_hardware 插件] → 串口 → 机械臂
```

**关键点：**
- 视觉链路 BEST_EFFORT，控制链路 RELIABLE
- 用 TF 统一坐标系（base_link 为参考系）
- 抓取用 Action（长时间 + 可取消 + 看进度）

---

### Q40. eye-in-hand vs eye-to-hand，TF 树和标定有什么不同？

**答案：**

| | Eye-in-hand（相机在手腕） | Eye-to-hand（相机固定外部） |
|---|---|---|
| 相机位置 | 固定在机械臂末端 tool0 上 | 固定在基座/环境 |
| TF 链 | base_link → ... → tool0 → camera_link | base_link → camera_link（静态） |
| 相机随动 | 相机跟着机械臂动 | 相机不动 |
| 标定 | 标定 camera ↔ tool0 外参（会随机械臂动） | 标定 camera ↔ base_link 外参（一次标定永久有效） |
| 优点 | 越近越准，靠近目标后可二次定位 | 全局视野，一次标定 |
| 缺点 | 靠近时可能遮挡、视野小 | 距离远精度下降，目标可能被臂遮挡 |

**对应项目：** 你的柑橘采摘方案，相机装在**底盘/基座附近固定**（eye-to-hand）用于全局搜索；接近果实后如需精细定位，再考虑二次（此时是另一种思路）。

**标定本质：** 解一个 AX=XB / AX=ZB 的手眼标定方程（对应不同模型），用标定板（如棋盘格）采集多组数据求外参矩阵。

---

## 学习资源

| 主题 | 资源 |
|---|---|
| ROS2 入门 | 官方教程 https://docs.ros.org/en/humble/Tutorials.html |
| 中文视频 | B 站「鱼香ROS」（fishros） |
| ros2_control | 官方文档 https://control.ros.org/ + ros2_control_demos 仓库 |
| MoveIt | https://moveit.picknik.ai/ + moveit2_tutorials |
| TF2 | 官方 tf2 tutorials |
| DDS/QoS | ROS2 concepts 文档 + 「ROS 2 Design」RFC 文档 |

---

> 学习顺序建议：Q1-Q13（基础+通信）→ Q18-Q21（URDF/TF）→ Q22-Q28（ros2_control，配合你的插件开发）→ Q29-Q33（MoveIt）→ 综合题最后看。
