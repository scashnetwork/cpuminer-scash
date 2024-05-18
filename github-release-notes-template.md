## Notes

Users are advised to build from source. Binaries are provided only for convenience and have been compiled with support for x64 AVX2 extensions.
- Linux (arm64, x64)
  - Static binary.
- Windows (x64)
  - Supports Windows 10 or later.
- MacOS (arm64)
  - Users may need to run the command `xattr -r -d com.apple.quarantine minerd` to launch the binary.

## Verify checksums

It is recommended that users verify the SHA256 checksum of downloaded files before opening them.
- Linux: `sha256sum <filename>`
- Mac: `shasum -a 256 <filename>`
- Windows: `certutil -hashfile <filename> SHA256`

```
$RELEASE_SHA256SUMS
```
