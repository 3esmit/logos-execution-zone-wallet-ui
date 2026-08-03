#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
"$repo_root/scripts/release-contract.sh"

popup="$repo_root/src/qml/popups/InitializePublicAccountDialog.qml"
if ! grep -Fq 'width: Math.max(0, Math.min(560,' "$popup"; then
    echo "expected a non-negative width clamp in $popup" >&2
    exit 1
fi
