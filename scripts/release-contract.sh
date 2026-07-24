#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_root"

fail() {
  printf 'release contract failed: %s\n' "$*" >&2
  exit 1
}

require_line() {
  local file="$1"
  local expected="$2"

  grep -Fqx -- "$expected" "$file" || fail "missing '${expected}' in ${file}"
}

metadata="metadata.json"
plugin_header="src/LEZWalletPlugin.h"
changelog="CHANGELOG.md"
workflow=".github/workflows/release.yml"

[[ -f "$metadata" ]] || fail "missing ${metadata}"
[[ -f "$plugin_header" ]] || fail "missing ${plugin_header}"
[[ -f "$changelog" ]] || fail "missing ${changelog}"
[[ -f "$workflow" ]] || fail "missing ${workflow}"

module_name="$(jq -er '.name' "$metadata")"
version="$(jq -er '.version' "$metadata")"
module_type="$(jq -er '.type' "$metadata")"
plugin_name="$(jq -er '.main' "$metadata")"

[[ "$module_name" == "lez_wallet_ui" ]] || fail "unexpected package name '${module_name}'"
[[ "$module_type" == "ui_qml" ]] || fail "unexpected package type '${module_type}'"
[[ "$plugin_name" == "lez_wallet_ui_plugin" ]] || fail "unexpected plugin name '${plugin_name}'"
[[ "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+([-.][0-9A-Za-z.-]+)?$ ]] || fail "invalid version '${version}'"

require_line "$plugin_header" "    QString version() const override { return \"${version}\"; }"
grep -Fq -- "## [${version}]" "$changelog" || fail "missing changelog entry for ${version}"

require_line "$workflow" "    uses: 3esmit/logos-modules-release-action/.github/workflows/release.yml@7e09ddf90bf33ce29ef3505609332175a6cd06c9"
require_line "$workflow" "      module_path: ."
require_line "$workflow" "      metadata_path: metadata.json"
require_line "$workflow" "      build_attr: lgx-portable"
require_line "$workflow" "      variants: linux-amd64,darwin-arm64"
require_line "$workflow" "      require_all_variants: true"
require_line "$workflow" "      dispatch_rebuild_index: false"
require_line "$workflow" "      prerelease: true"
require_line "$workflow" "      signing_mode: none"
grep -Fq -- 'releases must be dispatched from master' "$workflow" || fail "release branch guard is missing"

printf 'release contract valid for %s@%s\n' "$module_name" "$version"
