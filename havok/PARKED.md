# Parked Havok functions

One entry per function given up on: symbol, unit, match %, the remaining diff, and a diagnosis
(compiler, inlining, layout, version difference, paired-singles/hand asm, other).

## Phase 3 (math and base inline layer)

### `hkMemory::getAllocatedSize(int)` (hkMemory.cpp, 61.9%, 32 bytes)
- Diff: the target keeps `cmpwi; ble small; <round up to 16> + 0x10; blr; small: addi r3, r4, 8; blr`.
  Ours hoists `addi r0, r4, 0xf` above the compare and folds the large case into `bgtlr`.
- Tried: `if/else` in both orders, a result local, a block-local temporary, permuter (90 s, best
  needs `scheduling off`).
- Diagnosis: compiler (block placement / speculation). No source form found.

### `hkThreadMemory::allocateChunk(int, int)` (97.4%) and `deallocateChunk(void*, int, int)` (97.3%)
- Diff: one extra `blr` after the closing virtual tail call (`bctr`). The target has the 4-byte
  `onAllocate` / `onDeallocate` symbols right behind each function and no dead `blr`.
- Diagnosis: other (symbol universe). Same pattern as the TIPS.md entry on the dead `blr` after a
  virtual tail call; probably needs the symbols-only change described there (extend the function
  by 4 bytes, drop the 4-byte stub), validated on its own baseline.

### `hkThreadMemory::onStackOverflow(int)` (95.9%)
- Diff: `size = max(nbytes + 0x400, 0x1000)` is `ble L1; b L2; L1: li r31, 0x1000; L2:` in the
  target (ours: `bgt; li`), and r5/r6 are swapped in the block-header copy.
- Tried: `if (size <= 0x1000)`, three ternary spellings, a local inline `max2` template, permuter
  (120 s, no gain).
- Diagnosis: compiler (branch form of the clamp).

### `__sinit_\hkVector4_cpp` (`fn_80287AD0`, hkVector4.cpp, 0%, 52 bytes)
- Diff: missing. It copies the 16-byte `.bss` vector at `0x80532540` into `0x80532550` (a global
  zero vector read by hkGskCache, hkMeshShape and hkSolver).
- Diagnosis: layout. The `.bss` range `0x80532540-0x80532560` is not split to any unit; by link
  order it belongs to hkVector4.cpp (between hkSweptTransformClass and hkCdBodyClass). Needs a
  `.bss` entry in splits.txt plus the two globals; names unknown.

### `hkRotation::set(const hkQuaternion&)` (93.2%)
- Diff: identical schedule, FPR coloring differs (the quaternion's x and w land in f6/f8 swapped).
- Tried: local declaration orders, permuter (150 s, `__typeof__` noise only).
- Diagnosis: compiler (register coloring).

### `hkRotation::isOrthonormal(hkReal)` (93.7%)
- Diff: the cross product `col0 x col1` is scheduled lane x, y, z in the target; ours computes the
  z-lane product first. Locals, branches and the hkBool return match.
- Tried: four `hkVector4::setCross` bodies (the `set()` form is best here and for
  invertSymmetric), local orders, `equals3` (88%), permuter (150 s; the best result reorders the
  column checks, not adopted).
- Diagnosis: compiler (scheduling); possibly a different cross-product inline in this unit.

### `hkMatrix3::invertSymmetric()` (90.6%), `hkMatrix3::invert(float)` (80.6%)
- Diff: same cofactor shape as the target; cross-product scheduling and FPR numbering differ
  (invert also saves two more FPRs in the target).
- Diagnosis: compiler (scheduling / register pressure), same root as isOrthonormal.

### `hkMatrix3::isApproximatelyEqual(float, const hkMatrix3&) const` (90.0%)
- Diff: per column the target subtracts lane w first (then x, y, z); stack slots and the hkBool
  `&&` chain match.
- Tried: swapping the locals inside `equals3` (72%).
- Diagnosis: inlining (lane order of the inlined `setSub4`); changing `setSub4` globally not tried.

### `hkTransform::setMulInverseMul(const hkTransform&, const hkTransform&)` (95.3%)
- Diff: structure exact (transposed product, then `_setRotatedInverseDir` of the translation
  difference); FPR numbering is shifted by one through most of the body (the 0.0 constant lives in
  f31 in the target).
- Tried: the six load orders of the column loop, block scoping, permuter (200 s, best only adds a
  `__typeof__` copy).
- Diagnosis: compiler (register coloring).
