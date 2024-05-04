#!/usr/bin/env bash

# Copyright (C) 2023 Gramine contributors
# SPDX-License-Identifier: BSD-3-Clause

set -e

if test -n "$SGX"
then
    GRAMINE=gramine-sgx
else
    # GRAMINE=gramine-direct
    GRAMINE=gramine-encos
fi

#
$GRAMINE mbedtls