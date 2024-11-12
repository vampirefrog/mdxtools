%define api.pure full
%locations
%param { yyscan_t yyscanner } { struct mdx_compiler *compiler }

%code top {
}
%code requires {
  #include <stdio.h>
  #include "mdx_compiler.h"
  typedef void* yyscan_t;
}
%code {
  int yylex(YYSTYPE* yylvalp, YYLTYPE* yyllocp, yyscan_t yyscanner, struct mdx_compiler *compiler);
  void yyerror(YYLTYPE* yyllocp, yyscan_t unused, struct mdx_compiler *unused2, const char* msg);
}

%union
{
	char  cval;
	char* sval;
	int   ival;
	struct mml_notelength *nval;
}

%token INT NOTE DIRECTIVE_NAME IDENTIFIER MMLCHANNELS MML_NOTE MML_AT
%token WHITESPACE STRING_LITERAL MP MPON MPOF MA MAON MAOF MD MH MHON MHOF

%type<ival> INT MMLCHANNELS MML_NOTE
%type<sval> DIRECTIVE_NAME IDENTIFIER STRING_LITERAL
%type<cval> NOTE MML_AT
%type<ival> staccato mmlnote
%type<nval> notelen notelendiv

// %parse-param {FILE *f}

%{

void yyerror(YYLTYPE* yyllocp, yyscan_t unused, struct mdx_compiler *unused2, const char* msg) {
	(void)unused;
	(void)unused2;
	fprintf(stderr, "[%d:%d]: %s\n", yyllocp->first_line, yyllocp->first_column, msg);
}

%}

%%

input:
	/* empty */         {}
	| input declaration {}
	;

declaration:
	  directive         {}
	| mmldata           {}
	| opmdef            {}
	;

directive:
	  '#' whitespace_optional IDENTIFIER whitespace STRING_LITERAL whitespace_optional '\n' { mdx_compiler_directive(compiler, $3, $5); }
	| '#' whitespace_optional IDENTIFIER whitespace_optional '\n' { mdx_compiler_directive(compiler, $3, 0); }
	;

opmdef:
	'@' INT '=' '{'
		INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ','
		INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ','
		INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ','
		INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT ','
		INT ',' INT ',' INT
	'}' {
		uint8_t data[47] = {
			 $5,  $7,  $9, $11, $13, $15, $17, $19, $21, $23, $25,
			$27, $29, $31, $33, $35, $37, $39, $41, $43, $45, $47,
			$49, $51, $53, $55, $57, $59, $61, $63, $65, $67, $69,
			$71, $73, $75, $77, $79, $81, $83, $85, $87, $89, $91,
			$93, $95, $97
		};
		mdx_compiler_opmdef(compiler, $2, data);
	}
	;

mmldata:
	MMLCHANNELS mmlcommands '\n' {  }

mmlcommands:
	/* empty */
	| mmlcommand mmlcommands;

mmlnote:
	MML_NOTE             { $$ = $1; }
	| MML_NOTE '-'       { $$ = $1 | MML_NOTE_FLAT; }
	| MML_NOTE '+'       { $$ = $1 | MML_NOTE_SHARP; }
	;

mmlcommand:
	  mmlnote notelen     { mdx_compiler_note(compiler, compiler->chan_mask, $1, $2); }
	| 't' INT             { mdx_compiler_tempo(compiler, compiler->chan_mask, $2, 0); }
	| 'o' INT             { mdx_compiler_octave(compiler, compiler->chan_mask, $2); }
	| '<'                 { mdx_compiler_octave_down(compiler, compiler->chan_mask); }
	| '>'                 { mdx_compiler_octave_up(compiler, compiler->chan_mask); }
	| 'n' INT ',' notelen { mdx_compiler_note_num(compiler, compiler->chan_mask, $2, $4); }
	| 'n' INT             { mdx_compiler_note_num(compiler, compiler->chan_mask, $2, 0); }
	| 'l' notelen         { mdx_compiler_set_default_note_length(compiler, compiler->chan_mask, $2); }
	| 'r' notelen         { mdx_compiler_rest(compiler, compiler->chan_mask, $2); }
	| 'q' staccato        { mdx_compiler_staccato(compiler, compiler->chan_mask, $2, 0); }
	| '@' 'q' staccato    { mdx_compiler_staccato(compiler, compiler->chan_mask, $3, 1); }
	| mmlnote notelen '_' mmlnote { mdx_compiler_portamento(compiler, compiler->chan_mask, $1, $2, $4); }
	| '&'                 { mdx_compiler_legato(compiler, compiler->chan_mask); }
	| 'v' INT             { mdx_compiler_volume(compiler, compiler->chan_mask, $2, 0); }
	| '@' 't' INT         { mdx_compiler_tempo(compiler, compiler->chan_mask, $3, 1); }
	| '@' 'v' INT         { mdx_compiler_volume(compiler, compiler->chan_mask, $3, 1); }
	| '@' INT             { mdx_compiler_set_voice(compiler, compiler->chan_mask, $2); }
	| '!'                 { mdx_compiler_end(compiler, compiler->chan_mask); }
	| '('                 { mdx_compiler_volume_down(compiler, compiler->chan_mask); }
	| ')'                 { mdx_compiler_volume_up(compiler, compiler->chan_mask); }
	| 'p' INT             { mdx_compiler_pan(compiler, compiler->chan_mask, $2); }
	| 'k' INT             { mdx_compiler_key_on_delay(compiler, compiler->chan_mask, $2); }
	| 'L'                 { mdx_compiler_loop_start(compiler, compiler->chan_mask); }
	| 'D' INT             { mdx_compiler_detune(compiler, compiler->chan_mask, $2); }
	| 'w' INT             { mdx_compiler_opm_noise_freq(compiler, compiler->chan_mask, $2); }
	| 'y' INT ',' INT     { mdx_compiler_opm_write(compiler, compiler->chan_mask, $2, $4); }
	| 'S' INT             { mdx_compiler_sync_send(compiler, compiler->chan_mask, $2); }
	| 'W'                 { mdx_compiler_sync_wait(compiler, compiler->chan_mask); }
	| 'F' INT             { mdx_compiler_adpcm_freq(compiler, compiler->chan_mask, $2); }
	| MH INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT {
	  	mdx_compiler_mh(compiler, compiler->chan_mask, $2, $4, $6, $8, $10, $12, $14);
	  }
	| MHON                { mdx_compiler_mhon(compiler, compiler->chan_mask); }
	| MHOF                { mdx_compiler_mhof(compiler, compiler->chan_mask); }
	| MA INT ',' INT ',' INT { mdx_compiler_ma(compiler, compiler->chan_mask, $2, $4, $6); }
	| MAON                { mdx_compiler_maon(compiler, compiler->chan_mask); }
	| MAOF                { mdx_compiler_maof(compiler, compiler->chan_mask); }
	| MP INT ',' INT ',' INT { mdx_compiler_mp(compiler, compiler->chan_mask, $2, $4, $6); }
	| MPON                { mdx_compiler_mpon(compiler, compiler->chan_mask); }
	| MPOF                { mdx_compiler_mpof(compiler, compiler->chan_mask); }
	| MD INT              { mdx_compiler_md(compiler, compiler->chan_mask, $2); }
	| '['                 { mdx_compiler_repeat_start(compiler, compiler->chan_mask); }
	| '/'                 { mdx_compiler_repeat_escape(compiler, compiler->chan_mask); }
	| ']' INT             { mdx_compiler_repeat_end(compiler, compiler->chan_mask, $2); }
	;

// Fuck this shit
// This is ass
notelen:
	  notelendiv            { $$ = $1; }
	| '%' INT               { $$ = malloc(sizeof(struct mml_notelength)); $$->type = NoteLenTicks; $$->val = $2; }
	| notelen '^' notelen   { $$ = malloc(sizeof(struct mml_notelength)); $$->type = NoteLenJoin; $$->n1 = $1; $$->n2 = $3; }
	;

notelendiv:
	  /* empty */           { $$ = 0; }
	| INT                   { $$ = malloc(sizeof(struct mml_notelength)); $$->type = NoteLenInt; $$->val = $1; }
	| notelendiv '.'        { $$ = malloc(sizeof(struct mml_notelength)); $$->type = NoteLenDot; $$->n1 = $1; }
	;

staccato:
	  INT                   { $$ = $1; }
	| '%' INT               { $$ = 128 + $2; }
	;

whitespace:
	WHITESPACE;

whitespace_optional:
	/* empty */ | whitespace;
