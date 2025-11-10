#!/bin/bash

ASSIGNMENT_C_THREADS_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

function assignmentreads-clean(){
    rm -rf ${ASSIGNMENT_C_THREADS_DIR}/build
    rm -rf ${ASSIGNMENT_C_THREADS_DIR}/install
    echo "Clean Assignment."
}

function assignmentreads-configure() {
    mkdir -p ${ASSIGNMENT_C_THREADS_DIR}/build
    (
        cd ${ASSIGNMENT_C_THREADS_DIR}/build

        cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ${ASSIGNMENT_C_THREADS_DIR}/src && \
            echo "Assignment Configured!"
    )
}

function assignmentreads-compile () {
    (
        cd ${ASSIGNMENT_C_THREADS_DIR}/build

        cmake --build . && \
            echo "Assignment Compiled!"
    )
}

function assignmentreads-install() {
    (
        cd ${ASSIGNMENT_C_THREADS_DIR}/build

        cmake --install . --prefix ${ASSIGNMENT_C_THREADS_DIR}/install/usr && \
            echo "Assignment Installed Successfully!"
    )
}

function assignmentreads-install-debug() {
    (
        cd ${ASSIGNMENT_C_THREADS_DIR}/build
        cmake --install . --strip --prefix ${ASSIGNMENT_C_THREADS_DIR}/debug/usr && \
            echo "Assignment Debug Version Installed!"
    )
}


function assignmentreads-install-release() {
    (
        cd ${ASSIGNMENT_C_THREADS_DIR}/build
        cmake --install . --strip --prefix ${ASSIGNMENT_C_THREADS_DIR}/install/usr && \
            echo "Assignment Release Version Installed!"
    )
}

function assignmentreads-incremental-build() {
    assignmentreads-configure && assignmentreads-compile && assignmentreads-install-release
    echo "You Choose the Incremental Build Installation of the Assignment!"
}

function assignmentreads-clean-build() {
    assignmentreads-clean && assignmentreads-incremental-build
    echo "You Choose the Clean Build Installation of the Assignment"
}

function test-assignment () {
    (
        LD_LIBRARY_PATH="${ASSIGNMENT_C_THREADS_DIR}/install/usr/lib" \
        "${ASSIGNMENT_C_THREADS_DIR}/install/usr/bin/test_app" $@
    )
}

echo "-------------Cmake----------------"
echo "command: assignmentreads-incremental-build"
echo    "To build the project's build."
echo "----------------------------------"
echo "command: assignmentreads-clean-build"
echo    "To clean the project's build."
echo "----------------------------------"
echo "command: assignmentreads-install-debug"
echo    "To install debugging's version."
echo "----------------------------------"
echo "command: test-assignment"
echo    "Runs the .exe file."
echo "----------------------------------"
