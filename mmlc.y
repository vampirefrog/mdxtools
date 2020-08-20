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
#include <stdio.h>
#include <stdlib.h>
#include "mml2mdx.h"
#include "cmdline.h"
#include "tools.h"

int opt_stats = 0;
char *opt_output = 0;

int yylex (void);
extern int line, column, chan_mask;
void yyerror(char const *s) {
	fprintf(stderr, "\n%*s\n%*s\n", column, "^", column, s);
	fprintf(stderr, "%d:%d Parsing error: %s\n", line, column, s);
}

struct mdx_compiler compiler;

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
	  '#' whitespace_optional IDENTIFIER whitespace STRING_LITERAL whitespace_optional '\n' { mdx_compiler_directive(&compiler, $3, $5); }
	| '#' whitespace_optional IDENTIFIER whitespace_optional '\n' { mdx_compiler_directive(&compiler, $3, 0); }
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
		mdx_compiler_opmdef(&compiler, $2, data);
	}
	;

mmldata:
	MMLCHANNELS mmlcommands '\n' {  }

mmlcommands:
	/* empty */
	| mmlcommand mmlcommands;

mmlnote:
	MML_NOTE             { $$ = $1; }
	| MML_NOTE '-'       { $$ = $1 | MML_NOTE_SHARP; }
	| MML_NOTE '+'       { $$ = $1 | MML_NOTE_FLAT; }
	;

mmlcommand:
	  mmlnote notelen     { mdx_compiler_note(&compiler, chan_mask, $1, $2); }
	| 't' INT             { mdx_compiler_tempo(&compiler, chan_mask, $2, 0); }
	| 'o' INT             { mdx_compiler_octave(&compiler, chan_mask, $2); }
	| '<'                 { mdx_compiler_octave_down(&compiler, chan_mask); }
	| '>'                 { mdx_compiler_octave_up(&compiler, chan_mask); }
	| 'n' INT ',' notelen { mdx_compiler_note_num(&compiler, chan_mask, $2, $4); }
	| 'n' INT             { mdx_compiler_note_num(&compiler, chan_mask, $2, 0); }
	| 'l' INT             { mdx_compiler_set_default_note_length(&compiler, chan_mask, $2); }
	| 'r' notelen         { mdx_compiler_rest(&compiler, chan_mask, $2); }
	| 'q' staccato        { mdx_compiler_staccato(&compiler, chan_mask, $2, 0); }
	| '@' 'q' staccato    { mdx_compiler_staccato(&compiler, chan_mask, $3, 1); }
	| '_'                 { mdx_compiler_portamento(&compiler, chan_mask); }
	| '&'                 { mdx_compiler_legato(&compiler, chan_mask); }
	| 'v' INT             { mdx_compiler_volume(&compiler, chan_mask, $2, 0); }
	| '@' 't' INT         { mdx_compiler_tempo(&compiler, chan_mask, $3, 1); }
	| '@' 'v' INT         { mdx_compiler_volume(&compiler, chan_mask, $3, 1); }
	| '@' INT             { mdx_compiler_set_voice(&compiler, chan_mask, $2); }
	| '!'                 { mdx_compiler_end(&compiler, chan_mask); }
	| '('                 { mdx_compiler_volume_down(&compiler, chan_mask); }
	| ')'                 { mdx_compiler_volume_up(&compiler, chan_mask); }
	| 'p' INT             { mdx_compiler_pan(&compiler, chan_mask, $2); }
	| 'k' INT             { mdx_compiler_key_on_delay(&compiler, chan_mask, $2); }
	| 'L'                 { mdx_compiler_loop_start(&compiler, chan_mask); }
	| 'D' INT             { mdx_compiler_detune(&compiler, chan_mask, $2); }
	| 'w' INT             { mdx_compiler_opm_noise_freq(&compiler, chan_mask, $2); }
	| 'y' INT ',' INT     { mdx_compiler_opm_write(&compiler, chan_mask, $2, $4); }
	| 'S' INT             { mdx_compiler_sync_send(&compiler, chan_mask, $2); }
	| 'W'                 { mdx_compiler_sync_wait(&compiler, chan_mask); }
	| 'F' INT             { mdx_compiler_adpcm_freq(&compiler, chan_mask, $2); }
	| MH INT ',' INT ',' INT ',' INT ',' INT ',' INT ',' INT {
	  	mdx_compiler_opm_lfo(&compiler, chan_mask, $2, $4, $6, $8, $10, $12, $14);
	  }
	| MHON                { mdx_compiler_mhon(&compiler, chan_mask); }
	| MHOF                { mdx_compiler_mhof(&compiler, chan_mask); }
	| MA INT ',' INT ',' INT { mdx_compiler_ma(&compiler, chan_mask, $2, $4, $6); }
	| MAON                { mdx_compiler_maon(&compiler, chan_mask); }
	| MAOF                { mdx_compiler_maof(&compiler, chan_mask); }
	| MP INT ',' INT ',' INT { mdx_compiler_mp(&compiler, chan_mask, $2, $4, $6); }
	| MPON                { mdx_compiler_mpon(&compiler, chan_mask); }
	| MPOF                { mdx_compiler_mpof(&compiler, chan_mask); }
	| MD INT              { mdx_compiler_md(&compiler, chan_mask); }
	;

// Fuck this shit
// This is ass
notelen:
	  notelendiv            { $$ = $1; }
	| '%' INT               { $$ = malloc(sizeof(struct mml_notelength)); $$->type = NoteLenTicks; $$->val = $2; }
	| notelen '^' notelen   { $$ = malloc(sizeof(struct mml_notelength)); $$->type = NoteLenJoin; $$->n1 = $1; $$->n2 = $3; }
	;

notelendiv:
	  /* empty */                { $$ = 0; }
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

%%

extern FILE *yyin;

int main(int argc, char **argv) {
	int optind = cmdline_parse_args(argc, argv, (struct cmdline_option[]){
		{
			's', "stats",
			"Dump statistics about compilation",
			0,
			TYPE_SWITCH,
			TYPE_INT, &opt_stats
		},
		{
			'o', "output",
			"Choose output file",
			"file",
			TYPE_REQUIRED,
			TYPE_STRING, &opt_output
		},
		CMDLINE_ARG_TERMINATOR
	}, 1, 1, "<file.mml>");

	if(optind < 0) {
		exit(-1);
	}

	for(int i = optind; i < argc; i++) {
		FILE *f = fopen(argv[i], "r");
		if(!f) {
			fprintf(stderr, "Could not open %s: %s\n", argv[i], strerror(errno));
			continue;
		}


		mdx_compiler_init(&compiler);

		yyin = f;
		int y = yyparse();
		if(y == 1) {
			fprintf(stderr, "parsing failed: invalid input\n");
		} else if(y == 2) {
			fprintf(stderr, "parsing failed: memory exhausted\n");
		} else {
			if(opt_stats)
				mdx_compiler_dump(&compiler);
			char *mdxfilename;
			if(opt_output) {
				mdxfilename = opt_output;
			} else {
				char mdxbuf[256];
				replace_ext(mdxbuf, sizeof(mdxbuf), argv[i], "mdx");
				mdxfilename = mdxbuf;
			}
			mdx_compiler_save(&compiler, mdxfilename);
		}

		mdx_compiler_destroy(&compiler);
	}

	return 0;
}
;