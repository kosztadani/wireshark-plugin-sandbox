#!/usr/bin/env bash

set -euo pipefail

repository="$(readlink -f "$(dirname "${0}")")"
export WIRESHARK_CONFIG_DIR="${repository}/config"
export WIRESHARK_PLUGIN_DIR="${repository}/config/my-plugins"

STORED_DUMPS="${repository}/example-dumps"
TEST_DUMPS="${repository}/test-dumps"
PROTO="my-math"
EXAMPLES=("${repository}/examples/"*".pcapng")

TSHARK_COMMON_OPTIONS=(
    -d "tcp.port==9999,${PROTO}"
    -O "${PROTO}"
    -Y "${PROTO}"
    -J "${PROTO}"
)

function main() {
    rm -rf "${TEST_DUMPS}"
    mkdir -p "${TEST_DUMPS}/"{native,lua}
    local capture
    local failures=0
    for capture in "${EXAMPLES[@]}"; do
        local name="$(basename "${capture/.pcapng/}")"
        dump_with_lua "${capture}" text >"${TEST_DUMPS}/lua/${name}.txt"
        dump_with_lua "${capture}" json >"${TEST_DUMPS}/lua/${name}.json"
        compare_dumps "${name}" "lua" || failures=$((failures + $?))
        dump_with_native "${capture}" text >"${TEST_DUMPS}/native/${name}.txt"
        dump_with_native "${capture}" json >"${TEST_DUMPS}/native/${name}.json"
        compare_dumps "${name}" "native" || failures=$((failures + $?))
    done
    printf "\n"
    if [[ "${failures}" -eq 0 ]]; then
        printf "Overall result: PASS\n"
        exit 0
    else
        printf "Overall result: FAIL (%i)\n" "${failures}"
        exit 1
    fi
}

function dump_with_lua() {
    local capture="${1}"
    local format="${2}"
    export -n WIRESHARK_PLUGIN_DIR
    tshark \
        "${TSHARK_COMMON_OPTIONS[@]}" \
        -X lua_script:"${repository}/my-math.lua" \
        -r "${capture}" \
        -T "${format}"
}

function dump_with_native() {
    local capture="${1}"
    local format="${2}"
    export WIRESHARK_PLUGIN_DIR
    tshark \
        "${TSHARK_COMMON_OPTIONS[@]}" \
        -r "${capture}" \
        -T "${format}"
}

function compare_dumps() {
    local name="${1}"
    local plugin_type="${2}" # "lua" or "native"
    local failures=0
    compare_dump "${name}.txt" "${plugin_type}" || failures=$((failures + 1))
    compare_dump "${name}.json" "${plugin_type}" || failures=$((failures + 1))
    return "${failures}"
}

function compare_dump() {
    local filename="${1}"
    local plugin_type="${2}" # "lua" or "native"
    local stored_dump="${STORED_DUMPS}/${plugin_type}/${filename}"
    local test_dump="${TEST_DUMPS}/${plugin_type}/${filename}"
    local comparison=0
    compare_files "${stored_dump}" "${test_dump}" || comparison=$?
    if [[ "${comparison}" -eq 0 ]]; then
        printf "TEST: %s (%s) ... PASS\n" "${filename}" "${plugin_type}"
    else
        printf "TEST: %s (%s) ... FAIL\n" "${filename}" "${plugin_type}"
    fi
    return "${comparison}"
}

function compare_files() {
    local stored_dump="${1}"
    local test_dump="${2}"
    local comparison=0
    diff -q "${stored_dump}" "${test_dump}" >/dev/null || comparison=$?
    if [[ ${comparison} -eq 0 ]]; then
        return 0
    else
        return 1
    fi
}

main
