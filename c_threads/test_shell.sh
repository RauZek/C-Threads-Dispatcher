#!/bin/bash

PROGRAM = "./test_assignment"
TOTAL_RUNS = 1000
MAX_ARGS = 100000
MAX_MSG_ID = 100000

for ((run = 1; run <= TOTAL_RUNS; run++)); do
    echo "Run $run of ${TOTAL_RUNS}"
    ARG_COUNT =$((RANDOM % 50 + 1))
    ARGS = ()

    for ((i = 0; i < ARG_COUNT; i++)); do
        MSG = $((RANDOM % MAX_MSG_ID + 1))
        ARGS += ("${MSG}")
    done

    CMD = "${PROGRAM} ${ARGS[*]}"
    echo "[Run ${run}] CMD: ${CMD}" >> "${LOGFILE}"
    ${CMD} >> "${LOGFILE}" 2>&1
done

unset TOTAL_RUNS
unset MAX_ARGS
unset MAX_MSG_ID
