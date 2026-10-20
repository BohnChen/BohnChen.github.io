---
title: "AI 输出的每个字，是思考出来的吗？"
date: 2026-10-20T10:32:54+08:00
draft: false
categories: ["AI"]
tags: ["AI", "常识学习"]
---

![AI 输出的每个字，是思考出来的吗？](/images/how_ai_think/how_ai_think_cover.png)

在[上一篇文章](/posts/weeklysummary/whatistoken/)中，我们聊了 Token 的本质——它是分词器把人类文本切碎、放入“内部词典”后的切片与编号。我们调用 API 付的费用，很大一部分也是按处理与生成的 Token 数量来计费的。

但这也引出了一个常常让人困惑的问题：

既然大模型吐出来的文字只是一串拼起来的 Token，那它到底算不算在“思考”？为什么它能准确拼出我们想要的答案，甚至能写代码、解微积分、指挥机器人？如果它在思考，这个思考过程本身到底消耗了什么？

---

## 1. 表面是词句接龙，背后是理解

从训练目标来看，大模型的工作机制确实很像“接龙”。

目前主流自回归大模型（Autoregressive LLM）的基本训练任务非常纯粹：**给定前面的上下文（Context），预测下一个最可能出现的词元（Next-Token Prediction）。**

听起来这就像一个算力极其强大的“手机输入法联想”，对吧？

但表面简单的机制，在喂入海量人类知识与网络数据之后，发生了一种质变。OpenAI 前首席科学家 Ilya Sutskever 在一次深度访谈中给出过一个非常精辟的判断：

> **“预测下一个词，表面是统计，背后是理解。”**  
> —— Ilya Sutskever

我们可以看一个直观的例子：

> **推理想象：**  
> 侦探走进房间，发现地上有一具尸体，窗户紧锁，但桌上放着一杯还在冒热气的咖啡，这说明凶手刚刚离开不到……

要想准确把下一个词预测为“十分钟”或“片刻”，模型单凭死记硬背文本前后的字面搭配是根本做不到的。它在深层必须处理好几组隐含事实：

1. **物理规律：** 咖啡冒热气说明温度尚高，而液体降温需要时间；
2. **因果与空间逻辑：** 密闭房间、热咖啡与凶手作案的先后顺序；
3. **时间尺度推演：** 综合现实世界常识，收敛出一个合理的时间区间。

为了把“下一个词”猜得足够准，模型在数十亿乃至数万亿参数的权重空间中，不得不自发建立起一套对现实世界物理规律、逻辑因果与常识的内部抽象映射。在学术界，这被称为**“涌现的隐式世界模型”（Emergent World Representations）**。

它像人类一样拥有主观体验或情感吗？**没有。**  
但它是否具备复杂的表征与逻辑推理能力？**确实具备。** 它不是简单的数据库字符串剪切拼接，而是在高维向量空间里，依据语义几何与因果关系推导下一步的发展。

---

## 2. AI 每吐出一个词，经历了什么？

把大模型生成下一个词的“内部流水线”放慢动作，核心主要由以下几个环节构成：

![大模型生成下一个词（Next-Token）流水线](/images/how_ai_think/next_token_pipeline.png)

### 2.1 高维几何里的“向量计算”（Embedding & Attention）

大模型的神经网络无法直接读取字符，每个词在被分词器切分成 Token 后，会被投射成一个包含数千个维度的空间坐标（向量，Vector Embedding）。

- **几何空间上的近邻关系：** 语义相近的词，在向量空间中的距离也更近（例如“国王”与“王后”，“草莓”与“水果”）；
- **注意力机制（Self-Attention）：** 当输入一整句话时，模型会让序列里的每个 Token 与其他 Token 相互计算关联权重。

> 比如看到“苹果”两个字：如果上下文出现了“发布会”、“芯片”，注意力权重就会把它拉向科技公司的语义聚类；如果上下文出现的是“果盘”、“脆甜”，它就会被拉向水果的语义方向。

### 2.2 概率大转盘与采样输出（Softmax & Sampling）

经过数十层 Transformer 网络的层层变换与多头注意力聚合，模型会在最后一层为词表中的几万甚至十数万个候选 Token 分别计算打分（Logits）。

随后，通过 **Softmax 函数** 将这些分数归一化为概率分布：

- 词 A（比如“把”）：$0.001\%$
- 词 B（比如“十分钟”）：$85.2\%$
- 词 C（比如“桌子”）：$0.02\%$

最后，模型根据设定的采样策略（例如贪心搜索选概率最高者，或通过 Temperature、Top-p 引入适度随机性），选定并输出这一个 Token。

选出的新 Token 会被重新拼接回原来的文本末尾，作为新的上下文，驱动模型继续计算再下一个词。这种周而复始的滚动循环，就是大模型“逐字输出”的完整过程。

---

## 3. 思考过程到底消耗 Token 吗？

搞清楚了接龙的原理，回到核心问题：**AI 思考时，到底消不消耗 Token？**

答案要分两个层次来看：**内部的“隐式计算”**，以及 **外部的“显式思考过程”**。

![大模型隐式计算与显式思考对比](/images/how_ai_think/compute_vs_cot.png)

### 3.1 内部隐式计算：消耗算力，但不消耗计费 Token

当一个或一组 Token 输入模型后，在吐出下一个 Token 之前，模型内部发生了极大规模的矩阵乘法运算。

以一个 70B（700 亿）参数的模型为例，为了猜测下一个词，GPU 内部要执行上千亿次浮点运算（FLOPs）。这部分算力完全发生在 GPU 显存和张量核心（Tensor Cores）内部：

- 它消耗的是**电力、芯片算力以及硬件运行时间（延迟）**；
- 它**不计入**向用户输出的 Token 数量账单；
- 这一步很像人脑神经元电信号的闪烁，只要没被诉诸笔端或说出口，就属于内部开销。

### 3.2 显式思考过程：必须消耗额外的 Token

既然内部有这么庞大的运算，为什么面对复杂推理题时，模型还是经常会“张嘴就错”？

原因在于神经网络的前向传播机制：**大模型每生成一个 Token，所能调用的网络层数和计算步骤是严格固定的（也就是常说的 $O(1)$ 计算图深度）。**

如果要模型在生成单个 Token 的恒定时间内，一口气想通 10 步环环相扣的严密数理逻辑，内部的表达容量与深度是不够用的。为了解决复杂推理问题，业界引入了更进一步的机制，而这便需要实打实地消耗 Token：

#### ① 思维链提示（Chain-of-Thought, CoT）

我们在写提示词时常加上“请一步一步思考”（Let's think step by step）。这时 AI 不会直接报出最终数字，而是先把推导过程按步骤写出来：

- `“第一步：设原有总数为 x……”`（消耗 20 个 Token）
- `“第二步：根据题意可列出方程……”`（消耗 35 个 Token）
- `“因此得出最终答案为……”`

这些中间步骤生成的 Token，本质上就是把原本塞在黑盒里的计算步骤**“写在草稿纸上”**。通过把前面自己写出来的思考步骤再次喂回给自己作为上下文，后续 Token 的预测准确率就会呈阶梯式提升。

#### ② 原生思考型模型（如 OpenAI o1 / o3 系列、DeepSeek-R1）

在最新的推理增强型模型中，这种机制被沉淀到了底层架构与训练阶段。

在正式给出最终回答前，模型会进入一个内部推理阶段（通常包裹在 `<thinking>` 标签或特定推理通道中）。它在后台自我反思、探索备选假设、验算中间步骤、修正逻辑死胡同，悄悄生成数百上千个**“思考 Token”（Reasoning Tokens）**。

虽然很多产品前端将这部分思考折叠或隐藏了起来，但这些思考 Token 是真实参与了自回归前向计算的，在底层的 GPU 算力和 API 账单上也都需要切实行计费。

---

## 4. 总结

回顾全文，关于“AI 如何思考”我们可以形成几个清晰的认知：

1. **运作形式上**：AI 依然运行在“下一个词预测”（Next-Token Prediction）的框架内；
2. **能力本质上**：为了把词猜准，模型在参数空间内涌现出了对现实规律和逻辑因果的隐式世界表征，“统计之上有理解”；
3. **成本消耗上**：单步内部推理只消耗 GPU 算力与电力，不计入 Token；而当面临复杂逻辑必须将推理过程显式展开（CoT 或推理模型思考阶段）时，思考本身就变成了需要计量与付费的 Token。

---

## 参考文献与延伸阅读

1. **Attention Is All You Need**  
   Vaswani, A., Shazeer, N., Parmar, N., Uszkoreit, J., Jones, L., Gomez, A. N., Kaiser, Ł., & Polosukhin, I. (2017). *Advances in Neural Information Processing Systems (NeurIPS 2017)*.  
   [arXiv:1706.03762](https://arxiv.org/abs/1706.03762)  
   *（Transformer 架构奠基之作，系统阐述了 Self-Attention 机制与多头注意力原理。）*

2. **Emergent World Representations: Exploring a Sequence Model Trained on a Synthetic Task**  
   Li, K., Hopkins, A. K., Bau, D., Viégas, F., Pfister, H., & Wattenberg, M. (2023). *International Conference on Learning Representations (ICLR 2023)*.  
   [arXiv:2210.13382](https://arxiv.org/abs/2210.13382)  
   *（通过黑白棋/奥赛罗实验严格证明：单纯基于下一个 Token 预测训练的序列模型，内部能够自主涌现出对棋盘真实状态的高维世界表征。）*

3. **Emergent Linear Representations in World Models of Self-Supervised Sequence Models**  
   Nanda, N., Lee, A., & Wattenberg, M. (2023). *Proceedings of the 2023 BlackboxNLP Workshop (EMNLP 2023)*.  
   [arXiv:2309.00941](https://arxiv.org/abs/2309.00941)  
   *（深入可解释性领域，揭示了序列模型内部世界模型的线性表征结构与探测方式。）*

4. **Chain-of-Thought Prompting Elicits Reasoning in Large Language Models**  
   Wei, J., Wang, X., Schuurmans, D., Bosma, M., Ichter, B., Xia, F., Chi, E., Le, Q., & Zhou, D. (2022). *Advances in Neural Information Processing Systems (NeurIPS 2022)*.  
   [arXiv:2201.11903](https://arxiv.org/abs/2201.11903)  
   *（思维链 CoT 奠基论文，阐明了中间推理步骤 Token 如何激发大模型的复杂推理能力。）*

5. **Ilya Sutskever: Why Next-Token Prediction Could Surpass Human Intelligence**  
   Patel, D. (Host). (2023, March 27). *The Lunar Society Podcast / Dwarkesh Podcast*.  
   *（OpenAI 前首席科学家 Ilya Sutskever 详述“预测下一个词表面是统计，背后是对生成该 Token 的现实世界的深刻理解”的核心论点。）*
