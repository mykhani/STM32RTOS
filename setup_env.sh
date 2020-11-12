#!/bin/bash

export BUILDER_ROOT=$(cd "$(dirname -- ${BASH_SOURCE[0]})" && pwd)
export PATH=${BUILDER_ROOT}/tools/gcc-arm-none-eabi-9-2019-q4-major/bin:$PATH

echo "Builder root: ${BUILDER_ROOT}"
