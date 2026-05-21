# LeoUTF8 V0.7.5 Release Seal

This document seals the LeoUTF8 V0.7.5 LeoFuzzer transform expectation baseline.

## Baseline Tag

    v0.7.5-leofuzzer-transform-expectations

## Confirmed Platform

    Mac OS X 10.5.8 PowerPC

## Confirmed Command

    make leofuzz-transform-expect-check

## Confirmed Expectation Probes

- NFC exact-output probe
- NFD exact-output probe
- Unicode case-fold exact-output probe

## Confirmed Cases

NFC:

    input:    Cafe + combining acute
    expected: composed Café
    result:   runs=1 ok=1 rejected=0 findings=0

NFD:

    input:    composed Café
    expected: Cafe + combining acute
    result:   runs=1 ok=1 rejected=0 findings=0

CaseFold:

    input:    Straße
    expected: strasse
    result:   runs=1 ok=1 rejected=0 findings=0

## Semantics

The transform expectation probes compare exact UTF-8 output bytes.

For expectation probes:

- exact output match returns OK
- transform failure returns a finding
- output mismatch returns a finding
- malformed case files return usage errors

## Significance

V0.7.5 moves LeoUTF8's LeoFuzzer coverage from stability checks to semantic checks.

LeoFuzzer now verifies that selected UTF-8 transformations produce expected bytes, not merely that transformation functions return success.
