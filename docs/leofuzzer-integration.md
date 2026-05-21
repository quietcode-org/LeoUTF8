# LeoUTF8 LeoFuzzer Integration

LeoUTF8 integrates with LeoFuzzer as an external Leopard-Crew brick.

The integration provides a small validation probe and UTF-8 corpus sets that can be run through LeoFuzzer.

## Probe

    Tests/leofuzz_validate_probe.c

The probe accepts one input file path.

It reads the file as bytes and calls:

    LeoUTF8ValidateBytes(bytes, length)

## Exit Contract

- exit 0: valid UTF-8
- exit 1: expected domain-level rejection, such as malformed UTF-8
- exit 2: invalid probe usage

This maps cleanly to the LeoFuzzer probe contract:

- OK
- DOMAIN_REJECT
- USAGE

Malformed UTF-8 is not a LeoFuzzer finding for this probe. It is an expected domain-level rejection.

## Corpus Layout

    corpus/leofuzz/valid/
    corpus/leofuzz/invalid/

The valid corpus contains:

- ASCII input
- UTF-8 with umlauts
- UTF-8 emoji bytes
- embedded NUL bytes
- empty input

The invalid corpus contains:

- invalid continuation sequence
- lone continuation byte
- out-of-range sequence
- overlong slash
- truncated three-byte sequence

## Empty Input Semantics

An empty file is treated as valid zero-length UTF-8 input.

The probe passes a non-NULL buffer with length 0 to avoid confusing an empty byte sequence with a NULL input pointer.

## Make Targets

Build the probe:

    make leofuzz-validate-probe

Run the valid corpus:

    make leofuzz-corpus-valid

Run the invalid corpus:

    make leofuzz-corpus-invalid

Run the full LeoFuzzer check:

    make leofuzz-check

## Expected Results

Valid corpus:

    runs=5
    ok=5
    rejected=0
    findings=0

Invalid corpus:

    runs=5
    ok=0
    rejected=5
    findings=0

## Principle

LeoUTF8 must reject malformed UTF-8 safely and explicitly.

A malformed input is not a crash. It is a domain-level rejection.
