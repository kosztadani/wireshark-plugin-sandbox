#!/usr/bin/env bash

set -euo pipefail

WIRESHARK="${WIRESHARK:-wireshark}"

repository="$(readlink -f "$(dirname "${0}")")"
export WIRESHARK_CONFIG_DIR="${repository}/config"

exec "${WIRESHARK}" \
    -X lua_script:"${repository}/my-math.lua" \
    "$@"
