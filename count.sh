#!/bin/bash
echo '用户名             添加    删除'
for i in wangwenmin xia lh WardenMiao
do
    git log --author="$i" --pretty=tformat: --numstat | gawk '{ add += $1; subs += $2; } END { printf "%15s%8s%8s\n","'$i'", add, subs }' -
done
lc=$(($(wc -l server/include/sqlite3.h|cut -d\  -f1)+$(wc -l server/sqlite3.c|cut -d\  -f1)))
git log --author="xuan" --pretty=tformat: --numstat | gawk '{ add += $1; subs += $2; } END { printf "%15s%8s%8s\n","xuan", add-'$lc', subs }' -
