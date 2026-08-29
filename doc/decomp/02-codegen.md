# 02 — GCC 4.7 (Android NDK r8e) x86-32 codegen encyclopedia

Toolchain: `ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-g++`
Results were measured with temporary compile/disassembly experiments using
canonical flags:
`-fno-exceptions -fno-rtti -Wno-write-strings -std=gnu++11 -g -fno-function-sections -fno-data-sections`,
and cross-checked against carved and rebuilt objects. The original `/tmp`
experiment files were ephemeral and are not part of the repository; reproduce
a claim from the inline source/assembly description before relying on it for a
new edge case.

Toolchain personality (verified): `__PIC__ 2` (PIC ON by default — no -fPIC needed), `__SSE__/__SSE2__/__SSE3__ 1`,
`__SSE_MATH__/__SSE2_MATH__ 1` (SSE2 math applies to float **and** double), `__tune_atom__ 1` (Atom tuning).
This is Google's forked 4.7: it differs from upstream in several places flagged below with **[ANDROID]**.

## Quick reference (-O0 vs -O3)

| topic | -O0 | -O3 |
|---|---|---|
| prologue | `push %ebp` + `mov`/`lea (%esp),%ebp`; frame via `lea -N(%esp),%esp` **[ANDROID]**, never `sub` | no frame; args at `4(%esp)`+; frame kept only with 16B realign |
| stack realign | never | `and $0xfffffff0,%esp` only for SSE-vectorized (-O3) code |
| PIC base | `call __x86.get_pc_thunk.{bx,cx}` + `addl $_GLOBAL_OFFSET_TABLE_,%reg` | same |
| thunk reg | ecx for plain leaves; ebx (pushed) when GOT must survive (calls, or long-lived/vectorized loops) | same rule |
| extern global read | GOT slot reloaded per use | GOT slot hoisted, deref per use |
| local static/string | `@GOTOFF(%reg)` direct | same |
| comparison value (int ret) | `setcc` + `movzbl` | reg-reg: `setcc`+`movzbl`; const-vs-mem: `xor %eax,%eax` pre-`setcc` (no movzbl) |
| comparison value (bool ret) | bare `setcc` (no movzbl; byte-store+reload only if stored to a local) | bare `setcc` + nop padding (no movzbl, no xor) |
| comparison branch | straight-line `if`: fused `cmp/jcc`; loop conditions: `setcc %al; test %al,%al; jcc` | fused `cmp/jcc` or `test/jcc` |
| `&&`/`\|\|` result | branchy (short-circuit) | `setcc`+`andl/orl %edx,%eax` (order varies; see §3) |
| ternary | branch + mov | `cmov` (reg arms) or arithmetic (const arms) |
| constant div | magic `imul` **[ANDROID]** | magic `imul` |
| variable div | `idivl` (`mov`+`sar $0x1f`, no cltd) | `idivl` (same preamble) |
| 64-bit add/sub/mul/shift | inline (`adc`, cross-`imul`, `shld`) | inline |
| 64-bit div/mod | `__divdi3`/`__udivdi3`/`__moddi3`/`__umoddi3` | same |
| float math | SSE `addss`/`movss` + store to st(0) | SSE `addss` (return via x87 st(0)) |
| double math | SSE `addsd`/`movsd` (NOT x87) | SSE (return via st(0)) |
| float cmp | `ucomiss`+setcc | same |
| float→int | `cvttss2si` / `cvttsd2si` | same; 64-bit via `fisttpll` |
| loops | `jmp` to condition; cond via setcc+test+jcc | rotated; test at bottom |
| switch dense | jump table (PIC-relative) if bodies simple; else cmp chain | value table, jump table, or cmp chain (see §8) |
| switch sparse | cmp chain | cmp chain |
| calls | args right-to-left `movl $imm,(%esp)+N` | same; args from reg |
| tail calls | — | external/default-visibility PIC calls use `call`+`ret`; local or hidden callees may use `jmp` |
| struct ret (any size) | hidden sret ptr at `8(%ebp)`, callee `ret $4` (`c2 04 00`) | same (ptr at `4(%esp)`) |
| virtual call | `mov (%eax),%eax; mov (%eax),%eax; call *%eax` | `mov (%eax),%edx; call *(%edx)` (fused load) |

---

## 1. Prologue / epilogue

### -O0 frame setup — TWO encodings, both = `ebp ← esp`
```asm
  0: 55           push   %ebp
  1: 8d 2c 24     lea    (%esp),%ebp     ; 55 8d 2c 24 — Android-patched form
```
or
```asm
  0: 55           push   %ebp
  1: 89 e5        mov    %esp,%ebp      ; canonical
```
`int add(int a,int b){return a+b;}`, -O0. Both forms are real: `lea (%esp),%ebp` (bytes `8d 2c 24`, the Android x86 patch) and canonical `mov %esp,%ebp` (bytes `89 e5`). A historical census found the `mov` form substantially more common, but the split tree has since been reorganized. Treat either encoding as plausible and compare the specific target function; there are counterexamples to simple GOT/leaf heuristics.

### -O0 stack allocation — `lea`, NEVER `sub`
```asm
  25: 8d 64 24 fc   lea    -0x4(%esp),%esp    ; frame of 4 bytes
```
`float fsum(float a,float b){...}`, -O0. Every -O0 frame allocation in every experiment (and in all real split/*.o) uses `lea -N(%esp),%esp`. The only `sub` touching esp is the dynamic alloca adjustment (`sub %eax,%esp`). Epilogue for such functions is `leave` (`c9` = `mov %ebp,%esp; pop %ebp`), i.e. `...; leave; ret`; leaf-without-locals ends `pop %ebp; ret`.

### -O3 — no frame pointer
```asm
   0: 8b 44 24 08   mov    0x8(%esp),%eax
   4: 03 44 24 04   add    0x4(%esp),%eax
   c: c3            ret
```
`int add(...)`, -O3. Args read from `N(%esp)` directly. Frame kept + realigned only when the compiler needs 16-byte alignment (see below); then prologue is `push %ebp; mov %esp,%ebp; push %ebx/[%esi/%edi]; call thunk; addl; and $0xfffffff0,%esp` and args stay at `8(%ebp)+`.

### 16-byte realignment trigger
```asm
  14: 83 e4 f0     and    $0xfffffff0,%esp
```
`while_sum` loop, -O3 (vectorized with `movdqa`/`paddd`). Empirical trigger: **SSE-vectorized code (movdqa/aligned spills) at -O3** — the vectorizer requires 16B-aligned stack. NOT triggered by: scalar SSE (movss/addss), float/double locals, alloca alone, 8-byte struct args (Android ABI keeps the stack 16B-aligned at call sites, so plain functions never realign). Not observed at -O2 (no vectorizer without -O3 in 4.7).

verified: `g++ -O0/-O3 -c pro.cpp; objdump -d`

## 2. PIC

Every function that touches a global/static/string/call gets a prologue thunk:
```asm
   0: e8 fc ff ff ff   call   <func+1>          ; R_386_PC32 __x86.get_pc_thunk.cx
   5: 81 c1 02 00 00 00 addl   $0x2,%ecx        ; R_386_GOTPC _GLOBAL_OFFSET_TABLE_
```
**Register choice (verified across all experiments):** `cx` for plain leaves (no call inside; nothing to save); `bx` (pushed at entry, popped before ret) when the GOT base must survive — a call inside, OR register pressure over a long-lived use (e.g. the vectorized `sumn` loop at -O3 uses thunk.bx with **no calls at all**). (`dx` never observed.) The thunk bodies are emitted per-object:
`__x86.get_pc_thunk.bx`: `8b 1c 24 mov (%esp),%ebx; c3 ret` (reads its return address = PC).

**Extern global read** — GOT slot, then deref:
```asm
   b: 8b 81 00 00 00 00   mov    g_counter@GOT(%ecx),%eax
  11: 8b 00               mov    (%eax),%eax
```
`int read_global(){return g_counter;}`, -O3. At -O0 the GOT slot load is repeated for **every** use (`g_a+g_b` loads the slot twice, once per operand); -O3 hoists the slot loads and derefs at each use.

**Local static / string literal** — GOTOFF, no extra deref (binding is local):
```asm
  6b: 8d 81 00 00 00 00   lea    .LC0@GOTOFF(%ecx),%eax
```
`const char* greet(){return "hello";}`, -O3. Rule: `@GOT(%ebx)`+deref for external (default-visibility) symbols; `@GOTOFF(%ebx)` for file-local/static data, guard vars, strings.

**Calls** — an external call normally carries an `R_386_PLT32` relocation
against the callee; local/hidden direct calls use `R_386_PC32`. In `.o`
disassembly the unresolved displacement commonly appears as `e8 fc ff ff ff`.
verified: `g++ -O0/-O3 -c pic.cpp; objdump -dr pic_O3.o`

## 3. Comparisons (critical)

All at -O0, `int f(int x){ return x < 5; }`:
```asm
   4: 83 7d 08 04   cmpl   $0x4,0x8(%ebp)      ; x<5 canonicalized to x<=4
   8: 0f 9e c0      setle  %al
   b: 0f b6 c0      movzbl %al,%eax
```
**Canonicalization:** `x<5` → `cmpl $4;setle`; `x>=5` → `cmpl $4;setg`; `x<=5` → `cmpl $5;setle`; `x>5` → `cmpl $5;setg`; `==` → `cmpl $5;sete`; `!=` → `cmpl $5;setne`. The constant is compared directly against the memory operand — **no `movl` of the value into a register first, and NO `test %al,%al`** when the result is the return value. setcc for unsigned: `<`→`setb`, `>=`→`seta` (canonicalized to `$4` too). reg-reg compare: `cmp 0xc(%ebp),%eax; setl %al` (AT&T `cmp src,dst` — register is the destination of the subtract).

The `setcc %al; test %al,%al; jcc` sequence appears at -O0 **only in jump-to-condition loop conditions** (the condition is re-evaluated on every iteration; see `while` below). Straight-line `if/else` at -O0 is a fused `cmp/jcc` with no setcc (`if (a<b) return e1;` = `cmp 0xc(%ebp),%eax; jge`), and `if (x<5) g();` = `cmpl $0x4,0x8(%ebp); jg` directly. The "classic" `movl slot,%reg; cmpl; setcc; test %al,%al; jcc` sequence therefore does NOT appear in plain branches:

```asm
   8: c7 45 fc 00 00 00 00  movl   $0x0,-0x4(%ebp)
   f: eb 0a                 jmp    1b            ; jump-to-condition loop
  11: 8b 45 08              mov    0x8(%ebp),%eax
  14: 01 45 fc              add    %eax,-0x4(%ebp)
  17: 83 6d 08 01           subl   $0x1,0x8(%ebp)
  1b: 83 7d 08 00           cmpl   $0x0,0x8(%ebp)
  1f: 0f 9f c0              setg   %al
  22: 84 c0                 test   %al,%al
  24: 75 eb                 jne    11
```
`while(n>0){...}`, -O0. Condition = `cmpl $0` + `setg` + `test %al,%al` + `jne`.

**-O3 value returns — exact rules (re-measured):**
- `int f(x){ return x<5; }` (const vs mem): `xor %eax,%eax; cmpl $4,0x4(%esp); setle %al; ret` — xor pre-setcc, **no movzbl** (eax is dead so zeroing first is cheaper than extending).
- `int f(a,b){ return a<b; }` (reg-reg): `mov a,%eax; cmp %eax,b; setl %al; movzbl %al,%eax; ret` — **movzbl after setcc, no xor**.
- `bool f(...)`: BARE `setcc %al` at -O0 AND -O2/-O3 (no movzbl, no xor — only the low byte matters for a bool return). -O2/-O3 pad with nops: `setl %al; nop; nop; ret`. Only when the bool passes through a local slot (`bool x = a<b; return x;`) does -O0 emit the byte store + `movzbl -0x1(%ebp),%eax` reload; at -O3 the local vanishes and it is again bare `setl %al`.
- **Zero-check:** `x==0`/`x!=0`/null-pointer checks compile to `test %reg,%reg` (not `cmp $0`); `x>5` uses `cmp` (verified: `test %ecx,%ecx; jle` in bounded loop; `test %eax,%eax; cmove` in hot()).

**char comparisons:**
```asm
   a6: 0f b6 45 fc    movzbl -0x4(%ebp),%eax    ; char vs char: zero-extend
  aa: 3a 45 f8        cmp    -0x8(%ebp),%al     ; then 8-bit cmp (cmpb), signed setl
```
```asm
  c3: 80 7d fc 79     cmpb   $0x79,-0x4(%ebp)   ; char vs constant: direct byte cmp,
  c7: 0f 9e c0        setle  %al                ; NO extension; canonicalized too ('z'-1)
```
- `char vs constant`: `cmpb $imm,mem` directly on the stored byte.
- `char vs char` (-O3, no locals): `movzbl 0x8(%esp),%eax; cmp %al,0x4(%esp); setl %al` — zero-extends one side, 8-bit compare, SIGNED setcc.
- char `!= 0`: `cmpb $0x0,...; setne`.

**`&&`/`||`:**
- -O0 (`if (a<5 && b>3)`): nested branches with inverted conditions: `cmpl $4,a; jg out; cmpl $3,b; jle out; mov $1; jmp end`.
- -O3 (`if` version): branch + cmov: `cmpl $3,b; mov c,%eax; jg` → `cmpl $4,a; mov $1,%edx; cmovle %edx,%eax`.
- -O3 (boolean value `return a<5 && b>3`): **both operands evaluated, no short-circuit**:
```asm
   0: cmpl   $0x3,0x8(%esp)     ; b > 3  (evaluated FIRST — reversed order!)
   5: setg   %dl
   8: xor    %eax,%eax
   a: cmpl   $0x4,0x4(%esp)     ; a <= 4
   f: setle  %al
  12: and    %edx,%eax
```
`||` value: same shape with `or %edx,%eax`. Note the emission order is NOT reliably reversed: `return a<b && a>0` emits in SOURCE order (`cmp; setl %al; test; setg %dl; and %edx,%eax`). The stable invariant is the register layout, not the order: the first operand's setcc result lands in `%al` (zero-extended with xor/movzbl for int returns), the second in `%dl`, combined `and/or %edx,%eax`. Emission order depends on register scheduling.

**bool/int results (re-measured):** `bool x = a<b; return x;` -O0: `setl %al; mov %al,-0x1(%ebp); movzbl -0x1(%ebp),%eax` (byte store). -O3: bare `setl %al` + nop padding — **NO movzbl** (bool needs only %al). Contrast with `int f(a,b){return a<b;}`: -O0 `setl %al; movzbl %al,%eax`; -O3 reg-reg same `setl %al; movzbl %al,%eax` (no xor); -O3 const-vs-mem `xor %eax,%eax; cmpl $4; setle %al` (no movzbl).

**Ternary:** -O0 = branch + mov (`cmp; jge; mov $5; jmp; mov $7`). -O3:
- both arms regs: `cmovle %edx,%eax` (`cmp %eax,%edx; cmovle %edx,%eax` for `a<b?a:b`; cmovge/cmovl/cmovg per comparison).
- both arms constants: arithmetic trick, NO cmov: `a<b?5:7` → `cmp; setge %al; movzbl; add %eax,%eax; add $5,%eax`.
verified: `g++ -O0/-O3 -c cmp.cpp; objdump -d`

## 4. Integers

**Signed vs unsigned variable div** (all levels):
```asm
   4: 89 c2           mov    %eax,%edx         ; sign-extend preamble: mov + sar
   6: c1 fa 1f        sar    $0x1f,%edx        ; NOT cltd! (contradicts upstream docs)
   9: f7 7c 24 08     idivl  0x8(%esp)
```
Unsigned: `xor %edx,%edx` (-O3) / `mov $0x0,%edx` (-O0), then `divl mem`. Result of `%`: `mov %edx,%eax`. **No `cltd`/`cdq` anywhere** — this fork always emits the 3-instruction `mov;sar $31` form (also in real split objects).

**Division by constant — magic at EVERY level (including -O0!):** `a/7`:
```asm
  44: ba 93 24 49 92   mov    $0x92492493,%edx    ; magic 2^35/7 rounded
  49: 89 c8            mov    %ecx,%eax
  4b: f7 ea            imul   %edx                 ; one-arg imul (edx:eax)
  4d: 89 d0            mov    %edx,%eax
  4f: 01 c8            add    %ecx,%eax
  51: c1 f8 02         sar    $0x2,%eax
  54: c1 f9 1f         sar    $0x1f,%ecx
  57: 29 c8            sub    %ecx,%eax            ; fixup for negatives
```
**[ANDROID]** — upstream 4.7 emits raw `idiv` at -O0; this fork applies the multiply-shift transform at -O0/-O1/-O2/-O3 alike. -O0 adds redundant register shuffles around the same magic.

**Remainder by constant (-O3):** `imul $magic; sar; sar; sub` then `shl $3, %edx; sub %eax,%edx; mov %ecx,%eax; sub %edx,%eax` (q = a/7, r = a - 7*q — no `neg` form).

**32-bit multiply:** two-arg form always: `imul 0xc(%ebp),%eax` (-O0) / `imul 0x4(%esp),%eax` (-O3). One-arg `imul %edx` reserved for the div-magic.

**Shifts:** `shl %cl,%eax` (count in cl, no mask, both levels). 64-bit shift inline: `shld %cl,%eax,%edx; shl %cl,%eax; test $0x20,%cl; je; mov %eax,%edx; xor %eax,%eax` (shift ≥32 handling).

**Extensions:** `movsbl` (char→int), `movzbl`, `movswl`, `movzwl` — load-and-extend directly from the arg slot or memory operand, both levels. At -O0 with locals they first `mov %al`/`mov %dx` to the slot then extend on reload.

**64-bit:**
- add/sub: inline `addl + adcl` / `subl + sbbl` (lo at 4(%esp), hi at 8(%esp)).
- **mul: INLINE at -O0 and -O3** — cross-products `imul;imul;mul;add;add` (NO `__muldi3`; **[ANDROID]** patch).
- shl/shr: inline `shld`+`shl %cl` with `test $0x20` fixup.
- div/mod: calls `__divdi3/__udivdi3/__moddi3/__umoddi3` (nm `U` entries verified).
verified: `g++ -O0/-O3 -c int.cpp; objdump -d; nm -a`

## 5. Floats / doubles

**Arithmetic is SSE2 for float AND double** (contradicts the "doubles use x87" assumption):
```asm
  24: f3 0f 10 44 24 0c   movss  0xc(%esp),%xmm0
  2a: f3 0f 58 44 24 08   addss  0x8(%esp),%xmm0
  30: f3 0f 11 04 24      movss  %xmm0,(%esp)
  35: d9 04 24            flds   (%esp)         ; return via x87 st(0)
```
doubles: `movsd/addsd/mulsd/ucomisd`. Return convention: result stored to a stack temp and reloaded with `flds`/`fldl` → **returned in x87 st(0)** (i386 SysV ABI), both -O0 and -O3. The compiler keeps a 4/8/12-byte frame (`lea -4/-0xc(%esp),%esp`) for the roundtrip.

Re-verified 2026-08: the SSE2 default is the fork's `-mfpmath=sse` default (`__SSE_MATH__`/`__SSE2_MATH__` = 1, from `-dM -E`), NOT upstream's documented `-mfpmath=387`. With explicit `-mfpmath=387` the same source compiles to pure x87: `fldl a; fldl b; fmul %st(1),%st; faddp; ret`, loop acc = `fldz; faddl (%eax)` (no 12-byte frame, no fldl-return roundtrip), compare = `fldl; fldl; fxch; fucomip %st(1),%st; fstp %st(0); seta %al` (ucomisd → fucomip). The real original is overwhelmingly SSE2. **Agents should expect SSE2 (`movsd`/`addsd`/`ucomisd`) for double arithmetic; x87 primarily appears in int64↔double conversions (`fildll`/`fisttpll`) and st(0) return roundtrips.** Search recursively under `build/split/` when recensing instructions.

**Comparisons:** `movss arg,%xmm0; ucomiss arg2,%xmm0; setcc %al` (ucomiss/ucomisd; `a<b` → `seta` since operand order puts b in xmm0). Compare-against-float-constant (`a>0.0f`) loads the constant from `.rodata` via the PIC base (`movss .LC@GOTOFF(%ecx),%xmm1` — no imm-broadcast), then `ucomiss`.

**Conversions:** float→int `cvttss2si`, double→int `cvttsd2si` (truncating, both levels). float→long long and double→long long: **x87** `flds/fldl; fisttpll` (stored to temp, loaded `mov (%esp),%eax; mov 4(%esp),%edx`) — no libcalls. int→float `cvtsi2ssl`, int→double `cvtsi2sdl`, float→double `cvtss2sd`, double→float `cvtsd2ss`. Negation: rodata sign-mask + `xorps` (or `xorpd`).

**Params:** floats/doubles passed on the stack in their natural 4/8-byte slots; prologue moves them with `movd %eax,%xmm0` / `movss`-from-mem at -O0, direct `movss 0xc(%esp),%xmm0` at -O3.

Mixed `double+float` math converts up (`cvtss2sd` + `addsd`) then converts down to int with `cvttsd2si`.
verified: `g++ -O0/-O2/-O3 -c flt.cpp; objdump -d`

## 6. Globals & statics

**Plain extern global** (`g_counter`): load via GOT + deref (`mov @GOT(%ecx),%eax; mov (%eax),%eax`); -O0 reloads GOT slot per use, -O3 hoists. **Local static / GOTOFF**: `int counter(){static int n=0; return ++n;}` at -O0:
```asm
  5d: 8b 81 b0 01 00 00   mov    n@GOTOFF(%ecx),%eax
  63: 83 c0 01            add    $0x1,%eax
  66: 89 81 b0 01 00 00   mov    %eax,n@GOTOFF(%ecx)
```
Zero-initialized static (`static int n = 0`): **no guard** (constant init).

**Guard for dynamic init** (`static int n = x*3;`, -O0): guard byte `_ZGVZ1fiE1n` (local bss symbol), code:
```asm
  13: 8d 83 00 00 00 00    lea    guard@GOTOFF(%ebx),%eax
  19: 0f b6 00             movzbl (%eax),%eax
  1c: 84 c0                test   %al,%al
  1e: 75 34                jne    done
  20: ... mov %eax,(%esp); call __cxa_guard_acquire
  2e: test %eax,%eax; setne %al; test %al,%al; je done
  37: ... init code ...
  46: ... call __cxa_guard_release
```
Guards are emitted even with `-fno-exceptions` (verified). The guard path spans calls → PIC base is ebx (thunk.bx).

**Strings:** land in `.rodata.str1.1`; duplicates merged — `greet()` and `greet2()` both use `.LC0` ("hello" stored once); access `lea .LC0@GOTOFF(%ecx),%eax`.

**Data placement:** nonzero-initialized globals/arrays → `.data` (`arr`, `gs`); zero-init 100-int array → `.bss` (`big`, `B` in nm); partial init pads `.data` with zeros. `struct S gs = {1,2}` = 8 bytes inline in `.data` (no runtime init code for constant initializers — runtime code only for dynamic (guard) initializers).
verified: `g++ -O0/-O3 -c glob.cpp guard.cpp; objdump -dr -s -j .data/.rodata.str1.1; nm`

## 7. Alignment & padding

`.p2align 4,,15` before every function (raw -S verified); functions aligned 16 bytes. Padding fills observed (-O3 objects, incl. real ones):
- `90` single nops (1–4),
- `66 90 xchg %ax,%ax` (2B),
- `8d 76 00 lea 0x0(%esi),%esi` (3B),
- `8d 74 26 00 lea 0x0(%esi,%eiz,1),%esi` (4B),
- `8d b4 26 00 00 00 00` (7B) and `8d bc 27 00 00 00 00` (7B, edi variant),
- `8d b6 00 00 00 00` (6B, `lea 0x0(%esi),%esi`) and `8d bf 00 00 00 00` (6B, edi variant),
- `89 f6 mov %esi,%esi` (2B).
No `int3`/multibyte nops from `-fpatchable-*`; the multi-byte `lea` idioms are the classic GCC-4.7 Atom-safe fills. No dead nops after `ret` beyond alignment; unreachable code after noreturn calls is simply dropped (no warning-only tail).

**Hot/cold:** NO `.text.hot`/`.text.unlikely` — `__builtin_expect` only feeds branch/if-conversion heuristics: `hot()` at -O3 = `test %eax,%eax; mov $-1,%edx; cmove %edx,%eax; ret`.
verified: `g++ -O3 -S align.cpp; objdump -d`

## 8. Control flow

**Loops:** -O0 while/for = `jmp cond; body; cond: setcc+test+jcc` (jump-to-condition, body in middle — see §3). -O0 do-while = body then `cmpl; setcc; test; jne body` directly. -O3: rotated, test at bottom: `test; jle out; xor; jmp entry; loop: ...; cmp $N,%edx; jle loop` (bounded-loop example; entry pre-test for the first condition). Simple counting loops at -O3 get **vectorized** (paddd/punpckldq/movdqa, rodata vector constants via GOT) with a scalar tail loop; vectorization pulls in the `and $0xfffffff0,%esp` realignment.

**Switch dense:** jump table when the case bodies are simple (constant returns) or the case count is large; the -O0 5-case constant-return switch gives: `cmpl $5,a; ja default; shl $2; mov table@GOTOFF(%eax,%ecx,1),%eax; add %ecx,%eax; jmp *%eax`. -O3 (8-case, call bodies): `cmp $8,a; jbe; mov table@GOTOFF(%ecx,%edx,4),%edx; add %ecx,%edx; jmp *%edx` — **table entries are RELATIVE offsets** added to the PIC base (R_386_GOTOFF relocs in `.rodata`, all-zero bytes in the .o). Constant-return dense switch degenerates to a `.rodata` **value** table: `mov table@GOTOFF(%ecx,%edx,4),%eax` with `xor %eax,%eax` default (no jumps at all; values 10/20/30/40/50 observed in .rodata). CAVEAT (re-measured): a 4-case dense switch with function-call bodies does NOT get a table at either level — GCC emits a decision-tree cmp chain: `cmp $2,%eax; je; cmp $2,%eax; jg; cmp $1; je; ...` — note the duplicated midpoint `cmp $2` (tree compares both sides of the midpoint), with the case bodies as tail-copied call blocks.

**Switch sparse:** cmp chain (both levels): `cmp $5,%edx; je; cmp $100,%edx; mov $20,%eax; je; ...` with results interleaved at -O3.

**goto/labels:** nothing special (dead code eliminated); `empty()` stub = `push %ebp; mov %esp,%ebp; pop %ebp; ret` exactly (**mov form** — the trivial no-local leaf is one of the mov-form cases, bytes `55 89 e5 5d c3`); `stub(x){return x;}` = `push %ebp; lea (%esp),%ebp; mov 0x8(%ebp),%eax; pop %ebp; ret` (lea form, `55 8d 2c 24 8b 45 08 5d c3`).
verified: `g++ -O0/-O3 -c loop.cpp sw2.cpp; objdump -dr`

## 9. Function calls

**Plain call, -O0** — right-to-left arg copy into esp-relative slots then call:
```asm
  13: 8b 45 10         mov    0x10(%ebp),%eax
  16: 89 44 24 08      mov    %eax,0x8(%esp)     ; arg3 (rightmost) first
  1a: 8b 45 0c         mov    0xc(%ebp),%eax
  1d: 89 44 24 04      mov    %eax,0x4(%esp)     ; arg2
  21: 8b 45 08         mov    0x8(%ebp),%eax
  24: 89 04 24         mov    %eax,(%esp)        ; arg1
  27: e8 fc ff ff ff   call   extf@PLT
```
-O3: `movl $imm,disp(%esp)` for constants, `mov %reg,disp(%esp)` otherwise.

**Tail calls under PIC depend on callee binding.** A tested external,
default-visibility call such as `return extf(a,2,3);` is `call` + epilogue +
`ret` at -O3. That does **not** imply that PIC disables sibling calls: the
same compiler emits a short `jmp` to a local `static` noinline function and an
`R_386_PC32` `jmp` to a hidden-visibility function. A trailing jump can
therefore be a tail call when the callee is locally bindable. Inspect its
relocation and symbol visibility before classifying it.

**Member calls** — no `__thiscall` on i686: `this` is a plain first stack arg:
```asm
  20: c7 44 24 04 03 00 00 00  movl   $0x3,0x4(%esp)
  28: 8b 45 08                  mov    this,0x8(%ebp),%eax
  2b: 89 04 24                  mov    %eax,(%esp)      ; this
  2e: e8 fc ff ff ff            call   Foo::get2@PLT
```
(`get()` at -O3 just inlines to `mov 0x4(%eax),%eax`.)

**Virtual calls** — slot 0 at -O0: `mov (%eax),%eax; mov (%eax),%eax; mov this,(%esp); call *%eax`; slot 1 at -O0: `mov (%eax),%eax; lea 0x4(%eax),%eax; mov (%eax),%eax; movl $arg,0x4(%esp); call *%eax`. At -O3 the shape differs (NOT identical to -O0): `mov this,%eax; mov (%eax),%edx; mov %eax,(%esp); call *(%edx)` for slot 0 and `call *0x4(%edx)` for slot 1 — the final load is fused into the call operand. No devirtualization at -O3.

**Structs by value:** ≤16 bytes passed in plain 4-byte stack slots, accessed at `8(%ebp)+` directly (Pair=2 slots, Quad=4 slots). 32-byte `BigS` incoming: accessed at `8(%ebp)` (full copy on the stack; incoming by-value structs are never pointer-passed).
**Struct return:** hidden sret pointer as first stack arg (at `8(%ebp)` with a frame, `4(%esp)` at -O3); the callee writes through it, returns the pointer in `%eax`, and pops it with **`ret $4` (`c2 04 00`) for structs of EVERY size** — re-measured with 1-, 4-, 8- and 32-byte structs (mkq/mkr/mkp/mkb all `c2 04 00` at -O0 and -O3). The doc previously claimed `ret $4` only for >8 bytes — wrong; the `c2 04 00` epilogue is the universal sret signature in this fork.

**Non-returning callee** (`__attribute__((noreturn))`): call emitted, everything after is unreachable and eliminated; function just ends with the call (no epilogue, falls into padding).
verified: `g++ -O0/-O3 -c call.cpp; objdump -d`

## 10. Misc

- `__attribute__((section(".text.nomatch")))`: function goes to its own `.text.nomatch` section (objdump prints a separate "Disassembly of section" header) — placement trivial, confirmed.
- **Callee-saved at -O3:** `ebx` (PIC base when calls exist), `esi`/`edi` when register pressure demands (e.g. vectorized loop pushes `edi,esi,ebx`); pushed in reverse at entry, popped before ret. Functions that don't need them save nothing.
- **-O0 register usage:** dominantly `eax/ecx/edx` + stack slots; `ebx` appears only for the PIC base; `esi` only for alloca bookkeeping (`mov %esp,%esi`... actually `mov %esp,%ecx` + `mov %ecx,%esp` restore) and rarely for held values (`shl` example: `mov %edx,%esi; shl %cl,%esi`); `edi` essentially never. Reading -O0 code: expect everything via `%ebp` offsets.
- **cmov/setcc hygiene at -O3:** `xor %eax,%eax` before `setcc %al` when the value is otherwise untouched (partial-register avoidance); `movzbl` used when the setcc result is combined with other values.
verified: `g++ -O0/-O3 -c misc.cpp; objdump -d`

## Cross-checks against real project artifacts

- Carved examples now live at directory-preserving paths such as
  `build/split/nu2api/nufile/nufile.cpp.o` and
  `build/split/legoapi/world/level.cpp.o`. Resolve the exact current path
  through `objdiff.json` before repeating a cross-check.
- `build/CMakeFiles/saga.dir/src/batman.cpp.o` (-O0 rebuild): `NuMain` = `push ebp; mov esp,ebp; push ebx; lea -0x14(%esp),%esp; thunk; addl; mov args to (%esp); call @PLT; lea 0x14(%esp),%esp; pop ebx; pop ebp; ret` — matches §1/§9 exactly.
