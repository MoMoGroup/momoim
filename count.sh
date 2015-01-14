#!/bin/bash
git log --pretty=email | grep -P "^From:" | cut -f2 -d: | sort|uniq | while read name
do
echo -n "$name"
    git log --author="$name" --pretty=tformat: --numstat | grep -v sqlite |  gawk '{ add += $1; subs += $2; } END { printf ":添加%d,删除%d\n", add, subs }' -
done
