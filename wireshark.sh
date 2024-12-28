#!/usr/bin/env bash

set -euo pipefail

repository="$(readlink -f "$(dirname "${0}")")"
export WIRESHARK_CONFIG_DIR="${repository}/config"
export WIRESHARK_PLUGIN_DIR="${repository}/config/plugins"

exec wireshark "$@"
