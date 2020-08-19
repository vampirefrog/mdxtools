/* A Bison parser, made by GNU Bison 3.3.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.3.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1





# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "mml.tab.h".  */
#ifndef YY_YY_MML_TAB_H_INCLUDED
# define YY_YY_MML_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 5 "mml.y" /* yacc.c:352  */

	char  cval;
	char* sval;
	int   ival;
	struct mml_notelength *nval;

#line 141 "mml.tab.c" /* yacc.c:352  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_MML_TAB_H_INCLUDED  */

/* Second part of user prologue.  */
#line 21 "mml.y" /* yacc.c:354  */

#include <stdio.h>
#include "mml2mdx.h"

int yylex (void);
extern int line, column, chan_mask;
void yyerror(char const *s) {
	fprintf(stderr, "\n%*s\n%*s\n", column, "^", column, s);
	fprintf(stderr, "%d:%d Parsing error: %s\n", line, column, s);
}

struct mdx_compiler compiler;


#line 172 "mml.tab.c" /* yacc.c:354  */

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
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
# define YYCOPY_NEEDED 1
#endif


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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

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
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  219

#define YYUNDEFTOK  2
#define YYMAXUTOK   276

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
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
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    39,    39,    40,    44,    45,    46,    50,    51,    55,
      74,    77,    78,    81,    82,    83,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   133,   134,   135,   139,   140,
     141,   145,   146,   150,   153,   153
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
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
  "whitespace", "whitespace_optional", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
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

#define YYPACT_NINF -80

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-80)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
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

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
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

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -80,   -80,   -80,   -80,   -80,   -80,   185,   -80,   -80,   -44,
     -80,   187,   188,   -79
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     6,     7,     8,     9,    45,    46,    47,    68,
      69,    72,    49,    50
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
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

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
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


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        YY_LAC_DISCARD ("YYBACKUP");                              \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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

/* Given a state stack such that *YYBOTTOM is its bottom, such that
   *YYTOP is either its top or is YYTOP_EMPTY to indicate an empty
   stack, and such that *YYCAPACITY is the maximum number of elements it
   can hold without a reallocation, make sure there is enough room to
   store YYADD more elements.  If not, allocate a new stack using
   YYSTACK_ALLOC, copy the existing elements, and adjust *YYBOTTOM,
   *YYTOP, and *YYCAPACITY to reflect the new capacity and memory
   location.  If *YYBOTTOM != YYBOTTOM_NO_FREE, then free the old stack
   using YYSTACK_FREE.  Return 0 if successful or if no reallocation is
   required.  Return 1 if memory is exhausted.  */
static int
yy_lac_stack_realloc (YYSIZE_T *yycapacity, YYSIZE_T yyadd,
#if YYDEBUG
                      char const *yydebug_prefix,
                      char const *yydebug_suffix,
#endif
                      yytype_int16 **yybottom,
                      yytype_int16 *yybottom_no_free,
                      yytype_int16 **yytop, yytype_int16 *yytop_empty)
{
  YYSIZE_T yysize_old =
    (YYSIZE_T) (*yytop == yytop_empty ? 0 : *yytop - *yybottom + 1);
  YYSIZE_T yysize_new = yysize_old + yyadd;
  if (*yycapacity < yysize_new)
    {
      YYSIZE_T yyalloc = 2 * yysize_new;
      yytype_int16 *yybottom_new;
      /* Use YYMAXDEPTH for maximum stack size given that the stack
         should never need to grow larger than the main state stack
         needs to grow without LAC.  */
      if (YYMAXDEPTH < yysize_new)
        {
          YYDPRINTF ((stderr, "%smax size exceeded%s", yydebug_prefix,
                      yydebug_suffix));
          return 1;
        }
      if (YYMAXDEPTH < yyalloc)
        yyalloc = YYMAXDEPTH;
      yybottom_new =
        (yytype_int16*) YYSTACK_ALLOC (yyalloc * sizeof *yybottom_new);
      if (!yybottom_new)
        {
          YYDPRINTF ((stderr, "%srealloc failed%s", yydebug_prefix,
                      yydebug_suffix));
          return 1;
        }
      if (*yytop != yytop_empty)
        {
          YYCOPY (yybottom_new, *yybottom, yysize_old);
          *yytop = yybottom_new + (yysize_old - 1);
        }
      if (*yybottom != yybottom_no_free)
        YYSTACK_FREE (*yybottom);
      *yybottom = yybottom_new;
      *yycapacity = yyalloc;
    }
  return 0;
}

/* Establish the initial context for the current lookahead if no initial
   context is currently established.

   We define a context as a snapshot of the parser stacks.  We define
   the initial context for a lookahead as the context in which the
   parser initially examines that lookahead in order to select a
   syntactic action.  Thus, if the lookahead eventually proves
   syntactically unacceptable (possibly in a later context reached via a
   series of reductions), the initial context can be used to determine
   the exact set of tokens that would be syntactically acceptable in the
   lookahead's place.  Moreover, it is the context after which any
   further semantic actions would be erroneous because they would be
   determined by a syntactically unacceptable token.

   YY_LAC_ESTABLISH should be invoked when a reduction is about to be
   performed in an inconsistent state (which, for the purposes of LAC,
   includes consistent states that don't know they're consistent because
   their default reductions have been disabled).  Iff there is a
   lookahead token, it should also be invoked before reporting a syntax
   error.  This latter case is for the sake of the debugging output.

   For parse.lac=full, the implementation of YY_LAC_ESTABLISH is as
   follows.  If no initial context is currently established for the
   current lookahead, then check if that lookahead can eventually be
   shifted if syntactic actions continue from the current context.
   Report a syntax error if it cannot.  */
#define YY_LAC_ESTABLISH                                         \
do {                                                             \
  if (!yy_lac_established)                                       \
    {                                                            \
      YYDPRINTF ((stderr,                                        \
                  "LAC: initial context established for %s\n",   \
                  yytname[yytoken]));                            \
      yy_lac_established = 1;                                    \
      {                                                          \
        int yy_lac_status =                                      \
          yy_lac (yyesa, &yyes, &yyes_capacity, yyssp, yytoken); \
        if (yy_lac_status == 2)                                  \
          goto yyexhaustedlab;                                   \
        if (yy_lac_status == 1)                                  \
          goto yyerrlab;                                         \
      }                                                          \
    }                                                            \
} while (0)

/* Discard any previous initial lookahead context because of Event,
   which may be a lookahead change or an invalidation of the currently
   established initial context for the current lookahead.

   The most common example of a lookahead change is a shift.  An example
   of both cases is syntax error recovery.  That is, a syntax error
   occurs when the lookahead is syntactically erroneous for the
   currently established initial context, so error recovery manipulates
   the parser stacks to try to find a new initial context in which the
   current lookahead is syntactically acceptable.  If it fails to find
   such a context, it discards the lookahead.  */
#if YYDEBUG
# define YY_LAC_DISCARD(Event)                                           \
do {                                                                     \
  if (yy_lac_established)                                                \
    {                                                                    \
      if (yydebug)                                                       \
        YYFPRINTF (stderr, "LAC: initial context discarded due to "      \
                   Event "\n");                                          \
      yy_lac_established = 0;                                            \
    }                                                                    \
} while (0)
#else
# define YY_LAC_DISCARD(Event) yy_lac_established = 0
#endif

/* Given the stack whose top is *YYSSP, return 0 iff YYTOKEN can
   eventually (after perhaps some reductions) be shifted, return 1 if
   not, or return 2 if memory is exhausted.  As preconditions and
   postconditions: *YYES_CAPACITY is the allocated size of the array to
   which *YYES points, and either *YYES = YYESA or *YYES points to an
   array allocated with YYSTACK_ALLOC.  yy_lac may overwrite the
   contents of either array, alter *YYES and *YYES_CAPACITY, and free
   any old *YYES other than YYESA.  */
static int
yy_lac (yytype_int16 *yyesa, yytype_int16 **yyes,
        YYSIZE_T *yyes_capacity, yytype_int16 *yyssp, int yytoken)
{
  yytype_int16 *yyes_prev = yyssp;
  yytype_int16 *yyesp = yyes_prev;
  YYDPRINTF ((stderr, "LAC: checking lookahead %s:", yytname[yytoken]));
  if (yytoken == YYUNDEFTOK)
    {
      YYDPRINTF ((stderr, " Always Err\n"));
      return 1;
    }
  while (1)
    {
      int yyrule = yypact[*yyesp];
      if (yypact_value_is_default (yyrule)
          || (yyrule += yytoken) < 0 || YYLAST < yyrule
          || yycheck[yyrule] != yytoken)
        {
          yyrule = yydefact[*yyesp];
          if (yyrule == 0)
            {
              YYDPRINTF ((stderr, " Err\n"));
              return 1;
            }
        }
      else
        {
          yyrule = yytable[yyrule];
          if (yytable_value_is_error (yyrule))
            {
              YYDPRINTF ((stderr, " Err\n"));
              return 1;
            }
          if (0 < yyrule)
            {
              YYDPRINTF ((stderr, " S%d\n", yyrule));
              return 0;
            }
          yyrule = -yyrule;
        }
      {
        YYSIZE_T yylen = yyr2[yyrule];
        YYDPRINTF ((stderr, " R%d", yyrule - 1));
        if (yyesp != yyes_prev)
          {
            YYSIZE_T yysize = (YYSIZE_T) (yyesp - *yyes + 1);
            if (yylen < yysize)
              {
                yyesp -= yylen;
                yylen = 0;
              }
            else
              {
                yylen -= yysize;
                yyesp = yyes_prev;
              }
          }
        if (yylen)
          yyesp = yyes_prev -= yylen;
      }
      {
        yytype_int16 yystate;
        {
          const int yylhs = yyr1[yyrule] - YYNTOKENS;
          const int yyi = yypgoto[yylhs] + *yyesp;
          yystate = ((yytype_int16)
                     (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyesp
                      ? yytable[yyi]
                      : yydefgoto[yylhs]));
        }
        if (yyesp == yyes_prev)
          {
            yyesp = *yyes;
            *yyesp = yystate;
          }
        else
          {
            if (yy_lac_stack_realloc (yyes_capacity, 1,
#if YYDEBUG
                                      " (", ")",
#endif
                                      yyes, yyesa, &yyesp, yyes_prev))
              {
                YYDPRINTF ((stderr, "\n"));
                return 2;
              }
            *++yyesp = yystate;
          }
        YYDPRINTF ((stderr, " G%d", (int) yystate));
      }
    }
}


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
            else
              goto append;

          append:
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

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.  In order to see if a particular token T is a
   valid looakhead, invoke yy_lac (YYESA, YYES, YYES_CAPACITY, YYSSP, T).

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store or if
   yy_lac returned 2.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyesa, yytype_int16 **yyes,
                YYSIZE_T *yyes_capacity, yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
       In the first two cases, it might appear that the current syntax
       error should have been detected in the previous state when yy_lac
       was invoked.  However, at that time, there might have been a
       different syntax error that discarded a different initial context
       during error recovery, leaving behind the current lookahead.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      YYDPRINTF ((stderr, "Constructing syntax error message\n"));
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          int yyx;

          for (yyx = 0; yyx < YYNTOKENS; ++yyx)
            if (yyx != YYTERROR && yyx != YYUNDEFTOK)
              {
                {
                  int yy_lac_status = yy_lac (yyesa, yyes, yyes_capacity,
                                              yyssp, yyx);
                  if (yy_lac_status == 2)
                    return 2;
                  if (yy_lac_status == 1)
                    continue;
                }
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
# if YYDEBUG
      else if (yydebug)
        YYFPRINTF (stderr, "No expected tokens.\n");
# endif
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
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

    yytype_int16 yyesa[20];
    yytype_int16 *yyes;
    YYSIZE_T yyes_capacity;

  int yy_lac_established = 0;
  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  yyes = yyesa;
  yyes_capacity = sizeof yyesa / sizeof *yyes;
  if (YYMAXDEPTH < yyes_capacity)
    yyes_capacity = YYMAXDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
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
# else /* defined YYSTACK_RELOCATE */
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
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
    {
      YY_LAC_ESTABLISH;
      goto yydefault;
    }
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      YY_LAC_ESTABLISH;
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
  YY_LAC_DISCARD ("shift");

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  {
    int yychar_backup = yychar;
    switch (yyn)
      {
  case 2:
#line 39 "mml.y" /* yacc.c:1652  */
    {}
#line 1621 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 3:
#line 40 "mml.y" /* yacc.c:1652  */
    {}
#line 1627 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 4:
#line 44 "mml.y" /* yacc.c:1652  */
    {}
#line 1633 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 5:
#line 45 "mml.y" /* yacc.c:1652  */
    {}
#line 1639 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 6:
#line 46 "mml.y" /* yacc.c:1652  */
    {}
#line 1645 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 7:
#line 50 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_directive(&compiler, (yyvsp[-4].sval), (yyvsp[-2].sval)); }
#line 1651 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 8:
#line 51 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_directive(&compiler, (yyvsp[-2].sval), 0); }
#line 1657 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 9:
#line 61 "mml.y" /* yacc.c:1652  */
    {
		uint8_t data[47] = {
			 (yyvsp[-93].ival),  (yyvsp[-91].ival),  (yyvsp[-89].ival), (yyvsp[-87].ival), (yyvsp[-85].ival), (yyvsp[-83].ival), (yyvsp[-81].ival), (yyvsp[-79].ival), (yyvsp[-77].ival), (yyvsp[-75].ival), (yyvsp[-73].ival),
			(yyvsp[-71].ival), (yyvsp[-69].ival), (yyvsp[-67].ival), (yyvsp[-65].ival), (yyvsp[-63].ival), (yyvsp[-61].ival), (yyvsp[-59].ival), (yyvsp[-57].ival), (yyvsp[-55].ival), (yyvsp[-53].ival), (yyvsp[-51].ival),
			(yyvsp[-49].ival), (yyvsp[-47].ival), (yyvsp[-45].ival), (yyvsp[-43].ival), (yyvsp[-41].ival), (yyvsp[-39].ival), (yyvsp[-37].ival), (yyvsp[-35].ival), (yyvsp[-33].ival), (yyvsp[-31].ival), (yyvsp[-29].ival),
			(yyvsp[-27].ival), (yyvsp[-25].ival), (yyvsp[-23].ival), (yyvsp[-21].ival), (yyvsp[-19].ival), (yyvsp[-17].ival), (yyvsp[-15].ival), (yyvsp[-13].ival), (yyvsp[-11].ival), (yyvsp[-9].ival), (yyvsp[-7].ival),
			(yyvsp[-5].ival), (yyvsp[-3].ival), (yyvsp[-1].ival)
		};
		mdx_compiler_opmdef(&compiler, (yyvsp[-96].ival), data);
	}
#line 1672 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 10:
#line 74 "mml.y" /* yacc.c:1652  */
    {  }
#line 1678 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 13:
#line 81 "mml.y" /* yacc.c:1652  */
    { (yyval.ival) = (yyvsp[0].ival); }
#line 1684 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 14:
#line 82 "mml.y" /* yacc.c:1652  */
    { (yyval.ival) = (yyvsp[-1].ival) | MML_NOTE_SHARP; }
#line 1690 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 15:
#line 83 "mml.y" /* yacc.c:1652  */
    { (yyval.ival) = (yyvsp[-1].ival) | MML_NOTE_FLAT; }
#line 1696 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 16:
#line 87 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_note(&compiler, chan_mask, (yyvsp[-1].ival), (yyvsp[0].nval)); }
#line 1702 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 17:
#line 88 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_tempo(&compiler, chan_mask, (yyvsp[0].ival), 0); }
#line 1708 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 18:
#line 89 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_octave(&compiler, chan_mask, (yyvsp[0].ival)); }
#line 1714 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 19:
#line 90 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_octave_down(&compiler, chan_mask); }
#line 1720 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 20:
#line 91 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_octave_up(&compiler, chan_mask); }
#line 1726 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 21:
#line 92 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_note_num(&compiler, chan_mask, (yyvsp[-2].ival), (yyvsp[0].nval)); }
#line 1732 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 22:
#line 93 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_note_num(&compiler, chan_mask, (yyvsp[0].ival), 0); }
#line 1738 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 23:
#line 94 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_set_default_note_length(&compiler, chan_mask, (yyvsp[0].ival)); }
#line 1744 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 24:
#line 95 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_rest(&compiler, chan_mask, (yyvsp[0].nval)); }
#line 1750 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 25:
#line 96 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_staccato(&compiler, chan_mask, (yyvsp[0].ival), 0); }
#line 1756 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 26:
#line 97 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_staccato(&compiler, chan_mask, (yyvsp[0].ival), 1); }
#line 1762 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 27:
#line 98 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_portamento(&compiler, chan_mask); }
#line 1768 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 28:
#line 99 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_legato(&compiler, chan_mask); }
#line 1774 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 29:
#line 100 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_volume(&compiler, chan_mask, (yyvsp[0].ival), 0); }
#line 1780 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 30:
#line 101 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_tempo(&compiler, chan_mask, (yyvsp[0].ival), 1); }
#line 1786 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 31:
#line 102 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_volume(&compiler, chan_mask, (yyvsp[0].ival), 1); }
#line 1792 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 32:
#line 103 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_set_voice(&compiler, chan_mask, (yyvsp[0].ival)); }
#line 1798 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 33:
#line 104 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_end(&compiler, chan_mask); }
#line 1804 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 34:
#line 105 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_volume_down(&compiler, chan_mask); }
#line 1810 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 35:
#line 106 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_volume_up(&compiler, chan_mask); }
#line 1816 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 36:
#line 107 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_pan(&compiler, chan_mask, (yyvsp[0].ival)); }
#line 1822 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 37:
#line 108 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_key_on_delay(&compiler, chan_mask, (yyvsp[0].ival)); }
#line 1828 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 38:
#line 109 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_loop_start(&compiler, chan_mask); }
#line 1834 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 39:
#line 110 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_detune(&compiler, chan_mask, (yyvsp[0].ival)); }
#line 1840 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 40:
#line 111 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_opm_noise_freq(&compiler, chan_mask, (yyvsp[0].ival)); }
#line 1846 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 41:
#line 112 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_opm_write(&compiler, chan_mask, (yyvsp[-2].ival), (yyvsp[0].ival)); }
#line 1852 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 42:
#line 113 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_sync_send(&compiler, chan_mask, (yyvsp[0].ival)); }
#line 1858 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 43:
#line 114 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_sync_wait(&compiler, chan_mask); }
#line 1864 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 44:
#line 115 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_adpcm_freq(&compiler, chan_mask, (yyvsp[0].ival)); }
#line 1870 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 45:
#line 116 "mml.y" /* yacc.c:1652  */
    {
	  	mdx_compiler_opm_lfo(&compiler, chan_mask, (yyvsp[-12].ival), (yyvsp[-10].ival), (yyvsp[-8].ival), (yyvsp[-6].ival), (yyvsp[-4].ival), (yyvsp[-2].ival), (yyvsp[0].ival));
	  }
#line 1878 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 46:
#line 119 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_mhon(&compiler, chan_mask); }
#line 1884 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 47:
#line 120 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_mhof(&compiler, chan_mask); }
#line 1890 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 48:
#line 121 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_ma(&compiler, chan_mask, (yyvsp[-4].ival), (yyvsp[-2].ival), (yyvsp[0].ival)); }
#line 1896 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 49:
#line 122 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_maon(&compiler, chan_mask); }
#line 1902 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 50:
#line 123 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_maof(&compiler, chan_mask); }
#line 1908 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 51:
#line 124 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_mp(&compiler, chan_mask, (yyvsp[-4].ival), (yyvsp[-2].ival), (yyvsp[0].ival)); }
#line 1914 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 52:
#line 125 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_mpon(&compiler, chan_mask); }
#line 1920 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 53:
#line 126 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_mpof(&compiler, chan_mask); }
#line 1926 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 54:
#line 127 "mml.y" /* yacc.c:1652  */
    { mdx_compiler_md(&compiler, chan_mask); }
#line 1932 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 55:
#line 133 "mml.y" /* yacc.c:1652  */
    { (yyval.nval) = (yyvsp[0].nval); }
#line 1938 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 56:
#line 134 "mml.y" /* yacc.c:1652  */
    { (yyval.nval) = malloc(sizeof(struct mml_notelength)); (yyval.nval)->type = NoteLenTicks; (yyval.nval)->val = (yyvsp[0].ival); }
#line 1944 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 57:
#line 135 "mml.y" /* yacc.c:1652  */
    { (yyval.nval) = malloc(sizeof(struct mml_notelength)); (yyval.nval)->type = NoteLenJoin; (yyval.nval)->n1 = (yyvsp[-2].nval); (yyval.nval)->n2 = (yyvsp[0].nval); }
#line 1950 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 58:
#line 139 "mml.y" /* yacc.c:1652  */
    { (yyval.nval) = 0; }
#line 1956 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 59:
#line 140 "mml.y" /* yacc.c:1652  */
    { (yyval.nval) = malloc(sizeof(struct mml_notelength)); (yyval.nval)->type = NoteLenInt; (yyval.nval)->val = (yyvsp[0].ival); }
#line 1962 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 60:
#line 141 "mml.y" /* yacc.c:1652  */
    { (yyval.nval) = malloc(sizeof(struct mml_notelength)); (yyval.nval)->type = NoteLenDot; (yyval.nval)->n1 = (yyvsp[-1].nval); }
#line 1968 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 61:
#line 145 "mml.y" /* yacc.c:1652  */
    { (yyval.ival) = (yyvsp[0].ival); }
#line 1974 "mml.tab.c" /* yacc.c:1652  */
    break;

  case 62:
#line 146 "mml.y" /* yacc.c:1652  */
    { (yyval.ival) = 128 + (yyvsp[0].ival); }
#line 1980 "mml.tab.c" /* yacc.c:1652  */
    break;


#line 1984 "mml.tab.c" /* yacc.c:1652  */
        default: break;
      }
    if (yychar_backup != yychar)
      YY_LAC_DISCARD ("yychar change");
  }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyesa, &yyes, &yyes_capacity, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        if (yychar != YYEMPTY)
          YY_LAC_ESTABLISH;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

  /* If the stack popping above didn't lose the initial context for the
     current lookahead token, the shift below will for sure.  */
  YY_LAC_DISCARD ("error recovery");

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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


#if 1
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  if (yyes != yyesa)
    YYSTACK_FREE (yyes);
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 155 "mml.y" /* yacc.c:1918  */


int main(int argc, char **argv) {
	mdx_compiler_init(&compiler);
	yyparse();
	mdx_compiler_dump(&compiler);
	mdx_compiler_save(&compiler, "mml2mdx.mdx");

	return 0;
}
