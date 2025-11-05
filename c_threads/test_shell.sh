#!/bin/bash
PROJECT_ROOT=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

PROGRAM="${PROJECT_ROOT}/install/usr/bin/test_app"
LIB_PATH="${PROJECT_ROOT}/install/usr/lib"
LOGFILE="${PROJECT_ROOT}/test_run.log"

TOTAL_RUNS=10000
MAX_MSG_ID=10000

> "${LOGFILE}"

echo "Starting ${TOTAL_RUNS} runs... Logging to ${LOGFILE}"

for ((run = 1; run <= TOTAL_RUNS; run++)); do
    ARG_COUNT=$((RANDOM % 50 + 1))
    ARGS=()

    for ((i = 0; i < ARG_COUNT; i++)); do
        MSG=$((RANDOM % MAX_MSG_ID + 1))
        ARGS+=("${MSG}")
    done

    CMD="LD_LIBRARY_PATH=\"${LIB_PATH}\" ${PROGRAM} ${ARGS[*]}"

    echo "[Run ${run}] CMD: ${CMD}" >> "${LOGFILE}"

    eval ${CMD} >> "${LOGFILE}" 2>&1
done

echo "Test complete. Results are in ${LOGFILE}"

unset TOTAL_RUNS
unset MAX_MSG_ID
unset PROGRAM
unset LIB_PATH
unset LOGFILE