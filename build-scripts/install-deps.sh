#!/bin/bash

cmds=()

# Detect what dependencies are missing.
for cmd in autoconf automake libtool pkg-config; do
    if ! command -v $cmd &>/dev/null; then
        cmds+=("$cmd")
    fi
done

# Install missing dependencies
if [ ${#cmds[@]} -ne 0 ]; then
    sed -i 's/http:/https:/g' /etc/apt/sources.list
    apt-get update
    apt-get install -y ${cmds[@]}
fi
