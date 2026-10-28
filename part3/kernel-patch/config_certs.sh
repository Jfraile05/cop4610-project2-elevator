#!/bin/bash
# Usage: ./config_certs.sh /usr/src/linux-6.16.x
# Run after copying your running kernel config to .config and `make olddefconfig`.
# Disables the signing certificates that otherwise break custom kernel builds.
set -e
KDIR="${1:?usage: $0 <kernel source dir>}"
cd "$KDIR"
scripts/config --disable SYSTEM_TRUSTED_KEYS
scripts/config --disable SYSTEM_REVOCATION_KEYS
scripts/config --set-str CONFIG_SYSTEM_TRUSTED_KEYS ""
scripts/config --set-str CONFIG_SYSTEM_REVOCATION_KEYS ""
scripts/config --disable DEBUG_INFO_BTF
make olddefconfig
