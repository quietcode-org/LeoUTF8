# LEOUTF8-FIND-0002: HFS+ filename normalization on Leopard/PPC

## Summary

On Mac OS X 10.5.8 PowerPC, an HFS+ filename created through POSIX using an NFC UTF-8 spelling is observed through `readdir()` as a decomposed UTF-8 byte sequence.

The observed decomposed byte sequence matches `LeoUTF8NormalizeNFDBytes()` for the original NFC input.

This confirms that HFS+ filename handling is a real UTF-8 boundary concern for LeoUTF8.

## Host

- System: Mac OS X 10.5.8 Leopard
- Architecture: PowerPC
- Filesystem reported by `statfs`: `hfs`
- Probe target: `build-work/hfsplus-filename-probe`

## Probe

Probe source:

```text
Research/HFSPlus/hfsplus_filename_probe.c
````

Run with:

```sh
make hfscheck
```

## Test Input

### NFC filename

Display form:

```text
café.txt
```

UTF-8 bytes:

```text
63 61 66 C3 A9 2E 74 78 74
```

### NFD filename

Display form:

```text
café.txt
```

UTF-8 bytes:

```text
63 61 66 65 CC 81 2E 74 78 74
```

### LeoUTF8 expected NFD bytes

`LeoUTF8NormalizeNFDBytes()` produced:

```text
63 61 66 65 CC 81 2E 74 78 74
```

## Observed Behavior

The probe first created the NFC spelling.

Then it attempted to create the NFD spelling with `O_EXCL`.

Result:

```text
NFD O_EXCL create result: EEXIST
```

This means the filesystem treated the NFD spelling as the same existing filename.

Directory enumeration returned:

```text
entry: café.txt
entry bytes: 63 61 66 65 CC 81 2E 74 78 74
```

Summary:

```text
NFD O_EXCL matched existing file: yes
readdir returned NFC bytes: no
readdir returned LeoUTF8 NFD bytes: yes
```

## Interpretation

On this Leopard/PPC HFS+ volume:

- Creating a filename with NFC UTF-8 input succeeds.
    
- Creating the canonically equivalent NFD spelling with `O_EXCL` fails with `EEXIST`.
    
- `readdir()` returns decomposed UTF-8 bytes.
    
- The returned bytes match LeoUTF8's NFD normalization result.
    

This confirms that HFS+ filename handling must not be treated as plain byte-stable UTF-8.

## Architectural Consequence

LeoUTF8 should treat HFS+ filenames as a distinct boundary.

Recommended model:

```text
external/user UTF-8 input
  -> validate with LeoUTF8Core
  -> normalize explicitly when comparing or mapping filesystem names
  -> treat HFS+ observed POSIX names as decomposed UTF-8
  -> use CFString/NSString for display, not raw byte assumptions
```

## Policy Implication

LeoUTF8 should not silently normalize all strings.

However, for future HFS+-specific helpers, an explicit filename normalization path is justified.

Possible future API direction:

```text
LeoUTF8HFSNormalizeName(...)
LeoUTF8HFSCompareNames(...)
LeoUTF8HFSCreateDisplayString(...)
```

These should remain explicit and HFS+-scoped.

## Non-Goals

This finding does not define a final HFS+ filename API.

It does not yet cover:

- APFS
    
- UFS
    
- case-sensitive HFS+
    
- remote filesystems
    
- SMB/AFP behavior
    
- Finder-specific display behavior
    
- CFURL behavior
    
- Unicode exceptions specific to Apple's HFS+ decomposition tables
    

## Status

Accepted as the first HFS+ filename normalization finding for LeoUTF8 V0.3 research.  

