#!/usr/bin/env bash
set -euo pipefail

# ================= 1. 终端颜色与输出格式定义 =================
if [[ -t 1 ]]; then
    RED='\033[1;31m'
    GREEN='\033[1;32m'
    BLUE='\033[1;34m'
    CYAN='\033[1;36m'
    YELLOW='\033[1;33m'
    BOLD='\033[1m'
    NC='\033[0m'
else
    RED=''; GREEN=''; BLUE=''; CYAN=''; YELLOW=''; BOLD=''; NC=''
fi

info()  { printf "${BLUE}==>${NC} ${BOLD}%s${NC}\n" "$1"; }
ok()    { printf "${GREEN}==>${NC} ${BOLD}%s${NC}\n" "$1"; }
warn()  { printf "${YELLOW}Warning:${NC} %s\n" "$1" >&2; }
error() { printf "\n${RED}Error:${NC} %s\n" "$1" >&2; exit 1; }

# ================= 2. 临时目录与退出清理钩子 =================
# 使用当前进程 PID ($$) 命名临时目录，避免多终端并发冲突
TMP_DIR=".oj_tmp_$$"
cleanup() {
    rm -rf "$TMP_DIR"
}
# 无论脚本正常结束、报错退出还是被 Ctrl+C 中断，均自动清理临时文件
trap cleanup EXIT

# ================= 3. 默认参数与帮助信息 =================
TOTAL=""
SPLITS=""
GEN_SRC="gen.cpp"
STD_SRC="std.cpp"
OUT_DIR="data"

show_help() {
    cat << EOF
用法:
  ./ojgen.sh -n <总组数> [-s <梯度占比>] [-g <生成器.cpp>] [-a <标程.cpp>] [-o <输出目录>]
  ./ojgen.sh <总组数> [梯度占比]   (简写位置参数模式)

参数说明:
  -n, --num     必填，生成测试数据的总组数（正整数，如 20）
  -s, --split   可选，N-1 个梯度占比，英文逗号分隔（如 "0.1,0.4" 表示 3 个梯度: 10%, 40%, 50%）
                若省略此参数，则所有数据均归入 Tag 1
  -g, --gen     可选，数据生成器源码路径（默认: gen.cpp）
  -a, --std     可选，标准答案程序源码路径（默认: std.cpp）
  -o, --out     可选，测试数据输出目录（默认: data）
  -h, --help    显示本帮助信息

示例:
  ./ojgen.sh -n 20 -s 0.1,0.4
  ./ojgen.sh 15 0.2,0.3
EOF
}

# ================= 4. 解析命令行参数 =================
if [[ $# -eq 0 ]]; then
    show_help
    exit 1
fi

# 支持简写位置参数模式：./ojgen.sh 20 0.1,0.4
if [[ "$1" != -* ]]; then
    TOTAL="$1"
    SPLITS="${2:-}"
    if [[ $# -gt 2 ]]; then
        error "位置参数过多。正确简写用法：./ojgen.sh <总组数> [梯度占比]"
    fi
else
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -n|--num)
                [[ $# -lt 2 ]] && error "参数 $1 缺少具体的数值。"
                TOTAL="$2"
                shift 2
                ;;
            -s|--split)
                [[ $# -lt 2 ]] && error "参数 $1 缺少梯度比例字符串。"
                SPLITS="$2"
                shift 2
                ;;
            -g|--gen)
                [[ $# -lt 2 ]] && error "参数 $1 缺少文件路径。"
                GEN_SRC="$2"
                shift 2
                ;;
            -a|--std)
                [[ $# -lt 2 ]] && error "参数 $1 缺少文件路径。"
                STD_SRC="$2"
                shift 2
                ;;
            -o|--out)
                [[ $# -lt 2 ]] && error "参数 $1 缺少目录路径。"
                OUT_DIR="$2"
                shift 2
                ;;
            -h|--help)
                show_help
                exit 0
                ;;
            *)
                error "未知参数: $1（运行 ./ojgen.sh -h 查看帮助）"
                ;;
        esac
    done
fi

# ================= 5. 参数合法性与环境前置检查 =================
[[ -z "$TOTAL" ]] && error "未指定总数据组数！请使用 -n <组数>（例如：./ojgen.sh -n 20 -s 0.1,0.4）"
[[ "$TOTAL" =~ ^[1-9][0-9]*$ ]] || error "总数据组数必须为正整数，当前输入为: '$TOTAL'"

[[ -f "$GEN_SRC" ]] || error "未找到数据生成器源码：$GEN_SRC（请确认同级目录下存在该文件）"
[[ -f "$STD_SRC" ]] || error "未找到标程源码：$STD_SRC（请确认同级目录下存在该文件）"
command -v g++ >/dev/null 2>&1 || error "系统未安装 g++ 编译器，请先配置 C++ 编译环境。"

# ================= 6. 前缀和计算各梯度区间右边界 =================
# awk 完成：格式校验 -> 小数乘法下取整(+1e-9修正浮点误差) -> 累加前缀和 -> 末尾锚定 TOTAL
PREFIX_BOUNDS=$(awk -v total="$TOTAL" -v splits="$SPLITS" '
BEGIN {
    if (splits == "") {
        print total
        exit 0
    }
    n = split(splits, r, ",")
    sum = 0
    for (i = 1; i <= n; i++) {
        if (r[i] !~ /^([0-9]+(\.[0-9]+)?|\.[0-9]+)$/) {
            exit 2
        }
        val = r[i] + 0
        if (val < 0 || val > 1) {
            exit 2
        }
        cnt = int(total * val + 1e-9)
        sum += cnt
        printf "%d ", sum
    }
    if (sum > total) {
        exit 3
    }
    printf "%d\n", total
}') || {
    rc=$?
    if [[ $rc -eq 2 ]]; then
        error "梯度比例格式非法：'$SPLITS'（必须是 0 到 1 之间的小数，并用英文逗号分隔，如 0.1,0.4）"
    elif [[ $rc -eq 3 ]]; then
        error "传入的梯度比例总和超过了 1.0：'$SPLITS'，无法分配 $TOTAL 组数据。"
    else
        error "解析梯度比例时发生未知错误。"
    fi
}

# 构建前缀和数组 pre（索引 0 放置哨兵 0，索引 1..N 对应各梯度的右边界下标）
pre=(0 $PREFIX_BOUNDS)
NUM_TAGS=$((${#pre[@]} - 1))

# ================= 7. 编译 C++ 源码 =================
mkdir -p "$TMP_DIR"
info "正在编译 $GEN_SRC 和 $STD_SRC ..."

if ! g++ -O2 -std=c++17 "$GEN_SRC" -o "$TMP_DIR/gen"; then
    error "$GEN_SRC 编译失败，请根据上方编译器报错修改代码。"
fi

if ! g++ -O2 -std=c++17 "$STD_SRC" -o "$TMP_DIR/std"; then
    error "$STD_SRC 编译失败，请根据上方编译器报错修改代码。"
fi

# ================= 8. 展示梯度分配计划并准备输出目录 =================
info "梯度区间分配结果（共 $TOTAL 组数据，划分为 $NUM_TAGS 个梯度）："
for ((t = 1; t <= NUM_TAGS; t++)); do
    left=$((pre[t - 1] + 1))
    right=${pre[$t]}
    count=$((right - left + 1))
    if [[ $count -le 0 ]]; then
        printf "    ${YELLOW}Tag %d${NC}: 0 组（占比过小下取整为 0，已跳过）\n" "$t"
    else
        printf "    ${CYAN}Tag %d${NC}: data%03d ~ data%03d （共 %d 组）\n" "$t" "$left" "$right" "$count"
    fi
done

mkdir -p "$OUT_DIR"
# 清理输出目录中可能残留的旧数据，防止组数缩减时遗留脏数据
rm -f "$OUT_DIR"/data*.in "$OUT_DIR"/data*.out

# ================= 9. Homebrew 风格动态进度条渲染函数 =================
draw_progress() {
    local cur=$1
    local tot=$2
    local tag=$3
    local name=$4
    local width=30

    local filled=$((cur * width / tot))
    local empty=$((width - filled))
    local pct_int=$((cur * 100 / tot))
    local pct_dec=$(((cur * 1000 / tot) % 10))

    local bar_fill=""
    local bar_empty=""
    [[ $filled -gt 0 ]] && printf -v bar_fill "%${filled}s" "" && bar_fill=${bar_fill// /#}
    [[ $empty -gt 0 ]]  && printf -v bar_empty "%${empty}s" "" && bar_empty=${bar_empty// /-}

    printf "\r\033[K${CYAN}[${GREEN}%s${NC}%s${CYAN}]${NC} %3d.%d%% (%d/%d) | Tag %d -> %s.in/.out" \
        "$bar_fill" "$bar_empty" "$pct_int" "$pct_dec" "$cur" "$tot" "$tag" "$name"
}

# ================= 10. 主循环：单调指针定标号 + 重定向生成数据 =================
info "开始生成测试数据至 $OUT_DIR/ 目录..."
tag=1

for ((i = 1; i <= TOTAL; i++)); do
    # 单调指针推进：当下标 i 越过当前梯度的前缀和右边界时，切换到下一个梯度标号
    while [[ $i -gt ${pre[$tag]} ]]; do
        tag=$((tag + 1))
    done

    file_prefix=$(printf "data%03d" "$i")
    in_file="$OUT_DIR/${file_prefix}.in"
    out_file="$OUT_DIR/${file_prefix}.out"

    # 运行生成器（传入：梯度标号 tag、全局测试点编号 i 作为随机种子）
    if ! "$TMP_DIR/gen" "$tag" "$i" > "$in_file" 2> "$TMP_DIR/err.log"; then
        err_msg=$(cat "$TMP_DIR/err.log")
        error "生成器 gen 运行异常（测试点 ${file_prefix}，Tag ${tag}）：\n${err_msg}"
    fi

    if [[ ! -s "$in_file" ]]; then
        printf "\n"
        warn "测试点 ${file_prefix}.in 生成内容为空，请检查 gen.cpp 是否有标准输出。"
    fi

    # 运行标程（标准输入重定向自 .in，标准输出重定向至 .out）
    if ! "$TMP_DIR/std" < "$in_file" > "$out_file" 2> "$TMP_DIR/err.log"; then
        err_msg=$(cat "$TMP_DIR/err.log")
        error "标程 std 运行异常（测试点 ${file_prefix}，Tag ${tag}）：\n${err_msg}"
    fi

    draw_progress "$i" "$TOTAL" "$tag" "$file_prefix"
done

printf "\n"
ok "全部完成！共生成 $TOTAL 组数据，已保存至 ./$OUT_DIR/ 目录。"
