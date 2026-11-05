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

## Phase 4 (donor-covered out-of-line functions)

### Monitor-stream timer, "N form" (blocks ~300 timer sites in ~35 units)
- Diff: `HK_TIMER_BEGIN`/`END` (include/havok/hkMonitorStream.h) reproduce the "E form" exactly (end word
  loaded first, `extsb.` branch; hkWorld::stepDeltaTime is exact with it). About 300 sites, mostly in functions
  with a 16-byte aligned stack frame, instead load the limit word (0x0C) first and branch on a materialized bool:
  `extsb rX, r0; neg r0, rX; or r0, r0, rX; srwi. r0, r0, 31`. The same function never mixes the two forms.
- Tried: ~40 condition spellings (hkBool member/temp/local, `== true`, `!= false`, `?:`, `!!`, int/bool casts,
  `memoryAvailable()` in-class / out-of-class / static non-inline, timer as inline function vs macro, timer
  inside an inlined body, `-inline noauto/deferred`, `-ipa file` off, GC 2.6-3.0a5.2 and Wii 1.0-1.3).
  `unkC > unk4` gives the N load order and registers; only the bool materialization is missing.
- Diagnosis: compiler (bool folding); source form not found. High value: solving it unblocks most agents.

### `hkSphereBoxAgent::staticGetPenetrations` (hkSphereBoxAgent.cpp, 81.9%, 764 bytes; Halo 0x2f23c0, 0.91)
- Diff: body structure and saved registers exact; the two N-form timers (+3 insns each) and the fabs lane order
  of setAbs4 (target loads x, y, fabs x, then z, w).
- Diagnosis: blocked by the N-form timer.

### `hkBvAgent::getPenetrations` (94.3%) / `staticGetPenetrations` (96.2%) (hkBvAgent.cpp; Halo 0x2e7670/0x2e7710)
- Diff: the timer names are at +0x26/+0x32 of the unit's string pool (LthkBvAgent, StcheckBvShape, Stchild, lt
  come first, from processCollision and the other list-timer functions not written yet), and the child body
  stores: target loads motion, shape, key and stores parent, motion, key, shape.
- Tried: all 96 ctor/assignment orders of the child hkCdBody (best kept).
- Diagnosis: other (string pool needs the earlier functions) + scheduling of the child body construction.

### `hkGsk::convertFeatureToClosestDistance` (hkGsk.cpp, 41.9%, 208 bytes; Halo 0x3032c0, 0.99)
- Diff: logic exact (no sign flip of the plane on this target, unlike Halo). In the branches the target copies
  the point lane by lane (lfs/stfs through f1, as if `out` could alias `this`); ours loads all lanes first.
- Tried: real members instead of byte-offset access, local distance / out.m_normal as the source,
  `-schedule off` (wrong the other way).
- Diagnosis: compiler (alias analysis / scheduling).

### `hkCollisionDispatcher::registerCollisionAgent` (79.3%, Halo 0x2e6190, 0.91)
- Diff: the hkAgentFuncs copy is word-by-word through r0 in the target (lwz/stw pairs); ours loads all fields
  first. Same symptom as convertFeatureToClosestDistance. Symmetry flags are hkBool (the `extsb.` test).
- Tried: struct assignment, explicit member copy, `-inline noauto`, `-O4,s`, `-opt nopeephole`.
- Diagnosis: compiler (alias analysis / scheduling).

### 16 `registerAgent` functions (99.7-99.8%)
- Diff: `li r0, 0x1` (symmetricA = true) scheduled one slot later in the target; flag assignment order, true/false
  literals do not move it.
- Diagnosis: compiler (scheduling).
