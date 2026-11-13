# cglm 输入校验（NaN 防护）交付说明

## 背景

cglm 的归一化与矩阵求逆在退化输入（零向量、奇异矩阵）下会产生 NaN/Inf，
并沿渲染管线一路污染。本次改动为这些关键运算增加**可编译期关闭**的输入
校验，失败时返回明确错误码而不是静默产出 NaN。

## 改动清单

| 文件 | 改动 |
| --- | --- |
| `include/cglm/validate.h` | 新增。`glm_validate_*` 校验入口与错误码枚举 |
| `include/cglm/cglm.h` | 引入 `validate.h`，随总头文件可用 |
| `CMakeLists.txt` | 新增 `CGLM_VALIDATE` 选项（默认 ON），OFF 时公开 `CGLM_VALIDATE_OFF` |
| `meson.build` / `meson_options.txt` | 新增同名 `validate` 选项 |
| `test/src/test_validate.h` | 新增 4 个测试，覆盖三类验收点 |
| `test/tests.h` / `test/src/tests.c` | 注册新测试 |

## 错误码

| 错误码 | 值 | 触发条件 |
| --- | --- | --- |
| `GLM_VALIDATE_OK` | 0 | 校验通过，目标被正常写入 |
| `GLM_VALIDATE_ZERO_VECTOR` | -1 | 向量/四元数范数小于 `FLT_EPSILON` |
| `GLM_VALIDATE_SINGULAR_MATRIX` | -2 | 行列式绝对值小于 `GLM_VALIDATE_DET_EPSILON`（默认 `GLM_FLT_EPSILON`） |

覆盖的入口：`vec2/vec3/vec4/quat` 的 `normalize` 与 `normalize_to`，
`mat2/mat3/mat4` 的 `inv`，以及 `mat4_inv_fast`。

失败时**目标缓冲区保持不变**，NaN 无法经由“输出”继续传播。

## 校验开关

- 默认开启，直接 `#include <cglm/cglm.h>` 即可使用。
- 编译期关闭：编译命令加 `-DCGLM_VALIDATE_OFF`，或 CMake 配置
  `-DCGLM_VALIDATE=OFF`（meson：`-Dvalidate=false`）。
- 关闭后所有 `glm_validate_*` 退化为对原函数的单行 `always_inline` 转发，
  生成的机器指令与直接调用原 API 完全一致，零性能损失。

## 验收与复现

运行（无需键盘交互）：

```bash
bash output/verify/run_verification.sh
```

脚本依次证明：

1. **退化输入可检出**：仓库自带测试入口（CMake `tests`）中
   `validate_zero_vector_detected`、`validate_singular_matrix_detected`、
   `validate_dest_untouched_on_failure` 全部通过（共 1141 个测试全过）。
2. **开关关闭零性能损失**：同一函数的普通版与校验版在
   `CGLM_VALIDATE_OFF` 下编译出的汇编指令逐条相同（见
   `verify/artifacts/*.norm`）。
3. **正常输入零回归**：`validate_normal_input_bitexact` 测试加
   `verify/bitexact_check.c` 对 20000 组随机合法输入做 `memcmp`，
   开启/关闭两种构建下均 0 处位差异。

`verify/demo.c` 是自造的小型演示素材，直观展示错误码与受保护的渲染变换；
完整输出保存在 `verify/verification.log`。
