#!/bin/bash
csv_file="$1"
if [ ! -f "${csv_file}" ]; then
    echo "error: file ${csv_file} does not exist" >&2
    exit 1
fi
awk -F',' 'NR>1 && $4 ~ /^5/ {print $3}' "${csv_file}" \
| sort \
| uniq -c \
| sort -k1,1nr -k2,2 \
| head -n 2
awk -F',' 'NR>1 {sum += $5; cnt++} END {print sum, cnt}' "${csv_file}" \
| awk '{printf "%.2f\n", $1/$2}'

