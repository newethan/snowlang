This file gives a comprehensive overview of the snowlang standard.


##################################
# arithmetic operator precedence #
##################################

1. exponentiation
    `**` (POW)
2. unary operators
    `+` (PLUS), `-` (MINUS)
3. multiplication, division, modulo(remainder)
    `*` (MULT), `/` (DIV), `%` (REM)
4. addition, subtraction
    `+` (PLUS), `-` (MINUS)  [both binary]
5. logical or
    `|` (OR)
6. logical and
    `&` (AND)

###########
# grammar #
###########
Terminals are in all caps (e.g IDEN, SEMICOLON)
Non-terminals are all lowercase (e.g instruction, arith_expr)
Like in regex, (a capture group is a sequence enclosed in parenthesis)
`*` - the previous capture group can be repeated 0 or more times.
`+` - the previous capture group can be repeated 1 or more times.
`?` - the previous capture group can be repeated 0 or 1 times.
Footnotes are bellow the gramamr section.
productions are formatted thusly:
(nt is a non terminal, and sq(1), sq(2), ..., sq(n) are sequqnces of terminals and non-terminals)
```
nt
    : sq(1)
    : sq(2)
    ...
    : sq(n)
    ;
```


script
    : (mod | fun | import)*
    ;
mod
    : MOD LPAREN
        (assign (COMMA assign)*)?
        RPAREN IDEN LBRACE block RBRACE
    ;
fun
    : LET LPAREN
        (assign (COMMA assign)*)?
        RPAREN LBRACE block RBRACE
    ;
import
    : IMPORT STRLIT SEMICOLON
    ;
block
    : (instruction)*
    ;
instruction
    : construct
    : connect
    : for_loop
    : while_loop
    : BREAK SEMICOLON
    : CONTINUE SEMICOLON
    : if_statement
    : print
    : tick
    : hold
    : assign SEMICOLON
    ;
construct
    : LET IDEN
        (LPAREN (assign (COMMA assign)*)? RPAREN)?
        IDEN (LBRACK expr RBRACK)? SEMICOLON
    ;
connect
    : CON item item SEMICOLON
    ;
for_loop
    : FOR IDEN IN LPAREN expr COMMA expr RPAREN LBRACE block RBRACE
    ;
while_loop
    : WHILE expr LBRACE block RBRACE
    ;
if_statement
    : IF expr LBRACE block RBRACE
        (ELIF expr LBRACE block RBRACE)*
        (ELSE LBRACE block RBRACE)?
    ;
print
    : PRINT ((STRLIT (COMMA expr)*) | item) SEMICOLON
    ;
tick
    : TICK expr SEMICOLON
    ;
hold
    : HOLD item INT expr SEMICOLON
    ;
assign
    : expr (ASSIGN expr)?              # footnote 1 #
    ;
item
    : IDEN (LBRACK expr RBRACK)? (PERIOD IDEN (LBRACK expr RBRACK)?)*
    ;
expr
    : equality_expr (AND equality_expr)*
    ;
or_expr
    : equality_expr (OR equality_expr)*
    ;
equality_expr
    : ineqality_expr ((EQ|NEQ) ineqality_expr)*
    ;
ineqality_expr
    : arith_expr ((GT|GE|LE|LT) arith_expr)*
    ;
arith_expr
    : term ((PLUS|MINUS) term)*
    ;
term
    : factor ((MULT|DIV|REM) factor)*
    ;
factor
    : (PLUS|MINUS|NOT) factor
    : atom (POW factor)*
    ;
atom
    : IDEN
    : INT
    : FLOAT
    : LPAREN expr RPAREN
    : IDEN LPAREN (assign (COMMA assign)*)? RPAREN
    ;

#####################
# grammar footnotes #
#####################
1.  this production exists to keep the grammar LL(1) and allow
    instructions to be both assignments and expressions followed by a ';';
    if the productions were `instruction -> IDEN ASSIGN expr SEMICOLON`
    and `instruction -> expr SEMICOLON`, then upon encountering an IDEN
    at the beginning of an instruction, the parser would not know which
    production to use. (That is because an expression can also start with
    an IDEN). The production `instruction -> expr (ASSIGN expr)? SEMICOLON`
    allows both to be parsed correctly and unambiguously, but allows odd
    syntax such as `1 + 2 = 4;`. This is of course corrected semantically
    and left-hand expressions of assignments much be the name of a variable.

#######################
# regex of terminals: #
#######################

skipped terminals:
    WHITESPACE : \s+
    COMMENT : #.*(\n|$)

parenthesis, braces and brackets:
    LBRACE : \{
    RBRACE : \}
    LBRACK : \[
    RBRACK : \]
    LPAREN : \(
    RPAREN : \)

delimiters:
    SEMICOLON : ;
    COMMA : ,

operations:
    PERIOD : \.        (member access)
    PLUS : \+
    MINUS : -
    POW : \*\*
    MULT : \*
    DIV : /
    REM : %
    ASSIGN : =(?!=)    (variable assignment)
    OR : \|
    AND : &
    NOT : !(?!=)
    GT : >(?!=)
    GE : >=
    EQ : ==
    NEQ : !=
    LE : <=
    LT : <(?!=)

instructions and keywords for instructions:
    IMPORT : import\b
    MOD : mod\b
    PRINT : print\b
    TICK : tick\b
    HOLD : hold\b
    LET : let\b
    CON : con\b
    IF : if\b
    ELIF : elif\b
    ELSE : else\b
    FOR : for\b
    WHILE : while\b
    BREAK : break\b
    CONTINUE : continue\b
    RETURN : return\b
    IN : in\b

terminals with variable values:
    FLOAT : [0-9]+[.][0-9]+
    INT : \d+
    IDEN : [a-zA-Z_][a-zA-Z_0-9]*\b
    STRLIT : \".*?\"
