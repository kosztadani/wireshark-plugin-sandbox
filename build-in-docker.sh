#!/usr/bin/env bash

set -euo pipefail

repository="$(readlink -f "$(dirname "${0}")")"

tag="wireshark-plugin-sandbox"

docker build \
    --file docker/Dockerfile \
    --tag "${tag}" \
    "${repository}"

mkdir -p config/my-plugins
rm -rf config/my-plugins/*

docker run \
    --rm \
    "${tag}" \
    tar -C /wireshark-plugin . -cf - |
tar -C "${repository}/config/my-plugins" -xf -