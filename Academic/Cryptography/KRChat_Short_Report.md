# KR-Chat: Key Regression for Encrypted Chat Archives

**Mid-Semester Project Report**\
**Date:** October 2026

## 1. Introduction

Chat histories can contain private information. This project explores a
way to store chat archives as encrypted data while controlling which
time periods a reader can access.

The goal is to let a reader with a newer key state read past and current
messages, but not future messages.

## 2. Literature Survey

The project is based on *Key Regression: Enabling Efficient Key
Distribution for Secure Distributed Storage* by Fu, Kamara, and Kohno
(NDSS 2006).

Key regression lets a user derive keys for earlier time periods from one
current state, without deriving future states.

## 3. Basic Design and Implementation

-   Divide the archive into time-based chunks, such as one chunk per
    day.
-   Give each time period its own encryption key, derived from a
    key-regression state.
-   Encrypt each chunk using AES-GCM.
-   Give an authorized reader one state for their current epoch.
-   The reader derives earlier states to decrypt older chunks. Future
    epochs are rejected.

For the KR-AES construction, the core operations are:

``` text
previous_state = AES(current_state, 0^128)
epoch_key      = AES(current_state, 1^128)
```

The actual chat data is encrypted with AES-GCM. Its authentication check
detects ciphertext or metadata tampering.

## 4. Work Completed Till September 2026

Initial exploration focused on encrypting chat exports and rotating keys
over time. The final key-regression scope and working prototype were not
completed by the September cutoff.

## 5. Work Completed Since Finalizing the Scope

-   Studied the key-regression paper.
-   Implemented the KR-AES state operations in Python.
-   Added AES-GCM chunk encryption and decryption.
-   Tested past access, future-epoch denial, and tampering detection.
-   Ran a command-line demo; no frontend is included.

## 6. Plan of Action

1.  Protect the publisher's secret state with encryption.
2.  Add a parser for exported chat histories.
3.  Store encrypted chunks and test retrieval.
4.  Add protection against replaying an old version of the archive.
5.  Measure performance and document limitations.

## 7. References

1.  Fu, K., Kamara, S., and Kohno, T. "Key Regression: Enabling
    Efficient Key Distribution for Secure Distributed Storage."
    NDSS, 2006.
    https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/key_regression-full.pdf
2.  Python `cryptography` AES-GCM documentation:
    https://cryptography.io/en/latest/hazmat/primitives/aead/#cryptography.hazmat.primitives.ciphers.aead.AESGCM
