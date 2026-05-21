# LeoUTF8 V0.7.4 Release Seal

This document seals the LeoUTF8 V0.7.4 LeoFuzzer transform probe baseline.

## Baseline Tag

    v0.7.4-leofuzzer-transform-probes

## Confirmed Platform

    Mac OS X 10.5.8 PowerPC

## Confirmed Command

    make leofuzz-transform-check

## Confirmed Transform Probes

- NFC normalization
- NFD normalization
- Unicode case folding

## Confirmed Results

NFC valid corpus:

    runs=5
    ok=5
    rejected=0
    findings=0

NFC invalid corpus:

    runs=5
    ok=0
    rejected=5
    findings=0

NFD valid corpus:

    runs=5
    ok=5
    rejected=0
    findings=0

NFD invalid corpus:

    runs=5
    ok=0
    rejected=5
    findings=0

CaseFold valid corpus:

    runs=5
    ok=5
    rejected=0
    findings=0

CaseFold invalid corpus:

    runs=5
    ok=0
    rejected=5
    findings=0

## Semantics

For transform probes:

- valid UTF-8 inputs must transform successfully
- invalid UTF-8 inputs must be rejected as DOMAIN_REJECT
- invalid UTF-8 is not a LeoFuzzer finding
- TSV reports must contain seven fields per data row

## Deliberate Limits

V0.7.4 does not yet compare exact transform output bytes.

It verifies that transform paths accept valid UTF-8, reject malformed UTF-8 safely, and produce no crash, timeout, signal, or execution findings.
