#!/usr/bin/env bash

# Environment variables from h-manifest.conf and custom variables from flight sheet are not available here

. h-manifest.conf

[[ `ps aux | grep "cpuminer_scash_bin" | grep -v grep | wc -l` != 0 ]] &&
  echo -e "${RED}$CUSTOM_NAME miner is already running${NOCOLOR}" &&
  exit 1

unset LD_LIBRARY_PATH

./cpuminer_scash_bin $(cat $CUSTOM_CONFIG_FILENAME) 2>&1 | tee --append $CUSTOM_LOG_BASENAME.log
