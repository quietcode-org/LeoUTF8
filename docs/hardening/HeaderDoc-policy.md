# LeoUTF8 HeaderDoc Policy

## Status

This document defines the public API documentation policy for LeoUTF8.

It is part of the V0.7 Integration Brick Hardening phase.

## Purpose

LeoUTF8 is intended to feel like a small Leopard-native system brick.

For public headers, build correctness is not enough.

Public API must be documented at the declaration site.

## Rule

Everything installed or staged into:

~~~text
dist/LeoUTF8/include
release/LeoUTF8-*/include
~~~

must have HeaderDoc-style API comments.

Current public headers:

~~~text
Sources/LeoUTF8Core/LeoUTF8.h
Sources/LeoUTF8Core/LeoUTF8Version.h
Sources/LeoUTF8CoreFoundation/LeoUTF8CoreFoundation.h
Sources/LeoUTF8Foundation/LeoUTF8Foundation.h
~~~

## Public Header Standard

Every public function must document:

- function name
- short purpose
- detailed discussion where needed
- all parameters
- return value
- ownership rules
- invalid input behavior
- platform assumptions when relevant

## Required HeaderDoc Fields

Preferred function format:

~~~c
/*!
    @function FunctionName
    @abstract Short one-line summary.
    @discussion
        Longer explanation.

    @param parameterName
        Parameter explanation.

    @result
        Return value explanation.
 */
~~~

## Ownership Documentation

Ownership must be visible at the declaration site.

Examples:

~~~text
Caller must release the returned buffer with LeoUTF8Free().
Caller owns the returned CF object and must call CFRelease().
Caller owns the returned Objective-C object and must release it under
Leopard manual retain/release rules.
~~~

## Core Ownership Rules

LeoUTF8Core functions that allocate buffers must document:

~~~text
The caller owns the returned buffer and must release it with LeoUTF8Free().
~~~

Functions must also document whether output pointers are cleared on failure.

## CoreFoundation Ownership Rules

LeoUTF8CoreFoundation functions that create or copy CoreFoundation objects must
follow Create/Copy ownership naming and document:

~~~text
The caller owns the returned object and must release it with CFRelease().
~~~

## Foundation Ownership Rules

LeoUTF8Foundation functions that return retained Objective-C objects must
document:

~~~text
The caller owns the returned object and must release it under Leopard manual
retain/release rules.
~~~

No ARC assumptions are allowed.

## Version API Rules

Version functions must clearly distinguish:

~~~text
engine version
brick version
release name
target platform
~~~

The utf8proc engine version must not be confused with the LeoUTF8 brick version.

## HFS+ Boundary Rule

HFS+ research findings are documented in Markdown.

No HFS+ public API exists yet.

Therefore no HFS+ HeaderDoc is required yet.

If LeoUTF8HFS becomes public later, every HFS+-specific API must document:

- observed boundary behavior
- normalization assumptions
- comparison limitations
- non-goals
- filesystem scope

## Internal Source Files

Internal `.c` and `.m` files do not require HeaderDoc.

They should use ordinary comments only where needed to explain non-obvious logic.

## Markdown And HeaderDoc Relationship

Markdown documents provide contract-level explanations.

HeaderDoc comments provide declaration-level API documentation.

Both are required for public API maturity.

## V0.7 Completion Requirement

LeoUTF8 V0.7 is not considered Cupertino-2009 API-complete until all public
headers have HeaderDoc-style comments.

## Doctrine

Public headers are API surface.

Public headers are not merely compiler input.

A Leopard-native brick must document ownership, errors, and boundary behavior at
the point of use.
