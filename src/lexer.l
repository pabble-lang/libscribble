%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scribble/parser.h"

#ifdef __DEBUG__
#define DEBUG_PRINT(s) do { fprintf(stderr, (s), yytext); } while (0)
#else
#define DEBUG_PRINT(s) do {} while (0)
#endif
%}

%option noinput
%option nounput
%option yylineno

digit                   [0-9]
digits                  {digit}{digit}*
letter                  [A-Za-z]
identifier              ({letter}|_)({letter}|{digit}|_)*
symbol                  "{"|"}"|"("|")"|"["|"]"|":"|"/"|"\\"|"."|"#"|"&"|"?"|"!"
extidentifier           \"({letter}|_|{symbol})({letter}|{digit}|_|{symbol})*\"

string                  \"[^"]*\"
mlcomment_start         "/*"
mlcomment_body          ([^*])|("*"[^/])|{string}
mlcomment_end           "*/"
mlcomment               {mlcomment_start}{mlcomment_body}*{mlcomment_end}
comment                 "//"([^\n])*\n

%%

    /* Skip whitespace */
[ \t\r]                 ;
\n                      { }

    /* Keywords */
"allreduce"             { DEBUG_PRINT("Token: ALLREDUCE '%s'\n");     return ALLREDUCE;     } /* Pabble */
"and"                   { DEBUG_PRINT("Token: AND '%s'\n");           return AND;           }
"as"                    { DEBUG_PRINT("Token: AS '%s'\n");            return AS;            }
"at"                    { DEBUG_PRINT("Token: AT '%s'\n");            return AT;            }
"by"                    { DEBUG_PRINT("Token: BY '%s'\n");            return BY;            }
"catches"               { DEBUG_PRINT("Token: CATCHES '%s'\n");       return CATCHES;       }
"choice"                { DEBUG_PRINT("Token: CHOICE '%s'\n");        return CHOICE;        }
"const"                 { DEBUG_PRINT("Token: CONST '%s'\n");         return CONST;         } /* Pabble */
"continue"              { DEBUG_PRINT("Token: CONTINUE '%s'\n");      return CONTINUE;      }
"do"                    { DEBUG_PRINT("Token: DO '%s'\n");            return DO;            }
"foreach"               { DEBUG_PRINT("Token: FOREACH '%s'\n");       return FOREACH;       } /* Pabble */
"from"                  { DEBUG_PRINT("Token: FROM '%s'\n");          return FROM;          }
"global"                { DEBUG_PRINT("Token: GLOBAL '%s'\n");        return GLOBAL;        }
"group"                 { DEBUG_PRINT("Token: GROUP '%s'\n");         return GROUP;         } /* Pabble */
"if"                    { DEBUG_PRINT("Token: IF '%s'\n");            return IF;            } /* Pabble */
"import"                { DEBUG_PRINT("Token: IMPORT '%s'\n");        return IMPORT;        }
"in"                    { DEBUG_PRINT("Token: IN '%s'\n");            return IN;            } /* Pabble */
"inf"                   { DEBUG_PRINT("Token: INFinity '%s'\n");      return INF;           } /* Pabble */
"instantiates"          { DEBUG_PRINT("Token: INSTANTIATES '%s'\n");  return INSTANTIATES;  }
"interruptible"         { DEBUG_PRINT("Token: INTERRUPTIBLE '%s'\n"); return INTERRUPTIBLE; }
"is"                    { DEBUG_PRINT("Token: IS '%s'\n");            return IS;            } /* Pabble */
"local"                 { DEBUG_PRINT("Token: LOCAL '%s'\n");         return LOCAL;         }
"module"                { DEBUG_PRINT("Token: MODULE '%s'\n");        return MODULE;        }
"oneof"                 { DEBUG_PRINT("Token: ONEOF '%s'\n");         return ONEOF;         } /* Pabble */
"or"                    { DEBUG_PRINT("Token: OR '%s'\n");            return OR;            }
"par"                   { DEBUG_PRINT("Token: PAR '%s'\n");           return PAR;           }
"protocol"              { DEBUG_PRINT("Token: PROTOCOL '%s'\n");      return PROTOCOL;      }
"range"                 { DEBUG_PRINT("Token: RANGE '%s'\n");         return RANGE;         } /* Pabble */
"rec"                   { DEBUG_PRINT("Token: REC '%s'\n");           return REC;           }
"role"                  { DEBUG_PRINT("Token: ROLE '%s'\n");          return ROLE;          }
"sig"                   { DEBUG_PRINT("Token: SIG '%s'\n");           return SIG;           }
"throws"                { DEBUG_PRINT("Tokens: THROWS '%s'\n");       return THROWS;        }
"to"                    { DEBUG_PRINT("Token: TO '%s'\n");            return TO;            }
"type"                  { DEBUG_PRINT("Token: TYPE '%s'\n");          return TYPE;          }
"with"                  { DEBUG_PRINT("Token: WITH '%s'\n");          return WITH;          }

    /* Symbols */
".."                    { DEBUG_PRINT("Token: NUMRANGE '%s'\n");      return NUMRANGE;      } /* Pabble */
"<<"                    { DEBUG_PRINT("Token: SHL '%s'\n");           return SHL;           } /* Pabble */
">>"                    { DEBUG_PRINT("Token: SHR '%s'\n");           return SHR;           } /* Pabble */
"("                     { DEBUG_PRINT("Token: LPAREN '%s'\n");        return LPAREN;        }
")"                     { DEBUG_PRINT("Token: RPAREN '%s'\n");        return RPAREN;        }
"{"                     { DEBUG_PRINT("Token: LBRACE '%s'\n");        return LBRACE;        }
"}"                     { DEBUG_PRINT("Token: RBRACE '%s'\n");        return RBRACE;        }
"["                     { DEBUG_PRINT("Token: LSQUARE '%s'\n");       return LSQUARE;       }
"]"                     { DEBUG_PRINT("Token: RSQUARE '%s'\n");       return RSQUARE;       }
"<"                     { DEBUG_PRINT("Token: LANGLE '%s'\n");        return LANGLE;        }
">"                     { DEBUG_PRINT("Token: RANGLE '%s'\n");        return RANGLE;        }
","                     { DEBUG_PRINT("Token: COMMA '%s'\n");         return COMMA;         }
":"                     { DEBUG_PRINT("Token: COLON '%s'\n");         return COLON;         }
";"                     { DEBUG_PRINT("Token: SEMICOLON '%s'\n");     return SEMICOLON;     }
"="                     { DEBUG_PRINT("Token: EQUAL '%s'\n");         return EQUAL;         }
"+"                     { DEBUG_PRINT("Token: PLUS '%s'\n");          return PLUS;          }
"-"                     { DEBUG_PRINT("Token: MINUS '%s'\n");         return MINUS;         }
"*"                     { DEBUG_PRINT("Token: MULTIPLY '%s'\n");      return MULTIPLY;      }
"/"                     { DEBUG_PRINT("Token: DIVIDE '%s'\n");        return DIVIDE;        }
"%"                     { DEBUG_PRINT("Token: MODULO '%s'\n");        return MODULO;        }
"."                     { DEBUG_PRINT("Token: DOT '%s'\n");           return DOT;           }

    /* Variables */
{digits}                { yylval.num = atol(yytext); // Unsigned
                          DEBUG_PRINT("Token: DIGITS '%s'\n");        return DIGITS;     }
{identifier}            { yylval.str = strdup(yytext);
                          DEBUG_PRINT("Token: IDENTIFIER '%s'\n");    return IDENTIFIER; }
{mlcomment}             { DEBUG_PRINT("Token: COMMENT '%s'\n");                          }
{comment}               { DEBUG_PRINT("Token: COMMENT '%s'\n");                          }

    /* Catch all */
.                       { fprintf(stderr, "lexer: Don't know what is '%s'\n", yytext); yyterminate(); }

%%

int yywrap()
{
    return 1; // Continue after line wrapping.
}

#undef DEBUG_PRINT
