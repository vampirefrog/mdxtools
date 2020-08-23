%x opmdef directive mmldata
%x comment
%option stack noyywrap
%option nounput
%option noinput
%top {
	#include <stdint.h>
	#include <string.h>
	#include "mmlc.tab.h"
	void count(void);
	int chan_mask;
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
}
%%

[\t\r\n ]+    { count(); }

"@"         { count(); BEGIN(opmdef); return '@'; }
[A-HP-W]+   { count(); BEGIN(mmldata); chan_mask = yylval.ival = parse_chan_mask(yytext); return MMLCHANNELS; }
"#"         { count(); BEGIN(directive); return '#'; }

<opmdef>{
	[,={]      { count(); return yytext[0]; }
	[0-9]+     { count(); yylval.ival = atoi(yytext); return INT; }
	[\t\r\n ]+ { count(); }
	"}"        { count(); BEGIN(INITIAL); return '}'; }
}

<directive>{
	[a-zA-Z-]+        { count(); yylval.sval = strdup(yytext); return IDENTIFIER; }
	[\t\r ]+          { count(); return WHITESPACE; }
	\'(\\.|[^\\'])+\' { count(); yylval.sval = strdup(yytext+1); yylval.sval[strlen(yylval.sval) - 1] = 0; return STRING_LITERAL; }
	\"(\\.|[^\\"])+\" { count(); yylval.sval = strdup(yytext+1); yylval.sval[strlen(yylval.sval) - 1] = 0; return STRING_LITERAL; }
	\n                { count(); BEGIN(INITIAL); return '\n'; }
}

<mmldata>{
	[\t\r ]+   /* ignore whitespace */
	[a-g]      { count(); yylval.cval = yytext[0] - 'a'; return MML_NOTE; }
	[@to<>lnrq_&\+v()LD\[\]/ykwSWF!,%\.^] { count(); return yytext[0]; }
	-          { count(); return '-'; }
	-?[0-9]+   { count(); yylval.ival = atoi(yytext); return INT; }
	\n         { count(); BEGIN(INITIAL); return '\n'; }
	MHON       { count(); return MHON; }
	MHOF       { count(); return MHOF; }
	MH         { count(); return MH; }
	MAON       { count(); return MAON; }
	MAOF       { count(); return MAOF; }
	MA         { count(); return MA; }
	MPON       { count(); return MPON; }
	MPOF       { count(); return MPOF; }
	MP         { count(); return MP; }
	MD         { count(); return MD; }
	.          { count(); }
}

<*>"/*" yy_push_state(comment);
<comment>{
	[^*\n]*        { count(); } /* eat anything that's not a '*' */
	"*"+[^*/\n]*   { count(); } /* eat up '*'s not followed by '/'s */
	\n             { count(); }
	"*"+"/"        { count(); yy_pop_state(); }
}

%%

int column = 0, line = 1;

void count(void) {
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n') {
			line++;
			column = 0;
		} else if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;

//	ECHO;
}
