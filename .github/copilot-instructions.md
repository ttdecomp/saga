# Instructions

See README.md for instructions on how to build the project.
TLDR: `cmake --build build`
Do not use vscode cmake tools.

Ghidra is available via MCP, use it for as much as possible, decompilation, searches, context, etc. You may change stuff in Ghidra.

use `gonk split` to split the compiled binary into its components, and
use `objdiff-cli` to diff the compiled binary against the original binary. You can use `objdiff-cli --help` for more information.

implement functions in the `src` folder in the fitting source file. make sure to keep a logical structure, i.e. terrain stuff does not go into animation files. if you add new function that are different to those already in the source file, consider creating a new source file for them.
MAKE SURE every symbol you add is actually defined in the original binary (!), watch out for name mangling!. make sure you DO NOT remove existing symbols!

you may use standard linux binutils like `nm`, `objdump`, `readelf` to inspect the original as well as the compiled binary and its symbols. do not assume the ghidra decompilation is always correct, the original binary is the source of truth.

100% matching is the goal, but low single digit percentages are acceptable if i.e the function is big or there are too many unknowns or missing symbols. if you are unsure about a function, leave it as a stub and move on to the next one.

do not only implement bullshit boilerplate code, try to implement actual code, actual logic, actual functionality. be not afraid of big functions, reason about them some time if you have to, make educated guesses and some trial and error. if you really are unsure about a function, leave it as good as you can.

do not use any undefined or unknown variables/fields/etc. do not use random ass writes or reads or array acccesses to pointer or to random offsets, create structs, fields, enums, whatever seems fitting. do not use volatile or asm to force stuff.

if you require context look into ghidra first, search for stuff, then look at the existing code and other files, then at the original binary.

objdiff-cli can generate a report. at the very start of the report it specifies a global matching percentage. this is a good indicator of overall progress. check its value before you start and after you are done.

if you are unsure about something, re-read these instructions.
