import lief

SYMBOL_BLOCKLIST = set([
    "",
    "__cxa_finalize",
    "_DYNAMIC",
    "_GLOBAL_OFFSET_TABLE_",
    "_end",
    "_edata",
    "__dso_handle",
    "__bss_start",
])

target = lief.parse("res/libTTapp.so")
rewrite = lief.parse("build/saga")

if not target.symbols:
    print("Could not open target")

if not rewrite.symbols:
    print("Could not open rewrite")

target_symbols = {symbol.name: symbol.size for symbol in target.symbols if symbol.name not in SYMBOL_BLOCKLIST}
rewrite_symbols = set(symbol.name for symbol in rewrite.symbols if symbol.name not in SYMBOL_BLOCKLIST)

target_size = sum(symbol.size for symbol in target.symbols)

symbols_rewritten = 0
rewritten_size = 0

for name in rewrite_symbols:
    if name in target_symbols:
        symbols_rewritten += 1
        rewritten_size += target_symbols[name]
        print(f"Rewritten symbol: {name} ({target_symbols[name]} bytes)")

print(f"Symbols rewritten: {symbols_rewritten} / {len(target_symbols)}")
print(f"Rewritten symbols size: {rewritten_size} / {target_size}")
print(f"Progress: {symbols_rewritten / len(target_symbols) * 100:.2f}%")