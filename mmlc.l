%x opmdef directive mmldata
%x comment

%option noinput nounput noyywrap stack 8bit
%option yylineno
%option reentrant bison-bridge bison-locations

%top {
#include <stdint.h>
#include <string.h>
#include "mmlc.tab.h"
#include "mdx_compiler.h"

#define YY_USER_ACTION                                               \
    yylloc->first_line = yylloc->last_line;                          \
    yylloc->first_column = yylloc->last_column;                      \
    if (yylloc->last_line == yylineno)                               \
      yylloc->last_column += yyleng;                                 \
    else {                                                           \
      yylloc->last_line = yylineno;                                  \
      yylloc->last_column = yytext + yyleng - strrchr(yytext, '\n'); \
    }

#define YY_DECL \
    int yylex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, struct mdx_compiler *compiler)

	int parse_chan_mask(char *str);

#define YY_EXTRA_TYPE struct mdx_compiler *

}

%%

[\t\r\n ]+    ;

"@"         BEGIN(opmdef); return '@';
[A-HP-W]+   { BEGIN(mmldata); yyextra->chan_mask = yylval->ival = parse_chan_mask(yytext); return MMLCHANNELS; }
"#"         BEGIN(directive); return '#';

<opmdef>{
	[,={]      return yytext[0];
	[0-9]+     yylval->ival = atoi(yytext); return INT;
	[\t\r\n ]+ ;
	"}"        BEGIN(INITIAL); return '}';
}

<directive>{
	[a-zA-Z-]+        yylval->sval = strdup(yytext); return IDENTIFIER;
	[\t\r ]+          return WHITESPACE;
	\'(\\.|[^\\'])+\' yylval->sval = strdup(yytext+1); yylval->sval[strlen(yylval->sval) - 1] = 0; return STRING_LITERAL;
	\"(\\.|[^\\"])+\" yylval->sval = strdup(yytext+1); yylval->sval[strlen(yylval->sval) - 1] = 0; return STRING_LITERAL;
	\n                BEGIN(INITIAL); return '\n';
}

<mmldata>{
	[\t\r ]+   /* ignore whitespace */
	[a-g]      yylval->cval = yytext[0] - 'a'; return MML_NOTE;
	[@to<>lnrq_&\+v()LD\[\]/ypkwSWF!,%\.^] return yytext[0];
	-          return '-';
	-?[0-9]+   yylval->ival = atoi(yytext); return INT;
	\n         BEGIN(INITIAL); return '\n';
	MHON       return MHON;
	MHOF       return MHOF;
	MH         return MH;
	MAON       return MAON;
	MAOF       return MAOF;
	MA         return MA;
	MPON       return MPON;
	MPOF       return MPOF;
	MP         return MP;
	MD         return MD;
	.          ;
}

<*>"/*" yy_push_state(comment, compiler->scanner);
<comment>{
	[^*\n]*        ; /* eat anything that's not a '*' */
	"*"+[^*/\n]*   ; /* eat up '*'s not followed by '/'s */
	\n             ;
	"*"+"/"        yy_pop_state(compiler->scanner);
}

%%

int parse_chan_mask(char *str) {
	int mask = 0;
	for(int i = 0; i < strlen(str); i++) {
		if(str[i] >= 'A' && str[i] <= 'H')
			mask |= 1 << (str[i] - 'A');
		else if(str[i] >= 'P' && str[i] <= 'W')
			mask |= (1 << 8) << (str[i] - 'P');
	}
	return mask;
}
