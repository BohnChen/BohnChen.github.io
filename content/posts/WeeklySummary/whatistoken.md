---
title: "AI 小概念之——什么是 token ?"
date: 2026-10-19T21:25:55+08:00
draft: false
categories: ["AI"]
tags: ["AI", "技术学习"]
---

![AI 小概念之——什么是 token ?](/images/ai_era/what_is_token_cover.png)

最近看到阿加犀智能科技的官方公众号发了一篇文章，为了庆祝其在端侧大模型部署上取得的成绩。文章开头便说：

> “阿加犀智能科技在高通跃龙 IQ10 平台成功运行 Qwen3-Next-80B 大模型，当前实测 Prefill 速度达到 300+ tok/s，Decode 速度达到 20+ tok/s，上下文长度突破 16K。”

那么，这里出现了两次的 **tok（token）**，到底是什么？300+ tok/s 和 20+ tok/s 这样的速度，又到底意味着什么？

实际上，我们在使用 DeepSeek、ChatGPT 或 Claude 时计费扣的 Token、测速度说的 "tokens/s"，和我们读《RT-2: Vision-Language-Action Models Transfer Web Knowledge to Robotic Control》论文时看到的 RT-2 机器人吐出来的动作 Token，在底层原理上是完全一模一样的东西。

---

## 1. Token 与字符不是一一对应的

计算机不认识人类文字（比如“苹果”或者 "apple"），它只认识数字。大模型在读写文字之前，必须经过一个叫 **Tokenizer（分词器）** 的翻译官：

1. 它有一本“内置字典”（词表，通常包含 3 万到 10 几万个词元）。
2. 它把人类的文本切碎，每个碎片如果能在字典里找到，就给它一个编号（ID）。这个碎片和编号就叫 **Token**。

目前主流的大模型大多采用 **BPE（Byte-Pair Encoding，字节对编码）** 或基于其演进的分词算法（如 GPT-4 采用的 `cl100k_base`、Llama 采用的 SentencePiece BPE、Qwen 采用的分词器）。它的切分逻辑不是死记硬背每个词，而是统计高频出现的字符组合：

- **英文中**：一个 Token 大约等于 0.75 个英文单词，或者 3~4 个字母。
  - 常见单词 `"apple"`，字典里直接有，算 1 个 Token。
  - 如果是一个生僻词或组合词 `"unbelievable"`，分词器可能会切成：`"un"` + `"believ"` + `"able"`，算 3 个 Token。
- **中文中**：一个 Token 通常等于半个到一个汉字。
  - 常见汉字“我”、“爱”、“你”，每个字通常算 1 个 Token。
  - 一些少见的生僻字可能需要 2~3 个 Token 才能拼出来。
- **标点符号与空格**：一个逗号 `,`、一个空格，通常也是 1 个独立的 Token。

所以，我们每次调用大模型按 Token 计费，不是按字数算，而是按分词器切出来的“碎片编号数量”算。

---

## 2. tok/s 是什么含义？

这个指标衡量的是大模型打字有多快。大模型生成内容的过程，本质是“逐字蹦出来的”（自回归生成，Autoregressive）：

1. 预测第 1 个 Token（比如 `"今天"`）；
2. 拿着之前的输入 + `"今天"`，预测第 2 个 Token（比如 `"天气"`）；
3. 接着预测第 3 个 Token……

**Tokens/s（每秒生成的 Token 数）**：

- 如果一个模型的速度是 50 tokens/s，意味着它 1 秒钟能吐出 50 个 Token。
- 换算成人类阅读速度，大概就是 1 秒钟能在屏幕上打出 30~40 个汉字，肉眼看已经非常快了（像极速打字机）。

那么，为什么新闻里的实测指标要把速度拆成 **Prefill（预填充）** 和 **Decode（解码）** 两个完全不同的量级（300+ vs 20+）呢？

![大模型推理双阶段对比：Prefill 与 Decode](/images/ai_era/prefill_decode_diagram.png)

这两个阶段有着根本性的计算与硬件瓶颈差异：

1. **Prefill（预填充阶段，300+ tok/s）**：  
   当你在聊天框里贴了一篇几千字的长文并发送“帮我总结”，AI 在生成第一个回复字符之前，必须先将整个输入上下文一口气处理完毕。由于此时的所有提示词是已知的，GPU 可以利用强大的矩阵运算能力对这些输入 Token 进行**完全并行计算**。这一阶段属于算力受限（Compute-bound），因此吞吐极高，能跑出几百甚至上千 tok/s。

2. **Decode（自回归解码阶段，20+ tok/s）**：  
   到了模型开始回答吐字的阶段，它必须每生成一个 Token，将其追加到历史序列中，才能计算下一个 Token。这种**单步串行依赖**导致 GPU 无法提前并行，每生成一个 Token 都需要从高带宽显存中把庞大的数十 GB 权重参数完整读取一遍。这一阶段属于内存带宽受限（Memory-bound），因此速度会显著回落到每秒十几到几十个 Token，这也是我们肉眼看到的“打字机流式输出速度”。

---

## 小结

到这里，逻辑就清晰了：**Token 就是分词器（Tokenizer）把人类文本切碎后，存放在自身词典中的编号和对应切片。** 大模型生成的内容，本质上也是由这些切片一个接一个预测出来的。

哎？那你可能会有疑问：难道 AI 回答问题，纯粹就是词句的再组合吗？它到底会不会思考？它怎么知道该什么时候、怎么拼词？如果它思考怎么拼词，这个思考过程本身又耗不消耗 Token？

我们下一篇来探讨一下。

---

## 参考文献与延伸阅读

1. **Neural Machine Translation of Rare Words with Subword Units**  
   Sennrich, R., Haddow, B., & Birch, A. (2016). *Proceedings of the 54th Annual Meeting of the Association for Computational Linguistics (ACL 2016)*, pp. 1715–1725.  
   [arXiv:1508.07909](https://arxiv.org/abs/1508.07909)  
   *（将 BPE 算法引入自然语言处理的开山之作，奠定了现代大模型子词分词器 Tokenizer 的切分机制。）*

2. **RT-2: Vision-Language-Action Models Transfer Web Knowledge to Robotic Control**  
   Brohan, A., Chebotar, Y., Finn, C., Hausman, K., Herzog, A., Ho, D., Julian, R., Ibarz, J., Irpan, A., Jang, E., et al. (2023). *arXiv preprint*.  
   [arXiv:2307.15818](https://arxiv.org/abs/2307.15818)  
   *（Google 具身智能经典论文，展示了如何将机械臂的末端轨迹与离散动作空间直接映射为 Token 序列，与文本 Token 一同自回归预测。）*

3. **Efficient Memory Management for Large Language Model Serving with PagedAttention (vLLM)**  
   Kwon, W., Li, Z., Zhuang, S., Sheng, Y., Zheng, L., Yu, C. H., Gonzalez, J. E., Zhang, H., & Stoica, I. (2023). *Proceedings of the 29th ACM Symposium on Operating Systems Principles (SOSP 2023)*.  
   [arXiv:2309.06180](https://arxiv.org/abs/2309.06180)  
   *（系统论述了大模型服务中 Prefill（Compute-bound）与 Decode（Memory-bound）两阶段的根本差异及 KV Cache 内存优化机制。）*

4. **OpenAI Platform Documentation: What are tokens and how to count them?**  
   OpenAI. (2024). *OpenAI Official Guides & Tokenizer Tool*.  
   [https://platform.openai.com/tokenizer](https://platform.openai.com/tokenizer)  
   *（OpenAI 官方关于 Token 计算规则、不同语言切分比率以及计费计量方式的权威指南。）*
