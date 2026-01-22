#!/bin/sh

CODE_BASE_PATH=$(pwd)
CODE_BASE_PATH=${CODE_BASE_PATH%/setup}
export CODE_BASE_PATH

printf 'Root of code base has been specified as:\n    %s\n' "$CODE_BASE_PATH"
