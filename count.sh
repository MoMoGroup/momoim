#!/bin/bash
echo '用户名             添加    删除    有效'
for i in wangwenmin xia xuan lh WardenMiao
do
    git log --author="$i" --pretty=tformat: --numstat | gawk '{ add += $1; subs += $2; loc += $1 - $2 } END { printf "%15s%8s%8s%8s\n","'$i'", add, subs, loc }' -
done
e