#!/usr/bin/env bash

# Environment variables from h-manifest.conf and custom variables from flight sheet are available here

conf="--url $CUSTOM_URL --user $CUSTOM_TEMPLATE --pass $CUSTOM_PASS  $CUSTOM_USER_CONFIG"

mkfile_from_symlink $CUSTOM_CONFIG_FILENAME

echo $conf > $CUSTOM_CONFIG_FILENAME
