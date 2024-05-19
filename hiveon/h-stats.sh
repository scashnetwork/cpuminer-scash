#!/usr/bin/env bash

# Parse log file to get khs and other stats
# [2024-05-08 19:37:24] accepted: 1/1 (100.00%), 1.23 khash/s (yay!!!)

. `dirname $BASH_SOURCE`/h-manifest.conf

accepted_line=$(cat $CUSTOM_LOG_BASENAME.log | grep accepted | tail -1)

khs=$(echo $accepted_line | awk '{print $6}')

acceptratio=$(echo $accepted_line | awk '{print $4}')
accepted=$(echo $acceptratio | awk -F'/' '{print $1}')
submitted=$(echo $acceptratio | awk -F'/' '{print $2}')
rejected=$(($submitted-$accepted))

stats=$( jq -n \
            --arg v "$CUSTOM_VERSION" \
            --arg a "randomx" \
            '{ ver: $v, algo: $a, ar:['$accepted', '$rejected'] }' )

[[ -z $khs ]] && khs=0
[[ -z $stats ]] && stats="null"
