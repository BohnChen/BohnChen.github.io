# ============================================================
# 第 4 层演示：DDS —— 完全不经过 ROS 的裸 DDS 发布者
# 用 CycloneDDS 的 Python 绑定，从头到尾没有出现任何 ROS API。
# 感受点：DDS 本身就是一个完整的独立中间件，能自己组网、
#         自己发现、自己收发数据；ROS2 只是坐在它上面加了
#         一层"消息类型约定"和工具链。
# 运行：pip install cyclonedds 之后 python3 01_cyclonedds_pub.py
#       再另开终端 python3 02_cyclonedds_sub.py，两边能通。
# ============================================================

import time

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

print("[DDS] 开始裸 DDS 发布（没有 ROS，没有 rclcpp）...")
for i in range(10):
    pub.write(Hello(index=i, message="hello from raw DDS"))
    print(f"[DDS] 发布 index={i}")
    time.sleep(1)
