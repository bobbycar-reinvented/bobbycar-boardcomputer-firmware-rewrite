#!/bin/bash

if [[ -z "$IDF_PATH" ]]
then
    source export.sh --skip-source-check
fi

QTC_CLANG_CMD_OPTIONS_BLACKLIST=-mlongcalls\;-std=gnu++23 qtcreator "bobbycar-boardcomputer-firmware" 2>&1 >/dev/null &
