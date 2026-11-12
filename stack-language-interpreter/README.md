# Stack Language Interpreter (C++)

An interpreter for a small stack-based esoteric programming language.

The program reads two lines: the **program** (each character is one instruction) and the **input** string. It executes the program on a stack whose elements are lists of characters.

## Implementation highlights
- Custom singly linked list (`CharList`) and stack (`Stack`) – no STL containers
- Manual memory management (`new` / `delete`)
- Recursion used throughout – list operations, stack traversal and the main execution loop
- Numbers are stored as character lists and converted for arithmetic

## Supported instructions
| Group | Instructions |
|---|---|
| Stack operations | `'` push empty list · `:` duplicate · `;` swap · `@` copy the n-th element · `,` pop |
| Input / output | `.` read a character from input · `>` print a character · `&` print the whole stack |
| Arithmetic & logic | `+` add · `-` negate · `^` absolute value · `<` less than · `=` equal · `!` logical NOT |
| Characters | `$` split off the first character · `#` concatenate · `[` char → ASCII code · `]` ASCII code → char |
| Control flow | `~` push the current instruction index · `?` conditional jump |
| Anything else | appended to the list on top of the stack |

## How to run
```bash
g++ -O2 -o interpreter main.cpp
./interpreter
```
Then enter the program on the first line and the input on the second.
