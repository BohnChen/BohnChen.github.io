# ============================================================
# 第 4 层演示：裸 DDS 订阅者（对应 01_cyclonedds_pub.py）
# 两端之间没有任何 ROS，验证 DDS 自己能发现对方并传输数据。
# ============================================================

from cyclonedds import type as ddstype
from cyclonedds.domain import DomainParticipant
from cyclonedds.sub import DataReader
from cyclonedds.topic import Topic
from cyclonedds.util import duration

Hello = ddstype.struct("Hello", {
    "index": ddstype.uint32,
    "message": ddstype.str,
})

dp = DomainParticipant(0)
topic = Topic(dp, "HW_Topic", Hello)
rd = DataReader(dp, topic)

print("[DDS] 等待裸 DDS 数据（这证明 DDS 独立于 ROS 工作）...")
try:
    while True:
        for sample in rd.take(timeout=duration(seconds=1)):
            print(f"[DDS] 收到: index={sample.index}  message={sample.message}")
except KeyboardInterrupt:
    pass
