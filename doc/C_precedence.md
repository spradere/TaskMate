# 🔢 C Operator Precedence and Associativity Table

| Precedence | Operators | Description | Associativity |
|---|---|---|---|
| 1 | () [] . -> | Function call, array access, struct member | Left-to-right |
| 1 | ++ -- (postfix) |Post-increment, post-decrement | Left-to-right |
| 2 | + - ! ~ ++ -- * & sizeof (_type_) | Unary plus/minus, logical/bitwise NOT, pre-increment/decrement, dereference, address-of, size-of, cast | Right-to-left |
| 3 | * / % | Multiplication, division, modulo | Left-to-right |
| 4 | + - | Addition, subtraction | Left-to-right |
| 5 | << >> | Bitwise left/right shift | Left-to-right |
| 6 | < <= > >= | Relational operators | Left-to-right |
| 7 | == != | Equality operators | right-to-left |
| 8 | & | Bitwise AND | Left-to-right |
| 9 | ^| Bitwise XOR | Left-to-right |
| 10 | \| | Bitwise OR | Left-to-right |
| 11 | && | Logical AND | Left-to-right |
| 12 | \|\| | Logical OR | Left-to-right |
| 13 | ?: | Ternary conditional | Right-to-left |
| 14 | = += -= *= /= %= <<= >>= &= ^= `	=` | Assignment and compound assignment | Left-to-right |
| 15 | , | Comma (sequence) | Left-to-right |

