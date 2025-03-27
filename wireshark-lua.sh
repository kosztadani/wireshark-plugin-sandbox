#!/usr/bin/env bash

set -euo pipefail

repository="$(readlink -f "$(dirname "${0}")")"
export WIRESHARK_CONFIG_DIR="${repository}/config"

exec wireshark \
    -X lua_script:"${repository}/my-math.lua" \
    "$@"
