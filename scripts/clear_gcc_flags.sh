#!/usr/bin/env bash

# This script is designed to clear -L and -isystem flags
# from env vars NIX_LDFLAGS and NIX_CFLAGS_COMPILE

read FLAGS

FLAGS_ARRAY=($FLAGS)

CLEAR_FLAGS=()

startswith() { case $2 in "$1"*) true ;; *) false ;; esac }

PREV_WAS_ISYSTEM=false

for FLAG in "${FLAGS_ARRAY[@]}"; do
    :
    # If flag starts with "-L", skip it
    if startswith -L "$FLAG"; then
        continue
    fi

    # If flag equals to "-isystem", skip it and skip next one too
    if [[ "$FLAG" == "-isystem" ]]; then
        PREV_WAS_ISYSTEM=true
        continue
    fi

    # If previous flag was "-isystem", skip it
    if [ "$PREV_WAS_ISYSTEM" = true ]; then
        PREV_WAS_ISYSTEM=false
        continue
    fi

    # If flag passed all previous checks, add it
    CLEAR_FLAGS+=($FLAG)
done

echo "${CLEAR_FLAGS[@]}"
