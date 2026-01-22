#!/usr/bin/env bash
# Source this file to set up smart_run environment variables.

if [[ "${BASH_SOURCE[0]}" == "$0" ]]; then
  echo "Please source this file: . ./env.sh" >&2
  exit 1
fi

SMART_RUN_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [[ -z "${TOOL_EXTENSION}" ]]; then
  if [[ -d "/download/Xuantie-900-gcc-elf-newlib-x86_64-V2.0.3/bin" ]]; then
    export TOOL_EXTENSION="/download/Xuantie-900-gcc-elf-newlib-x86_64-V2.0.3/bin"
  fi
fi

if [[ -z "${CODE_BASE_PATH}" ]]; then
  export CODE_BASE_PATH="${SMART_RUN_DIR}/../C910_RTL_FACTORY"
fi

if [[ -z "${CCACHE_DIR}" ]]; then
  export CCACHE_DIR="${SMART_RUN_DIR}/work/.ccache"
fi

if [[ -n "${TOOL_EXTENSION}" ]]; then
  case ":${PATH}:" in
    *":${TOOL_EXTENSION}:"*) ;;
    *) export PATH="${TOOL_EXTENSION}:${PATH}" ;;
  esac
fi

if [[ -z "${TOOL_EXTENSION}" ]]; then
  echo "Warning: TOOL_EXTENSION is unset; set it to your riscv64 toolchain bin path." >&2
elif [[ ! -d "${TOOL_EXTENSION}" ]]; then
  echo "Warning: TOOL_EXTENSION directory not found: ${TOOL_EXTENSION}" >&2
fi

if [[ ! -d "${CODE_BASE_PATH}" ]]; then
  echo "Warning: CODE_BASE_PATH directory not found: ${CODE_BASE_PATH}" >&2
fi
