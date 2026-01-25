# Scanning Process
- `yyin` : input (default = STDIN)
- `yylex()` : To start scanning (reads yyin, called by main() or yyparse())
- `yytext` : contains the matched string from the token
- `yyout` : Output stream (default = STDOUT)
- `yyrestart(new file)` : Clears internal buffers, and restarts the scanner to read from the new file
- `yywrap()` : called by yylex(), when the EOF is reached
- `yyleng` : len of the string stored in yytext
- `yylineno` : tracks the current line number (needs `%option yylineno`)
- `yyerror(char *)`

# THE 3 PARTSSSSSSS

## Defs and Macros
```lex
%{
#include <stdio.h>
%}
```

## Translation Rules
```lex
```

## User defined auxiliary procs (main func, redefine yywrap())

# Regex Stuff

- `^` match the beginning of a line
- `$` match the end of the line
- `r{m, n}` m = min reps, and n = max reps

