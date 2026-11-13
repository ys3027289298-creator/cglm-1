# 分段录屏说明

## 产物

| 文件 | 说明 |
| --- | --- |
| `validation_demo.cast` | asciinema v2 完整录屏（4 段拼接，段间 1.5s 停顿） |
| `validation_demo.gif` | 由 cast 渲染的 GIF（790×560，约 20s） |
| `cast/seg_1.cast` … `seg_4.cast` | 四个分段的原始录屏 |
| `frames/` | GIF 抽帧，便于逐帧检查标题与内容 |
| `record.sh` | 一键重录 + 拼接脚本，无需任何键盘交互 |

## 四个分段（均带青色标题横幅）

1. **分段 1/4：退化输入可检出** —— 错误码定义 + 仓库测试入口中 4 个校验测试通过
2. **分段 2/4：明确错误码，目标缓冲不被污染** —— `demo_on` 演示零向量/奇异矩阵返回错误码
3. **分段 3/4：开关关闭，零性能损失（汇编逐条一致）** —— 普通版与校验版汇编 `diff` 无差异
4. **分段 4/4：正常输入零回归（20000 组逐位比对）** —— 开关开/关两种构建均 0 处位差异

## 重新生成

```bash
# 1. 录屏（tmux + asciinema，自动结束，不等待键盘输入）
bash output/recording/record.sh

# 2. 渲染 GIF（中文标题需要 CJK 字体；脚本使用 Noto Sans CJK SC 回退）
agg --text-font-family "DejaVu Sans Mono,Noto Sans CJK SC" \
    output/recording/validation_demo.cast \
    output/recording/validation_demo.gif
```

若环境缺少 CJK 字体，可安装到用户目录后刷新字体缓存：

```bash
mkdir -p ~/.local/share/fonts
curl -L -o ~/.local/share/fonts/NotoSansCJKsc-Regular.otf \
  https://github.com/notofonts/noto-cjk/raw/main/Sans/OTF/SimplifiedChinese/NotoSansCJKsc-Regular.otf
fc-cache -f ~/.local/share/fonts
```

录制在无 tmux 状态栏、非登录纯净 shell（`bash --noprofile --norc`）中进行，
不包含 MOTD 等环境噪音；段间停顿通过拼接时 1.5s 时间偏移实现。
