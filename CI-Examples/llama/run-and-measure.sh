#!/bin/bash
# run_and_measure.sh
# 用于启动目标脚本，并在整个生命周期内使用 pidstat 采样统计 CPU 使用情况，
# 最后估算累计使用的 CPU cycles。

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <target_script> [target_script arguments...]"
    exit 1
fi

TARGET_SCRIPT="$1"
shift

# 启动目标脚本，并获取其 PID
./"$TARGET_SCRIPT" "$@" &
TARGET_PID=$!
echo "Launched target script '$TARGET_SCRIPT' with PID: $TARGET_PID"

# 获取目标进程及其所有后代 PID 的函数
get_all_pids() {
    # 使用 pstree -p 获取以 TARGET_PID 为根的进程树，再用 grep 提取 PID
    pstree -p $TARGET_PID | grep -o '([0-9]\+)' | grep -o '[0-9]\+'
}

# 创建临时文件保存 pidstat 输出
tmpfile=$(mktemp)

# 循环采样，直到目标脚本退出
while kill -0 $TARGET_PID 2>/dev/null; do
    # 获取目标进程及其所有后代的 PID（包括多线程／子进程）
    ALL_PIDS=$(get_all_pids)
    # 形成逗号分隔的 PID 列表（如果仅有一个 PID，也没问题）
    PIDS_CSV=$(echo "$ALL_PIDS" | tr '\n' ',' | sed 's/,$//')
    
    # 使用 pidstat 采样 1 秒钟的数据
    # -t 显示线程信息，-p 指定 PID 列表，后面的 1 1 表示 1 秒采样一次，仅一轮采样
    pidstat -t -p "$PIDS_CSV" 1 1 >> "$tmpfile"
done

echo "Target script finished. Processing CPU usage data..."

# 解析临时文件，累计每次采样的 %CPU 值（单位：%·sec）
total_cpu_usage=0
num_samples=0

# pidstat 的输出格式类似如下：
# Time          PID    %usr %system %guest   %wait   %CPU  CPU  Command
# 12:00:01 AM  1234    0.50   0.20    0.00    0.00   0.70    0  your_program
#
# 本脚本跳过包含 "PID" 或 "Linux" 的头部行，假设数据行第一列为时间，%CPU 为第 7 列。
while read -r line; do
    # 跳过标题或非数据行
    if echo "$line" | grep -q -E "PID|Linux"; then
        continue
    fi
    # 判断是否以时间格式开头（例如 "12:00:01"）
    if echo "$line" | grep -qE '^[0-9]{1,2}:[0-9]{2}:[0-9]{2}'; then
        # 提取第7列（%CPU）
        cpu_val=$(echo "$line" | awk '{print $7}')
        # 判断该字段是否为数字
        if [[ $cpu_val =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
            total_cpu_usage=$(echo "$total_cpu_usage + $cpu_val" | bc)
            num_samples=$((num_samples + 1))
        fi
    fi
done < "$tmpfile"

echo "Total cumulative CPU usage: $total_cpu_usage (%·sec) over $num_samples samples."

# 估算 CPU cycles：
# 假设每次采样间隔 1 秒，则累计 CPU 时间（秒）约为 total_cpu_usage/100 （因为 100% 表示 1 CPU 全核 1 秒）
# 需要获取 CPU 主频（单位 MHz），再换算为 Hz；例如 2500 MHz 对应 2.5e9 cycles/sec
cpu_mhz=$(lscpu | grep "CPU MHz" | awk '{print $3}')
if [[ -n "$cpu_mhz" ]]; then
    cpu_hz=$(echo "$cpu_mhz * 1000000" | bc -l)
    # 估算累计 CPU 时间（秒）
    total_cpu_time=$(echo "scale=6; $total_cpu_usage / 100" | bc -l)
    # 估算 CPU cycles
    estimated_cycles=$(echo "scale=0; $total_cpu_time * $cpu_hz" | bc)
    echo "Estimated CPU cycles (approx): $estimated_cycles"
else
    echo "Could not determine CPU frequency."
fi

# 清理临时文件
rm "$tmpfile"
