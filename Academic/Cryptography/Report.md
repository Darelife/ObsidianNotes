# KR-Chat: System Design & Implementation Plan

**Key Regression for Encrypted Chat Archives**  
**Prototype basis:** KR-AES (Fu, Kamara & Kohno, NDSS 2006)  
**Prepared:** 10 October 2026

## 1. Project summary

This project builds a command-line prototype for an encrypted chat archive using **key regression**. A recipient who holds the member state for epoch *i* can derive the encryption keys for epochs `0` through `i`, but cannot derive future states. The storage server holds ciphertext only.

The application is a WhatsApp memory-book use case, but the first milestone is deliberately frontend-free: a Python library, CLI, local JSON ciphertext envelopes, and automated tests.

## 2. Scope and non-goals

### Included

- Finite-horizon KR-AES state generation.
- Member-state issuance.
- AES-GCM chunk encryption and decryption.
- Authenticated archive, epoch, and chunk metadata.
- Tests for historical access, future-access denial, and tampering.
- A CLI demo.

### Not included in the first milestone

- A React frontend.
- Production account management or cloud deployment.
- WhatsApp media import.
- Guaranteed rollback protection.
- Multi-device synchronization.
- In-place ciphertext transformation.

**Important distinction:** Key regression is not updatable encryption. Old chunks remain encrypted under their own epoch-derived keys. We do not re-encrypt the entire archive under the newest key.

## 3. Research basis

Primary reference: Kevin Fu, Seny Kamara, and Tadayoshi Kohno, “Key Regression: Enabling Efficient Key Distribution for Secure Distributed Storage,” NDSS 2006. The paper formalizes a member state from which a member derives the current epoch key and previous member states, while future keys remain pseudorandom to an evicted member.

The paper defines KR-AES using a 128-bit member state `stm`. The previous state is:

```text
unwind(stm) = AES_stm(0^128)
```

The current epoch data key is:

```text
keyder(stm) = AES_stm(1^128)
```

The two fixed plaintext blocks are different so the state transition and data-key derivation are separate operations.

The paper's original KR-AES has a fixed maximum number of epochs and publisher state proportional to that horizon. Our prototype preserves that finite-horizon structure. We use the paper's construction for the key-regression layer and AES-GCM for application data; this project is an educational prototype, not a reproduction of the paper's complete file system or proof.

## 4. System architecture

### Components

- **Publisher/owner (trusted):** Generates the finite state sequence, encrypts each epoch's chat chunk, and issues a member credential for the current epoch to an authorized reader.
- **Client/reader (trusted):** Stores one credential (epoch index plus 16-byte member state). For a chunk from an earlier epoch, it repeatedly applies `unwind` until reaching that epoch, derives the data key with `keyder`, and decrypts with AES-GCM.
- **Storage server (untrusted):** Stores opaque ciphertext envelopes and returns them on request. It never receives plaintext messages, member states, or derived AES keys in the intended architecture.
- **Local prototype:** Owner state and encrypted envelopes are JSON files. The owner-state JSON is intentionally plaintext for the demo; protecting it at rest is a required next milestone.

```text
OWNER / PUBLISHER (trusted)
  - Precomputes states
  - Encrypts daily chunks
  - Issues authorized member states
             |
             | encrypted chunks + opaque metadata
             v
STORAGE SERVER (untrusted)
  - Stores ciphertext envelopes only
  - Has no plaintext or member states
             |
             | ciphertext download
             v
READER CLIENT (trusted)
  - Uses its latest state to derive older states/keys
  - Decrypts locally with AES-GCM
```

## 5. Cryptographic algorithms

### Setup

`Setup(max_epochs)` samples a random 128-bit terminal state. For `i` from `max_epochs - 2` down to `0`, set:

```text
state[i] = unwind(state[i + 1])
```

The publisher stores the states in chronological order.

### Issue a credential

Return `MemberCredential(epoch=i, state=state[i])` and increment the next-epoch pointer. A credential's epoch is public metadata; the state is secret.

### Encrypt a chunk

`EncryptChunk(plaintext, state[i], archive_id, i, chunk_id)`:

1. Derive `K_i = keyder(state[i])`.
2. Generate a fresh random 96-bit nonce.
3. Build AAD (additional authenticated data) from a canonical encoding of format version, archive ID, epoch, and chunk ID.
4. Encrypt with AES-GCM.
5. Store the nonce and ciphertext/tag alongside the authenticated metadata.

### Decrypt a chunk

`DecryptChunk(envelope, credential(i))`:

1. Reject the envelope if its epoch `j` is greater than the credential epoch `i`.
2. Starting with the supplied `state_i`, apply `unwind` exactly `i - j` times to get `state_j`.
3. Derive `K_j = keyder(state_j)`.
4. Reconstruct the AAD and decrypt with AES-GCM.
5. Reject the chunk if authentication fails.

### Construction notation

- `0^128` is a 16-byte all-zero block.
- `1^128` in the paper is a 16-byte all-one block (`0xff` repeated 16 times).
- AES-ECB is used only for the construction's single-block AES calls. User data uses AES-GCM.

```text
unwind(stm) = AES-128(key=stm, block=00…00)
keyder(stm) = AES-128(key=stm, block=FF…FF)
K_epoch     = keyder(state_epoch)
ciphertext  = AES-GCM(K_epoch, nonce, plaintext, AAD)
```

## 6. Data format

### Member credential JSON

```json
{
  "format_version": 1,
  "epoch": 12,
  "state_hex": "<secret 16-byte state encoded as hex>"
}
```

The member state is secret and must be sent and stored securely.

### Ciphertext envelope JSON

```json
{
  "format_version": 1,
  "archive_id": "archive-identifier",
  "epoch": 12,
  "chunk_id": "chunk-identifier",
  "nonce_hex": "<12-byte nonce encoded as hex>",
  "ciphertext_b64": "<ciphertext plus GCM authentication tag, base64 encoded>"
}
```

The AAD is reconstructed from `archive_id`, `epoch`, `chunk_id`, and `format_version`; changing these fields invalidates authentication.

Prototype envelope metadata is not padded. The server can learn archive IDs, epoch numbers, chunk counts, and approximate data sizes. A production design should use opaque IDs and consider metadata minimization.

## 7. Rotation and access model

Each day/epoch uses its own derived key. When the owner issues epoch `i`'s member state to a reader, that reader can derive epoch `i`'s key and all earlier keys. If the owner stops issuing later states after a reader is removed, that reader cannot derive keys for future epochs.

This is **historical-access control, not retroactive revocation**. A reader retains access to all data covered by the last state they received and any plaintext already copied. The publisher's full state is highly sensitive; compromise of it can expose the whole configured horizon.

Because the publisher precomputes and stores the state vector, the first implementation supports a fixed horizon (for example, 365 daily epochs). Extending the horizon requires a new setup or a more advanced construction.

## 8. Threat model

### Protected against

- An honest-but-curious storage server attempting to read content.
- A reader holding only an older member state attempting to derive future epoch keys.
- Accidental or malicious modification of ciphertext or authenticated metadata.

### Not protected against in the current prototype

- Compromise of the publisher's complete state.
- Endpoint compromise.
- Plaintext copying by an authorized reader.
- Deletion or replay of a previously valid archive.
- Traffic analysis or metadata leakage.
- Password guessing against an eventual weak vault.
- Denial of service.

### Planned rollback mitigation

Add an owner-signed manifest with a monotonic version or a trusted latest-root record. AES-GCM detects tampering but does not by itself prove that the server returned the newest complete archive.

## 9. Repository and implementation

- `krchat/core.py`: KR-AES `unwind`/`keyder`, publisher state setup and issuance, credential serialization, and AES-GCM chunk envelope functions.
- `krchat/cli.py`: `demo`, `init`, `issue`, `encrypt`, and `decrypt` commands.
- `tests/test_core.py`: tests for unwind-chain correctness, historical decryption, future-epoch denial, ciphertext tampering, metadata tampering, and credential serialization.
- `README.md`: setup instructions, command examples, construction notes, limitations, and next milestones.

## 10. Validation and evaluation

### Functional tests

- A credential for epoch `i` decrypts each valid chunk from epochs `j <= i`.
- A credential for epoch `i` is denied chunks with `j > i`.

### Integrity tests

- Flipping ciphertext bytes must fail AES-GCM authentication.
- Changing authenticated metadata must fail AES-GCM authentication.

### State tests

- `unwind(state[i])` equals `state[i - 1]`.
- Malformed state files and invalid epoch values are rejected.

### Performance measurements to add

- Setup time versus horizon size.
- Member-state size.
- Cost to derive a key `k` epochs back.
- AES-GCM throughput for different chunk sizes.
- End-to-end archive processing for 1 MB, 100 MB, and larger synthetic archives.

Expected storage overhead: each chunk adds a 12-byte nonce and a 16-byte GCM tag, excluding JSON/base64 overhead. A member credential contains one 16-byte state plus epoch/format metadata.

## 11. Development plan

1. **Milestone 1 — completed in the included proof of concept:** Implement KR-AES state operations, AES-GCM envelopes, CLI demo, and core tests.
2. **Milestone 2 — protect publisher state at rest:** Add an authenticated encrypted vault and a password-based KDF; avoid leaving long-term secrets in plaintext JSON.
3. **Milestone 3 — import chat exports:** Add WhatsApp text-export parsing and chunk messages by date; keep raw message text encrypted, including timestamps and participant names.
4. **Milestone 4 — storage and rollback protection:** Add an untrusted storage API and encrypted object storage; implement owner-signed manifests and rollback checks.
5. **Milestone 5 — evaluate:** Run performance tests, record results, document attacks and limitations, then optionally build the memory-book frontend.

## 12. Security caveats

This code is for a course demonstration, not production use. It has not undergone an independent security review. Do not put real private chats into it.

The publisher state JSON is plaintext. Protecting it is mandatory before handling real data. The current format also lacks rollback protection and an authenticated account/authorization layer.

Do not describe this as server-side ciphertext update, forward secrecy for all historical messages, or retroactive revocation. Its core property is compact distribution of time-indexed access: a newer member state derives older keys but not future states.

## 13. References

1. Fu, K., Kamara, S., & Kohno, T. (2006). *Key Regression: Enabling Efficient Key Distribution for Secure Distributed Storage*. NDSS 2006. https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/key_regression-full.pdf
2. NDSS proceedings entry: https://www.ndss-symposium.org/ndss2006/key-regression-enabling-efficient-key-distribution-secure-distributed-storage/
3. Python `cryptography` documentation, AEAD / AESGCM: https://cryptography.io/en/latest/hazmat/primitives/aead/#cryptography.hazmat.primitives.ciphers.aead.AESGCM

The included prototype intentionally uses the KR-AES construction rather than substituting a plain SHA-256 chain.