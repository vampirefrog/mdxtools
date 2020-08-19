/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */


/* Line 189 of yacc.c  */
#line 72 "mmlc.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT = 258,
     NOTE = 259,
     DIRECTIVE_NAME = 260,
     IDENTIFIER = 261,
     MMLCHANNELS = 262,
     MML_NOTE = 263,
     MML_AT = 264,
     WHITESPACE = 265,
     STRING_LITERAL = 266,
     MP = 267,
     MPON = 268,
     MPOF = 269,
     MA = 270,
     MAON = 271,
     MAOF = 272,
     MD = 273,
     MH = 274,
     MHON = 275,
     MHOF = 276
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 2 "mmlc.y"

	char  cval;
	char* sval;
	int   ival;
	struct mml_notelength *nval;



/* Line 214 of yacc.c  */
#line 138 "mmlc.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */

/* Line 264 of yacc.c  */
#line 18 "mmlc.y"

#include <stdio.h>
#include "mml2mdx.h"

int yylex (void);
extern int line, column, chan_mask;
void yyerror(char const *s) {
	fprintf(stderr, "\n%*s\n%*s\n", column, "^", column, s);
	fprintf(stderr, "%d:%d Parsing error: %s\n", line, column, s);
}

struct mdx_compiler compiler;



/* Line 264 of yacc.c  */
#line 166 "mmlc.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   272

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  57
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  14
/* YYNRULES -- Number of rules.  */
#define YYNRULES  65
/* YYNRULES -- Number of states.  */
#define YYNSTATES  219

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   276

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      23,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    42,     2,    22,     2,    54,    40,     2,
      43,    44,     2,    30,    27,    29,    56,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      33,    25,    34,     2,    24,     2,     2,     2,    48,     2,
      53,     2,     2,     2,     2,     2,    47,     2,     2,     2,
       2,     2,     2,    51,     2,     2,     2,    52,     2,     2,
       2,     2,     2,     2,    55,    39,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    46,    36,     2,
      35,    32,    45,    38,    37,     2,    31,     2,    41,    49,
       2,    50,     2,    26,     2,    28,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    21,    27,
     126,   130,   131,   134,   136,   139,   142,   145,   148,   151,
     153,   155,   160,   163,   166,   169,   172,   176,   178,   180,
     183,   187,   191,   194,   196,   198,   200,   203,   206,   208,
     211,   214,   219,   222,   224,   227,   242,   244,   246,   253,
     255,   257,   264,   266,   268,   271,   273,   276,   280,   281,
     283,   286,   288,   291,   293,   294
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      58,     0,    -1,    -1,    58,    59,    -1,    60,    -1,    62,
      -1,    61,    -1,    22,    70,     6,    69,    11,    70,    23,
      -1,    22,    70,     6,    70,    23,    -1,    24,     3,    25,
      26,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    28,    -1,     7,    63,    23,    -1,
      -1,    65,    63,    -1,     8,    -1,     8,    29,    -1,     8,
      30,    -1,    64,    66,    -1,    31,     3,    -1,    32,     3,
      -1,    33,    -1,    34,    -1,    35,     3,    27,    66,    -1,
      35,     3,    -1,    36,     3,    -1,    37,    66,    -1,    38,
      68,    -1,    24,    38,    68,    -1,    39,    -1,    40,    -1,
      41,     3,    -1,    24,    31,     3,    -1,    24,    41,     3,
      -1,    24,     3,    -1,    42,    -1,    43,    -1,    44,    -1,
      45,     3,    -1,    46,     3,    -1,    47,    -1,    48,     3,
      -1,    49,     3,    -1,    50,     3,    27,     3,    -1,    51,
       3,    -1,    52,    -1,    53,     3,    -1,    19,     3,    27,
       3,    27,     3,    27,     3,    27,     3,    27,     3,    27,
       3,    -1,    20,    -1,    21,    -1,    15,     3,    27,     3,
      27,     3,    -1,    16,    -1,    17,    -1,    12,     3,    27,
       3,    27,     3,    -1,    13,    -1,    14,    -1,    18,     3,
      -1,    67,    -1,    54,     3,    -1,    66,    55,    66,    -1,
      -1,     3,    -1,    67,    56,    -1,     3,    -1,    54,     3,
      -1,    10,    -1,    -1,    69,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    36,    36,    37,    41,    42,    43,    47,    48,    52,
      71,    73,    75,    78,    79,    80,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   130,   131,   132,   136,   137,
     138,   142,   143,   147,   149,   150
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT", "NOTE", "DIRECTIVE_NAME",
  "IDENTIFIER", "MMLCHANNELS", "MML_NOTE", "MML_AT", "WHITESPACE",
  "STRING_LITERAL", "MP", "MPON", "MPOF", "MA", "MAON", "MAOF", "MD", "MH",
  "MHON", "MHOF", "'#'", "'\\n'", "'@'", "'='", "'{'", "','", "'}'", "'-'",
  "'+'", "'t'", "'o'", "'<'", "'>'", "'n'", "'l'", "'r'", "'q'", "'_'",
  "'&'", "'v'", "'!'", "'('", "')'", "'p'", "'k'", "'L'", "'D'", "'w'",
  "'y'", "'S'", "'W'", "'F'", "'%'", "'^'", "'.'", "$accept", "input",
  "declaration", "directive", "opmdef", "mmldata", "mmlcommands",
  "mmlnote", "mmlcommand", "notelen", "notelendiv", "staccato",
  "whitespace", "whitespace_optional", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,    35,    10,    64,    61,   123,    44,   125,    45,
      43,   116,   111,    60,    62,   110,   108,   114,   113,    95,
      38,   118,    33,    40,    41,   112,   107,    76,    68,   119,
     121,    83,    87,    70,    37,    94,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    57,    58,    58,    59,    59,    59,    60,    60,    61,
      62,    63,    63,    64,    64,    64,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    66,    66,    66,    67,    67,
      67,    68,    68,    69,    70,    70
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     7,     5,    98,
       3,     0,     2,     1,     2,     2,     2,     2,     2,     1,
       1,     4,     2,     2,     2,     2,     3,     1,     1,     2,
       3,     3,     2,     1,     1,     1,     2,     2,     1,     2,
       2,     4,     2,     1,     2,    14,     1,     1,     6,     1,
       1,     6,     1,     1,     2,     1,     2,     3,     0,     1,
       2,     1,     2,     1,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,    11,    64,     0,     3,     4,     6,     5,
      13,     0,    52,    53,     0,    49,    50,     0,     0,    46,
      47,     0,     0,     0,    19,    20,     0,     0,    58,     0,
      27,    28,     0,    33,    34,    35,     0,     0,    38,     0,
       0,     0,     0,    43,     0,     0,    58,    11,    63,    65,
       0,     0,    14,    15,     0,     0,    54,     0,    32,     0,
       0,     0,    17,    18,    22,    23,    59,     0,    24,    55,
      61,     0,    25,    29,    36,    37,    39,    40,     0,    42,
      44,    10,    16,    12,    64,     0,     0,     0,     0,    30,
      26,    31,    58,    56,    58,    60,    62,     0,    65,     0,
       0,     0,     0,     0,    21,    57,    41,    64,     8,     0,
       0,     0,     0,     0,     0,    51,    48,     0,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    45,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     9
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     6,     7,     8,     9,    45,    46,    47,    68,
      69,    72,    49,    50
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -80
static const yytype_int16 yypact[] =
{
     -80,     3,   -80,    22,    -6,     5,   -80,   -80,   -80,   -80,
     -23,     6,   -80,   -80,     8,   -80,   -80,     9,    10,   -80,
     -80,    46,    11,    12,   -80,   -80,    13,    14,    -3,    -2,
     -80,   -80,    15,   -80,   -80,   -80,    16,    17,   -80,    18,
      19,    20,    21,   -80,    23,    24,    -3,    22,   -80,   -80,
      25,     4,   -80,   -80,    49,    51,   -80,    52,   -80,    29,
      -2,    30,   -80,   -80,    53,   -80,   -80,    41,   -10,    26,
     -80,    78,   -80,   -80,   -80,   -80,   -80,   -80,    56,   -80,
     -80,   -80,   -10,   -80,    -6,    59,    83,    85,    86,   -80,
     -80,   -80,    -3,   -80,    -3,   -80,   -80,    87,    80,    69,
      90,    67,    68,    70,   -10,   -10,   -80,    -6,   -80,    71,
      93,    96,    97,    79,    98,   -80,   -80,    76,   -80,    77,
     102,   103,    81,    82,   104,   107,    84,    88,   109,   110,
      89,    91,   111,   114,   -80,    92,   117,    94,   119,    99,
     120,   100,   121,   101,   122,   105,   126,   106,   127,   108,
     128,   112,   131,   113,   133,   115,   134,   116,   135,   118,
     138,   123,   141,   124,   143,   125,   144,   129,   145,   130,
     146,   132,   150,   136,   151,   137,   152,   139,   155,   140,
     157,   142,   158,   147,   159,   148,   162,   149,   165,   153,
     167,   154,   168,   156,   169,   160,   170,   161,   174,   163,
     175,   164,   176,   166,   179,   171,   181,   172,   182,   173,
     183,   177,   186,   178,   189,   180,   191,   184,   -80
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -80,   -80,   -80,   -80,   -80,   -80,   185,   -80,   -80,   -44,
     -80,   187,   188,   -79
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      66,    70,    82,     2,    48,    99,    52,    53,    51,    54,
       3,    55,    56,    57,    62,    63,    64,    65,    73,    74,
      75,    76,    77,    78,    79,     4,    80,     5,   113,    85,
      10,    84,    89,    91,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    93,    94,    21,    81,   104,    58,
     105,    67,    71,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    86,    59,    87,    88,
      92,    96,    95,    97,    60,   100,   101,    61,   102,   103,
     106,   107,   108,   109,   110,   111,   115,   112,   114,   116,
     117,   119,   118,   120,   121,   122,   123,   126,   124,   125,
     127,   128,   130,   131,   134,   129,   132,   135,   133,   136,
     137,   138,   139,   141,   143,   145,   140,   142,   144,   147,
     149,   151,   146,   148,   153,   150,   155,   157,   159,   152,
     154,   161,   156,   158,   163,   160,   165,   167,   169,   171,
     162,   164,   166,   173,   175,   177,   168,   170,   179,   172,
     181,   183,   185,   174,   176,   187,   178,   180,   189,   182,
     191,   193,   195,   197,   184,   186,   188,   199,   201,   203,
     190,   192,   205,   194,   207,   209,   211,   196,   198,   213,
     200,   202,   215,   204,   217,     0,     0,     0,   206,   208,
     210,     0,     0,     0,   212,   214,     0,   216,     0,     0,
       0,     0,   218,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    83,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    90,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    98
};

static const yytype_int8 yycheck[] =
{
       3,     3,    46,     0,    10,    84,    29,    30,     3,     3,
       7,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,    22,     3,    24,   107,    25,
       8,     6,     3,     3,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,     3,    55,    24,    23,    92,     3,
      94,    54,    54,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    27,    31,    27,    27,
      27,     3,    56,    27,    38,    26,     3,    41,     3,     3,
       3,    11,    23,     3,    27,    27,     3,    27,    27,     3,
       3,     3,    23,    27,    27,     3,     3,     3,    27,    27,
       3,    27,     3,     3,     3,    27,    27,     3,    27,    27,
       3,    27,     3,     3,     3,     3,    27,    27,    27,     3,
       3,     3,    27,    27,     3,    27,     3,     3,     3,    27,
      27,     3,    27,    27,     3,    27,     3,     3,     3,     3,
      27,    27,    27,     3,     3,     3,    27,    27,     3,    27,
       3,     3,     3,    27,    27,     3,    27,    27,     3,    27,
       3,     3,     3,     3,    27,    27,    27,     3,     3,     3,
      27,    27,     3,    27,     3,     3,     3,    27,    27,     3,
      27,    27,     3,    27,     3,    -1,    -1,    -1,    27,    27,
      27,    -1,    -1,    -1,    27,    27,    -1,    27,    -1,    -1,
      -1,    -1,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    84
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    58,     0,     7,    22,    24,    59,    60,    61,    62,
       8,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    24,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    63,    64,    65,    10,    69,
      70,     3,    29,    30,     3,     3,     3,     3,     3,    31,
      38,    41,     3,     3,     3,     3,     3,    54,    66,    67,
       3,    54,    68,     3,     3,     3,     3,     3,     3,     3,
       3,    23,    66,    63,     6,    25,    27,    27,    27,     3,
      68,     3,    27,     3,    55,    56,     3,    27,    69,    70,
      26,     3,     3,     3,    66,    66,     3,    11,    23,     3,
      27,    27,    27,    70,    27,     3,     3,     3,    23,     3,
      27,    27,     3,     3,    27,    27,     3,     3,    27,    27,
       3,     3,    27,    27,     3,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    27,     3,
      27,     3,    27,     3,    27,     3,    27,     3,    28
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1464 of yacc.c  */
#line 36 "mmlc.y"
    {;}
    break;

  case 3:

/* Line 1464 of yacc.c  */
#line 37 "mmlc.y"
    {;}
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 41 "mmlc.y"
    {;}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 42 "mmlc.y"
    {;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 43 "mmlc.y"
    {;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 47 "mmlc.y"
    { mdx_compiler_directive(&compiler, (yyvsp[(3) - (7)].sval), (yyvsp[(5) - (7)].sval)); ;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 48 "mmlc.y"
    { mdx_compiler_directive(&compiler, (yyvsp[(3) - (5)].sval), 0); ;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 58 "mmlc.y"
    {
		uint8_t data[47] = {
			 (yyvsp[(5) - (98)].ival),  (yyvsp[(7) - (98)].ival),  (yyvsp[(9) - (98)].ival), (yyvsp[(11) - (98)].ival), (yyvsp[(13) - (98)].ival), (yyvsp[(15) - (98)].ival), (yyvsp[(17) - (98)].ival), (yyvsp[(19) - (98)].ival), (yyvsp[(21) - (98)].ival), (yyvsp[(23) - (98)].ival), (yyvsp[(25) - (98)].ival),
			(yyvsp[(27) - (98)].ival), (yyvsp[(29) - (98)].ival), (yyvsp[(31) - (98)].ival), (yyvsp[(33) - (98)].ival), (yyvsp[(35) - (98)].ival), (yyvsp[(37) - (98)].ival), (yyvsp[(39) - (98)].ival), (yyvsp[(41) - (98)].ival), (yyvsp[(43) - (98)].ival), (yyvsp[(45) - (98)].ival), (yyvsp[(47) - (98)].ival),
			(yyvsp[(49) - (98)].ival), (yyvsp[(51) - (98)].ival), (yyvsp[(53) - (98)].ival), (yyvsp[(55) - (98)].ival), (yyvsp[(57) - (98)].ival), (yyvsp[(59) - (98)].ival), (yyvsp[(61) - (98)].ival), (yyvsp[(63) - (98)].ival), (yyvsp[(65) - (98)].ival), (yyvsp[(67) - (98)].ival), (yyvsp[(69) - (98)].ival),
			(yyvsp[(71) - (98)].ival), (yyvsp[(73) - (98)].ival), (yyvsp[(75) - (98)].ival), (yyvsp[(77) - (98)].ival), (yyvsp[(79) - (98)].ival), (yyvsp[(81) - (98)].ival), (yyvsp[(83) - (98)].ival), (yyvsp[(85) - (98)].ival), (yyvsp[(87) - (98)].ival), (yyvsp[(89) - (98)].ival), (yyvsp[(91) - (98)].ival),
			(yyvsp[(93) - (98)].ival), (yyvsp[(95) - (98)].ival), (yyvsp[(97) - (98)].ival)
		};
		mdx_compiler_opmdef(&compiler, (yyvsp[(2) - (98)].ival), data);
	;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 71 "mmlc.y"
    {  ;}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 78 "mmlc.y"
    { (yyval.ival) = (yyvsp[(1) - (1)].ival); ;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 79 "mmlc.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) | MML_NOTE_SHARP; ;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 80 "mmlc.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) | MML_NOTE_FLAT; ;}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 84 "mmlc.y"
    { mdx_compiler_note(&compiler, chan_mask, (yyvsp[(1) - (2)].ival), (yyvsp[(2) - (2)].nval)); ;}
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 85 "mmlc.y"
    { mdx_compiler_tempo(&compiler, chan_mask, (yyvsp[(2) - (2)].ival), 0); ;}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 86 "mmlc.y"
    { mdx_compiler_octave(&compiler, chan_mask, (yyvsp[(2) - (2)].ival)); ;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 87 "mmlc.y"
    { mdx_compiler_octave_down(&compiler, chan_mask); ;}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 88 "mmlc.y"
    { mdx_compiler_octave_up(&compiler, chan_mask); ;}
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 89 "mmlc.y"
    { mdx_compiler_note_num(&compiler, chan_mask, (yyvsp[(2) - (4)].ival), (yyvsp[(4) - (4)].nval)); ;}
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 90 "mmlc.y"
    { mdx_compiler_note_num(&compiler, chan_mask, (yyvsp[(2) - (2)].ival), 0); ;}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 91 "mmlc.y"
    { mdx_compiler_set_default_note_length(&compiler, chan_mask, (yyvsp[(2) - (2)].ival)); ;}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 92 "mmlc.y"
    { mdx_compiler_rest(&compiler, chan_mask, (yyvsp[(2) - (2)].nval)); ;}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 93 "mmlc.y"
    { mdx_compiler_staccato(&compiler, chan_mask, (yyvsp[(2) - (2)].ival), 0); ;}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 94 "mmlc.y"
    { mdx_compiler_staccato(&compiler, chan_mask, (yyvsp[(3) - (3)].ival), 1); ;}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 95 "mmlc.y"
    { mdx_compiler_portamento(&compiler, chan_mask); ;}
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 96 "mmlc.y"
    { mdx_compiler_legato(&compiler, chan_mask); ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 97 "mmlc.y"
    { mdx_compiler_volume(&compiler, chan_mask, (yyvsp[(2) - (2)].ival), 0); ;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 98 "mmlc.y"
    { mdx_compiler_tempo(&compiler, chan_mask, (yyvsp[(3) - (3)].ival), 1); ;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 99 "mmlc.y"
    { mdx_compiler_volume(&compiler, chan_mask, (yyvsp[(3) - (3)].ival), 1); ;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 100 "mmlc.y"
    { mdx_compiler_set_voice(&compiler, chan_mask, (yyvsp[(2) - (2)].ival)); ;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 101 "mmlc.y"
    { mdx_compiler_end(&compiler, chan_mask); ;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 102 "mmlc.y"
    { mdx_compiler_volume_down(&compiler, chan_mask); ;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 103 "mmlc.y"
    { mdx_compiler_volume_up(&compiler, chan_mask); ;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 104 "mmlc.y"
    { mdx_compiler_pan(&compiler, chan_mask, (yyvsp[(2) - (2)].ival)); ;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 105 "mmlc.y"
    { mdx_compiler_key_on_delay(&compiler, chan_mask, (yyvsp[(2) - (2)].ival)); ;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 106 "mmlc.y"
    { mdx_compiler_loop_start(&compiler, chan_mask); ;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 107 "mmlc.y"
    { mdx_compiler_detune(&compiler, chan_mask, (yyvsp[(2) - (2)].ival)); ;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 108 "mmlc.y"
    { mdx_compiler_opm_noise_freq(&compiler, chan_mask, (yyvsp[(2) - (2)].ival)); ;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 109 "mmlc.y"
    { mdx_compiler_opm_write(&compiler, chan_mask, (yyvsp[(2) - (4)].ival), (yyvsp[(4) - (4)].ival)); ;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 110 "mmlc.y"
    { mdx_compiler_sync_send(&compiler, chan_mask, (yyvsp[(2) - (2)].ival)); ;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 111 "mmlc.y"
    { mdx_compiler_sync_wait(&compiler, chan_mask); ;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 112 "mmlc.y"
    { mdx_compiler_adpcm_freq(&compiler, chan_mask, (yyvsp[(2) - (2)].ival)); ;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 113 "mmlc.y"
    {
	  	mdx_compiler_opm_lfo(&compiler, chan_mask, (yyvsp[(2) - (14)].ival), (yyvsp[(4) - (14)].ival), (yyvsp[(6) - (14)].ival), (yyvsp[(8) - (14)].ival), (yyvsp[(10) - (14)].ival), (yyvsp[(12) - (14)].ival), (yyvsp[(14) - (14)].ival));
	  ;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 116 "mmlc.y"
    { mdx_compiler_mhon(&compiler, chan_mask); ;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 117 "mmlc.y"
    { mdx_compiler_mhof(&compiler, chan_mask); ;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 118 "mmlc.y"
    { mdx_compiler_ma(&compiler, chan_mask, (yyvsp[(2) - (6)].ival), (yyvsp[(4) - (6)].ival), (yyvsp[(6) - (6)].ival)); ;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 119 "mmlc.y"
    { mdx_compiler_maon(&compiler, chan_mask); ;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 120 "mmlc.y"
    { mdx_compiler_maof(&compiler, chan_mask); ;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 121 "mmlc.y"
    { mdx_compiler_mp(&compiler, chan_mask, (yyvsp[(2) - (6)].ival), (yyvsp[(4) - (6)].ival), (yyvsp[(6) - (6)].ival)); ;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 122 "mmlc.y"
    { mdx_compiler_mpon(&compiler, chan_mask); ;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 123 "mmlc.y"
    { mdx_compiler_mpof(&compiler, chan_mask); ;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 124 "mmlc.y"
    { mdx_compiler_md(&compiler, chan_mask); ;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 130 "mmlc.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 131 "mmlc.y"
    { (yyval.nval) = malloc(sizeof(struct mml_notelength)); (yyval.nval)->type = NoteLenTicks; (yyval.nval)->val = (yyvsp[(2) - (2)].ival); ;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 132 "mmlc.y"
    { (yyval.nval) = malloc(sizeof(struct mml_notelength)); (yyval.nval)->type = NoteLenJoin; (yyval.nval)->n1 = (yyvsp[(1) - (3)].nval); (yyval.nval)->n2 = (yyvsp[(3) - (3)].nval); ;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 136 "mmlc.y"
    { (yyval.nval) = 0; ;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 137 "mmlc.y"
    { (yyval.nval) = malloc(sizeof(struct mml_notelength)); (yyval.nval)->type = NoteLenInt; (yyval.nval)->val = (yyvsp[(1) - (1)].ival); ;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 138 "mmlc.y"
    { (yyval.nval) = malloc(sizeof(struct mml_notelength)); (yyval.nval)->type = NoteLenDot; (yyval.nval)->n1 = (yyvsp[(1) - (2)].nval); ;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 142 "mmlc.y"
    { (yyval.ival) = (yyvsp[(1) - (1)].ival); ;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 143 "mmlc.y"
    { (yyval.ival) = 128 + (yyvsp[(2) - (2)].ival); ;}
    break;



/* Line 1464 of yacc.c  */
#line 1963 "mmlc.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1684 of yacc.c  */
#line 152 "mmlc.y"


int main(int argc, char **argv) {
	mdx_compiler_init(&compiler);
	yyparse();
	mdx_compiler_dump(&compiler);
	mdx_compiler_save(&compiler, "mml2mdx.mdx");

	return 0;
}

