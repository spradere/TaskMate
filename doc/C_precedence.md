# C Operator Precedence and Associativity Table

| Precedence | Operators | Description | Associativity |
|---|---|---|---| 
| 1 | () [] . -> | Function call, array access, struct member | Left-to-right |
| 2 | ++ -- (postfix) |Post-increment, post-decrement | Left-to-right |
| 3 | + - ! ~ ++ -- * & sizeof (_type_) | Unary plus/minus, logical/bitwise NOT, pre-inc/dec, dereference, address-of, size-of, cast | Right-to-left |
| 4 | * / % | Multiplication, division, modulo | Left-to-right |
| 5 | + - | Addition, subtraction | Left-to-right |
| 6 | << >> | Bitwise left/right shift | Left-to-right |
| 7 | < <= > >= | Relational operators | Left-to-right |
| 8 | == != | Equality operators | Left-to-right |
| 9 | & | Bitwise AND | Left-to-right |
| 10 | ^| Bitwise XOR | Left-to-right |
| 11 | \| | Bitwise OR | Left-to-right |
| 12 | && | Logical AND | Left-to-right |
| 13 | \|\| | Logical OR | Left-to-righ |
| 14 | ?: | Ternary conditional | Right-to-left |
| 15 | = += -= *= /= %= <<= >>= &= ^= `	=` | Assignment and compound assignment | Left-to-right |
| 16 | , | Comma (sequence) | Left-to-right |

