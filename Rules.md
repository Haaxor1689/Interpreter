Global -> FuncDef+ `EoF`
FuncDef -> `func` `identifier` Arguments Block
FuncCall -> `identifier` Arguments
Arguments -> `(` `identifier`* `)`
Block -> `{` Statement* `}`
Statement -> ForExpr | IfExpr | WhileExpr | Expression `;`
Expression -> `identifier` IdExpr | `string` | `number`
IdExpr -> FuncCall | `epsilon`
ForExpr -> `for` `identifier` `in` Expression Block
IfExpr -> If ElseIf* Else | If
If -> `if` Expression Block
ElseIf -> `elseif` Expression Block
Else -> `else` Block
WhileExpr -> `while` Expression Block| `do` Block `while` Expression `;`
