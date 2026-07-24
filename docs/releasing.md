# Release LEZ Wallet UI

This repository is the release authority for the `lez_wallet_ui` package. Do
not publish this package through a catalog repository or an Inspector release.

## Release contract

The source-owned `Publish LEZ Wallet UI` workflow builds the portable
`lgx-portable` output for exactly these variants:

- `linux-amd64`
- `darwin-arm64`

Both variants are required. A release is marked as a prerelease and contains:

- the merged `.lgx` package;
- `sidecar.json`, including the artifact manifest, checksum, and built
  variants.

The workflow does not dispatch catalog-index work. Catalogs may consume the
published package independently, but they are not part of this release path.

## Prepare a release

1. Update `metadata.json` `version`.
2. Update `src/LEZWalletPlugin.h` so `LEZWalletPlugin::version()` returns the
   same version.
3. Add a matching `## [version]` entry to `CHANGELOG.md`.
4. Run the local release checks:

   ```bash
   bash tests/release_contract_test.sh
   nix build -L --no-write-lock-file .#lgx-portable
   ```

5. Merge the release preparation into `master`.
6. In GitHub Actions, run `Publish LEZ Wallet UI` from `master`.

Use `force_build` only to repair or intentionally replace a release that
already has the same version. The normal path skips a release only after both
the LGX artifact and its sidecar metadata are present.

## Validation

`tests/release_contract_test.sh` verifies the package identity, version
agreement between metadata and runtime plugin, changelog entry, pinned shared
release workflow, strict platform set, prerelease setting, and disabled catalog
dispatch. CI runs this test before the portable package build.
