# LeoUTF8 V0.7.3.1 Release Seal

This document seals the LeoUTF8 V0.7.3.1 LeoFuzzer integration baseline.

## Baseline Tag

    v0.7.3.1-leofuzzer-empty-input-fix

## Confirmed Platform

Confirmed on:

    Mac OS X 10.5.8 PowerPC

## LeoFuzzer Integration

LeoUTF8 now includes:

- LeoFuzzer validation probe
- valid UTF-8 corpus
- invalid UTF-8 corpus
- Makefile targets for LeoFuzzer integration
- report generation through LeoFuzzer
- TSV field validation through LeoFuzzer checks

## Confirmed Commands

    make leofuzz-check

## Confirmed Valid Corpus Result

    LEOFUZZ:SUMMARY target=build-work/leofuzz_validate_probe runs=5 ok=5 rejected=0 findings=0

## Confirmed Invalid Corpus Result

    LEOFUZZ:SUMMARY target=build-work/leofuzz_validate_probe runs=5 ok=0 rejected=5 findings=0

## Empty Input Fix

The first LeoFuzzer integration pass showed that `empty.txt` was reported as `DOMAIN_REJECT`.

This was caused by the probe passing:

    bytes = NULL
    length = 0

The probe now uses a non-NULL buffer with length 0, so an empty file is tested as a real zero-length byte sequence.

The result is now:

    empty.txt -> OK

## Semantics

For this validation probe:

- valid UTF-8 returns OK
- malformed UTF-8 returns DOMAIN_REJECT
- malformed UTF-8 is not a LeoFuzzer finding
- crashes, timeouts, signals, and execution errors remain findings

## Deliberate Limits

V0.7.3.1 does not yet include:

- normalization corpus checks
- case-folding corpus checks
- CoreFoundation bridge corpus checks
- Foundation bridge corpus checks
- mutated/fuzzed corpus generation

Those should be added after the validation probe remains stable.
