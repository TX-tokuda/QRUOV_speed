#!/usr/bin/env bash
set -u

CMD="./src/qruov"
N=100

if [[ ! -x "$CMD" ]]; then
  echo "Error: '$CMD' が見つからないか実行権限がありません" >&2
  exit 1
fi

# 開始時刻（ナノ秒）
start_ns=$(date +%s%N)

for _ in $(seq 1 "$N"); do
  $CMD >/dev/null 2>&1
done

end_ns=$(date +%s%N)

elapsed_ns=$((end_ns - start_ns))

# 表示用変換
elapsed_ms=$(awk "BEGIN {printf \"%.3f\", $elapsed_ns/1000000}")
avg_us=$(awk "BEGIN {printf \"%.3f\", $elapsed_ns/$N/1000}")

echo "Total time : ${elapsed_ms} ms"
echo "Avg / run  : ${avg_us} µs"
