/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         ppparse
#define yylex           pplex
#define yyerror         pperror
#define yydebug         ppdebug
#define yynerrs         ppnerrs
#define yylval          pplval
#define yychar          ppchar

/* First part of user prologue.  */
#line 34 "ppparse.y"

#include "config.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#ifdef	HAVE_STRINGS_H
#include <strings.h>
#endif
#include <ctype.h>

#define	COB_IN_PPPARSE	1
#include "cobc.h"
#include "tree.h"

#ifndef	_STDLIB_H
#define	_STDLIB_H 1
#endif

#define pperror(x)	cb_error_always ("%s", x)

#define COND_EQ		0
#define COND_LT		1U
#define COND_GT		2U
#define COND_LE		3U
#define COND_GE		4U
#define COND_NE		5U

/* Global variables */

int				current_call_convention;

/* Local variables */

static struct cb_define_struct	*ppp_setvar_list = NULL;
static enum cb_directive_action		current_cmd = PLEX_ACT_IF;

/* Local functions */

/* Strips the given string from its quotation characters, if any.  Returns its
   argument as is otherwise. */
static char *
unquote (char *name)
{
	size_t size;
	if ((name[0] == '\'' || name[0] == '"') && (size = strlen (name)) > 1 &&
	    (name[0] == name[size - 1])) {
		name[size - 1] = '\0';
		name++;
	}
	return name;
}
#define fix_filename(filename) unquote (filename)

static int
literal_is_space_keyword (char *lit)
{
	return (strcmp ("SPACE",  lit) == 0
		 || strcmp ("SPACES", lit) == 0);
}

static char *
literal_token (char *t, int allow_spaces)
{
	if (t[0] == '\'' || t[0] == '"') {
		if (cb_partial_replace_when_literal_src != CB_SKIP)
			(void) ppparse_verify (cb_partial_replace_when_literal_src,
					       _("partial replacing with literal"));
	} else if (allow_spaces && literal_is_space_keyword (t)) {
		if (cb_partial_replace_when_literal_src != CB_SKIP)
			(void) ppparse_verify (cb_partial_replace_when_literal_src,
					       _("partial replacing with literal"));
		t[0] = '\0';
	} else {
		ppparse_error (_("unexpected COBOL word in partial replacement "
				 "phrase"));
	}
	return unquote (t);
}

static char *
fold_lower (char *name)
{
	unsigned char	*p;

	for (p = (unsigned char *)name; *p; p++) {
		*p = (cob_u8_t)tolower (*p);
	}
	return name;
}

static char *
fold_upper (char *name)
{
	unsigned char	*p;

	for (p = (unsigned char *)name; *p; p++) {
		*p = (cob_u8_t)toupper (*p);
	}
	return name;
}

static struct cb_replace_src *
ppp_replace_src (const struct cb_text_list * const text_list,
		 const unsigned int literal_src)
{
	const unsigned int allow_empty_replacement =
		!literal_src || cb_partial_replace_when_literal_src != CB_SKIP;
	struct cb_replace_src *s = cobc_plex_malloc (sizeof (struct cb_replace_src));
	/* Note the two next fields are re-assessed in ppp_replace_list_add below */
	s->lead_trail = CB_REPLACE_ALL;
	s->strict = allow_empty_replacement ? 0 : 1;
	s->text_list = text_list;
	return s;
}

static struct cb_replace_list *
ppp_replace_list_add (struct cb_replace_list *list,
		      struct cb_replace_src *src,
		      const struct cb_text_list *new_text,
		      const unsigned int lead_or_trail)
{
	struct cb_replace_list *p;

	p = cobc_plex_malloc (sizeof (struct cb_replace_list));
	p->line_num = cb_source_line;
	src->lead_trail = lead_or_trail;
	if (!lead_or_trail) {
		/* Strictness flag is irrelevant for non-LEADING nor TRAILING
		   replacements */
		src->strict = 0;
	} else {
		/* Use replacement text to decide strictness of partial match */
		const unsigned char *c;
		int has_space = new_text->next != NULL;
		for (c = (unsigned char *) new_text->text; !has_space && *c; c++) {
			has_space = isspace(*c);
		}
		if (has_space) {
			/* Note: as it appears, multi-word or spaces in
			   replacing is forbidden on GCOS. */
			ppparse_error (_("invalid partial replacing operand"));
			return NULL;
		}
		src->strict = src->strict && *new_text->text == '\0';
	}
	p->src = src;
	p->new_text = new_text;
	if (!list) {
		p->last = p;
		return p;
	}
	list->last->next = p;
	list->last = p;
	return list;
}

static unsigned int
ppp_set_value (struct cb_define_struct *p, const char *value)
{
	const char	*s;
	size_t		size;
	unsigned int	dotseen;

	p->value = NULL;
	p->sign = 0;
	p->int_part = 0;
	p->dec_part = 0;

	if (!value) {
		p->deftype = PLEX_DEF_NONE;
		return 0;
	}

	if (*value == '"' || *value == '\'') {
		s = value + 1;
		size = strlen (s) - 1;
		if (s[size] != *value) {
			p->deftype = PLEX_DEF_NONE;
			return 1;
		}
		p->deftype = PLEX_DEF_LIT;
		p->value = cobc_plex_strdup (s);
		p->value[size] = 0;
		return 0;
	}

	if (*value == '(') {
		/* actual MicroFocus Format for numeric values: (numlit) */
		s = value + 1;
		size = strlen (s) - 1;
		if (s[size] != ')') {
			p->deftype = PLEX_DEF_NONE;
			return 1;
		}
		p->deftype = PLEX_DEF_NUM;
		p->value = cobc_plex_strdup (s);
		p->value[size] = 0;
	} else {
		/* compatibility because this was supported since OpenCOBOL 2.0 */
		p->deftype = PLEX_DEF_NUM;
		p->value = cobc_plex_strdup (value);
	}

	p->sign = 0;
	s = p->value;
	if (*s == '+') {
		s++;
	} else if (*s == '-') {
		s++;
		p->sign = 1;
	}
	dotseen = 0;
	for ( ; *s; ++s) {
		if (*s == '.') {
			if (dotseen) {
				p->deftype = PLEX_DEF_NONE;
				return 1;
			}
			dotseen = 1;
			continue;
		}
		if (*s > '9' || *s < '0') {
			p->deftype = PLEX_DEF_NONE;
			return 1;
		}
		if (!dotseen) {
			p->int_part = (p->int_part * 10) + (*s - '0');
		} else {
			p->dec_part = (p->dec_part * 10) + (*s - '0');
		}
	}

	if (!p->int_part && !p->dec_part) {
		p->sign = 0;	/* zero is unsigned */
	}
	return 0;
}

static unsigned int
ppp_compare_vals (const struct cb_define_struct *p1,
		 const struct cb_define_struct *p2,
		 const unsigned int cond)
{
	int	result;

	if (!p1 || !p2) {
		return 0;
	}
	if (p1->deftype != PLEX_DEF_LIT && p1->deftype != PLEX_DEF_NUM) {
		return 0;
	}
	if (p2->deftype != PLEX_DEF_LIT && p2->deftype != PLEX_DEF_NUM) {
		return 0;
	}
	if (p1->deftype != p2->deftype) {
		cb_warning (COBC_WARN_FILLER, _("directive comparison on different types"));
		return 0;
	}
	if (p1->deftype == PLEX_DEF_LIT) {
		result = strcmp (p1->value, p2->value);
	} else {
		if (p1->sign && !p2->sign) {
			result = -1;
		} else if (!p1->sign && p2->sign) {
			result = 1;
		} else if (p1->int_part < p2->int_part) {
			if (p1->sign) {
				result = 1;
			} else {
				result = -1;
			}
		} else if (p1->int_part > p2->int_part) {
			if (p1->sign) {
				result = -1;
			} else {
				result = 1;
			}
		} else if (p1->dec_part < p2->dec_part) {
			if (p1->sign) {
				result = 1;
			} else {
				result = -1;
			}
		} else if (p1->dec_part > p2->dec_part) {
			if (p1->sign) {
				result = -1;
			} else {
				result = 1;
			}
		} else {
			result = 0;
		}
	}
	switch (cond) {
	case COND_EQ:
		return (result == 0);
	case COND_LT:
		return (result < 0);
	case COND_GT:
		return (result > 0);
	case COND_LE:
		return (result <= 0);
	case COND_GE:
		return (result >= 0);
	case COND_NE:
		return (result != 0);
	default:
		break;
	}
	return 0;
}

static struct cb_define_struct *
ppp_define_add (struct cb_define_struct *list, const char *name,
		const char *text, const unsigned int override)
{
	struct cb_define_struct	*p;
	struct cb_define_struct	*l;

	/* Check duplicate */
	for (l = list; l; l = l->next) {
		if (!strcasecmp (name, l->name)) {
			if (!override && l->deftype != PLEX_DEF_DEL) {
				cb_error (_("duplicate DEFINE directive '%s'"), name);
				return NULL;
			}
			if (l->value) {
				l->value = NULL;
			}
			if (ppp_set_value (l, text)) {
				cb_error (_("invalid constant %s in DEFINE directive"), text);
				return NULL;
			}
			return list;
		}
	}

	p = cobc_plex_malloc (sizeof (struct cb_define_struct));
	p->name = cobc_plex_strdup (name);
	if (ppp_set_value (p, text)) {
		cb_error (_ ("invalid constant %s in DEFINE directive"), text);
		return NULL;
	}

	if (!list) {
		p->last = p;
		return p;
	}
	list->last->next = p;
	list->last = p;
	return list;
}

static void
ppp_define_del (const char *name)
{
	struct cb_define_struct	*l;

	for (l = ppp_setvar_list; l; l = l->next) {
		if (!strcmp (name, l->name)) {
			l->deftype = PLEX_DEF_DEL;
			if (l->value) {
				l->value = NULL;
			}
			l->sign = 0;
			l->int_part = 0;
			l->dec_part = 0;
			break;
		}
	}
}

void
ppp_clear_lists (void)
{
	ppp_setvar_list = NULL;
}

struct cb_define_struct *
ppp_search_lists (const char *name)
{
	struct cb_define_struct	*p;

	for (p = ppp_setvar_list; p; p = p->next) {
		if (p->name == NULL) {
			continue;
		}
		if (!strcasecmp (name, p->name)) {
			if (p->deftype != PLEX_DEF_DEL) {
				return p;
			}
			break;
		}
	}
	return NULL;
}

static struct cb_text_list *
ppp_list_add (struct cb_text_list *list, const char *text)
{
	struct cb_text_list	*p;

	p = cobc_plex_malloc (sizeof (struct cb_text_list));
	p->text = cobc_plex_strdup (text);
	if (!list) {
		p->last = p;
		return p;
	}
	list->last->next = p;
	list->last = p;
	return list;
}

static struct cb_text_list *
ppp_list_append (struct cb_text_list *list_1, struct cb_text_list *list_2)
{
	struct cb_text_list	*list_1_end;

	if (!list_1) {
		return list_2;
	}

	for (list_1_end = list_1;
	     list_1_end->next;
	     list_1_end = list_1_end->next);
	list_1_end->next = list_2;
	list_2->last = list_1_end;

	return list_1;
}

static unsigned int
ppp_search_comp_vars (const char *name)
{
#undef	CB_PARSE_DEF
#define	CB_PARSE_DEF(x,z)	if (!cb_strcasecmp (name, x)) return (z);
#include "ppparse.def"
#undef	CB_PARSE_DEF
	cb_warning (COBC_WARN_FILLER, _("compiler flag '%s' unknown"), name);
	return 0;
}

static unsigned int
ppp_check_needs_quote (const char *envval)
{
	const char	*s;
	size_t		size;
	unsigned int	dot_seen;
	unsigned int	sign_seen;

	/* Non-quoted value - Check if possible numeric */
	dot_seen = 0;
	sign_seen = 0;
	size = 0;
	s = envval;
	if (*s == '+' || *s == '-') {
		sign_seen = 1;
		size++;
		s++;
	}
	for (; *s; ++s) {
		if (*s == '.') {
			if (dot_seen) {
				break;
			}
			dot_seen = 1;
			size++;
			continue;
		}
		if (*s > '9' || *s < '0') {
			break;
		}
		size++;
	}

	if (*s || size <= ((size_t)dot_seen + sign_seen)) {
		return 1;
	}
	return 0;
}

static void
ppp_error_invalid_option (const char *directive, const char *option)
{
	if (option) {
		cb_error (_("invalid %s directive option '%s'"), directive, option);
	} else {
		cb_error (_("invalid %s directive option"), directive);
	}
}

static void
append_to_turn_list (struct cb_text_list *ec_names, int enable, int with_location)
{
	struct cb_turn_list	*l;
	struct cb_turn_list	*turn_list_end;

	/* Add turn directive data to end of cb_turn_list */
	l = cobc_plex_malloc (sizeof (struct cb_turn_list));
	l->ec_names = ec_names;
	l->enable = enable;
	l->with_location = with_location;
	l->next = NULL;
	/* The line number is set properly in the scanner */
	l->line = -1;

	if (cb_turn_list) {
		for (turn_list_end = cb_turn_list;
		     turn_list_end->next;
		     turn_list_end = turn_list_end->next);
		turn_list_end->next = l;
	} else {
		cb_turn_list = l;
	}

	/*
	  Output #TURN so we can assign a line number to this data later in the
	  scanner.
	*/
	fprintf (ppout, "#TURN\n");
}

/* Global functions */

void
ppparse_clear_vars (const struct cb_define_struct *p)
{
	const struct cb_define_struct	*q;

	ppp_setvar_list = NULL;
	/* Set standard DEFINE's */
	if (cb_perform_osvs) {
		ppp_setvar_list = ppp_define_add (ppp_setvar_list,
						  "PERFORM-TYPE",
						  "'OSVS'", 0);
	} else {
		ppp_setvar_list = ppp_define_add (ppp_setvar_list,
						  "PERFORM-TYPE",
						  "'MF'", 0);
	}
	if (cb_ebcdic_sign) {
		ppp_setvar_list = ppp_define_add (ppp_setvar_list,
						  "SIGN",
						  "'EBCDIC'", 0);
	} else {
		ppp_setvar_list = ppp_define_add (ppp_setvar_list,
						  "SIGN",
						  "'ASCII'", 0);
	}
#ifdef	WORDS_BIGENDIAN
	ppp_setvar_list = ppp_define_add (ppp_setvar_list,
					  "ENDIAN",
					  "'BIG'", 0);
#else
	ppp_setvar_list = ppp_define_add (ppp_setvar_list,
					  "ENDIAN",
					  "'LITTLE'", 0);
#endif
#if	' ' == 0x20
	ppp_setvar_list = ppp_define_add (ppp_setvar_list,
					  "CHARSET",
					  "'ASCII'", 0);
#elif	' ' == 0x40
	ppp_setvar_list = ppp_define_add (ppp_setvar_list,
					  "CHARSET",
					  "'EBCDIC'", 0);
#else
	ppp_setvar_list = ppp_define_add (ppp_setvar_list,
					  "CHARSET",
					  "'UNKNOWN'", 0);
#endif
	/* Set DEFINE's from '-D' option(s) */
	for (q = p; q; q = q->next) {
		ppp_setvar_list = ppp_define_add (ppp_setvar_list,
						  q->name,
						  q->value, 0);
	}
	/* reset CALL CONVENTION */
	current_call_convention = CB_CONV_COBOL;
}


#line 663 "ppparse.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
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

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_PP_PPPARSE_H_INCLUDED
# define YY_PP_PPPARSE_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ppdebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    TOKEN_EOF = 0,                 /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ALSO = 258,                    /* ALSO  */
    BY = 259,                      /* BY  */
    COPY = 260,                    /* COPY  */
    EQEQ = 261,                    /* "=="  */
    IN = 262,                      /* IN  */
    LAST = 263,                    /* LAST  */
    LEADING = 264,                 /* LEADING  */
    OF = 265,                      /* OF  */
    OFF = 266,                     /* OFF  */
    PRINTING = 267,                /* PRINTING  */
    REPLACE = 268,                 /* REPLACE  */
    REPLACING = 269,               /* REPLACING  */
    SUPPRESS = 270,                /* SUPPRESS  */
    TRAILING = 271,                /* TRAILING  */
    DOT = 272,                     /* "."  */
    GARBAGE = 273,                 /* "word"  */
    LISTING_DIRECTIVE = 274,       /* LISTING_DIRECTIVE  */
    LISTING_STATEMENT = 275,       /* LISTING_STATEMENT  */
    TITLE_STATEMENT = 276,         /* TITLE_STATEMENT  */
    COBOL_WORDS_DIRECTIVE = 277,   /* COBOL_WORDS_DIRECTIVE  */
    EQUATE = 278,                  /* EQUATE  */
    UNDEFINE = 279,                /* UNDEFINE  */
    SUBSTITUTE = 280,              /* SUBSTITUTE  */
    RESERVE = 281,                 /* RESERVE  */
    CONTROL_STATEMENT = 282,       /* CONTROL_STATEMENT  */
    SOURCE = 283,                  /* SOURCE  */
    NOSOURCE = 284,                /* NOSOURCE  */
    LIST = 285,                    /* LIST  */
    NOLIST = 286,                  /* NOLIST  */
    MAP = 287,                     /* MAP  */
    NOMAP = 288,                   /* NOMAP  */
    LEAP_SECOND_DIRECTIVE = 289,   /* LEAP_SECOND_DIRECTIVE  */
    CONTROL_DIVISION = 290,        /* "CONTROL DIVISION"  */
    SUBSTITUTION_SECTION = 291,    /* "SUBSTITUTION SECTION"  */
    SOURCE_DIRECTIVE = 292,        /* SOURCE_DIRECTIVE  */
    FORMAT = 293,                  /* FORMAT  */
    IS = 294,                      /* IS  */
    CALL_DIRECTIVE = 295,          /* CALL_DIRECTIVE  */
    COBOL = 296,                   /* COBOL  */
    TOK_EXTERN = 297,              /* "EXTERN"  */
    STDCALL = 298,                 /* STDCALL  */
    STATIC = 299,                  /* STATIC  */
    DEFINE_DIRECTIVE = 300,        /* DEFINE_DIRECTIVE  */
    AS = 301,                      /* AS  */
    PARAMETER = 302,               /* PARAMETER  */
    OVERRIDE = 303,                /* OVERRIDE  */
    REFMOD_DIRECTIVE = 304,        /* REFMOD_DIRECTIVE  */
    SET_DIRECTIVE = 305,           /* SET_DIRECTIVE  */
    ADDRSV = 306,                  /* ADDRSV  */
    ADDSYN = 307,                  /* ADDSYN  */
    AREACHECK = 308,               /* AREACHECK  */
    NOAREACHECK = 309,             /* NOAREACHECK  */
    ASSIGN = 310,                  /* ASSIGN  */
    BOUND = 311,                   /* BOUND  */
    CALLFH = 312,                  /* CALLFH  */
    CHECKNUM = 313,                /* CHECKNUM  */
    COMP1 = 314,                   /* COMP1  */
    CONSTANT = 315,                /* CONSTANT  */
    DPC_IN_DATA = 316,             /* "DPC-IN-DATA"  */
    FOLDCOPYNAME = 317,            /* FOLDCOPYNAME  */
    MAKESYN = 318,                 /* MAKESYN  */
    NOBOUND = 319,                 /* NOBOUND  */
    NOCHECKNUM = 320,              /* NOCHECKNUM  */
    NODPC_IN_DATA = 321,           /* "NODPC-IN-DATA"  */
    NOFOLDCOPYNAME = 322,          /* NOFOLDCOPYNAME  */
    NOODOSLIDE = 323,              /* NOODOSLIDE  */
    NOSPZERO = 324,                /* NOSPZERO  */
    NOSSRANGE = 325,               /* NOSSRANGE  */
    NOTRACE = 326,                 /* NOTRACE  */
    ODOSLIDE = 327,                /* ODOSLIDE  */
    REMOVE = 328,                  /* REMOVE  */
    SOURCEFORMAT = 329,            /* SOURCEFORMAT  */
    SPZERO = 330,                  /* SPZERO  */
    SSRANGE = 331,                 /* SSRANGE  */
    TRACE = 332,                   /* TRACE  */
    IF_DIRECTIVE = 333,            /* IF_DIRECTIVE  */
    ELSE_DIRECTIVE = 334,          /* ELSE_DIRECTIVE  */
    ENDIF_DIRECTIVE = 335,         /* ENDIF_DIRECTIVE  */
    ELIF_DIRECTIVE = 336,          /* ELIF_DIRECTIVE  */
    GE = 337,                      /* ">="  */
    LE = 338,                      /* "<="  */
    LT = 339,                      /* "<"  */
    GT = 340,                      /* ">"  */
    EQ = 341,                      /* "="  */
    NE = 342,                      /* "<>"  */
    NOT = 343,                     /* NOT  */
    THAN = 344,                    /* THAN  */
    TO = 345,                      /* TO  */
    OR = 346,                      /* OR  */
    EQUAL = 347,                   /* EQUAL  */
    GREATER = 348,                 /* GREATER  */
    LESS = 349,                    /* LESS  */
    SET = 350,                     /* SET  */
    DEFINED = 351,                 /* DEFINED  */
    TURN_DIRECTIVE = 352,          /* TURN_DIRECTIVE  */
    ON = 353,                      /* ON  */
    CHECKING = 354,                /* CHECKING  */
    WITH = 355,                    /* WITH  */
    LOCATION = 356,                /* LOCATION  */
    IMP_DIRECTIVE = 357,           /* IMP_DIRECTIVE  */
    INCLUDE = 358,                 /* INCLUDE  */
    TERMINATOR = 359,              /* "end of line"  */
    TOKEN = 360,                   /* "Word or Literal"  */
    TEXT_NAME = 361,               /* "Text-Name"  */
    VARIABLE_NAME = 362,           /* "Variable"  */
    LITERAL = 363                  /* "Literal"  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 619 "ppparse.y"

	char			*s;
	struct cb_text_list	*l;
	struct cb_replace_src	*p;
	struct cb_replace_list	*r;
	struct cb_define_struct	*ds;
	unsigned int		ui;
	int			si;

#line 831 "ppparse.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE pplval;


int ppparse (void);


#endif /* !YY_PP_PPPARSE_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ALSO = 3,                       /* ALSO  */
  YYSYMBOL_BY = 4,                         /* BY  */
  YYSYMBOL_COPY = 5,                       /* COPY  */
  YYSYMBOL_EQEQ = 6,                       /* "=="  */
  YYSYMBOL_IN = 7,                         /* IN  */
  YYSYMBOL_LAST = 8,                       /* LAST  */
  YYSYMBOL_LEADING = 9,                    /* LEADING  */
  YYSYMBOL_OF = 10,                        /* OF  */
  YYSYMBOL_OFF = 11,                       /* OFF  */
  YYSYMBOL_PRINTING = 12,                  /* PRINTING  */
  YYSYMBOL_REPLACE = 13,                   /* REPLACE  */
  YYSYMBOL_REPLACING = 14,                 /* REPLACING  */
  YYSYMBOL_SUPPRESS = 15,                  /* SUPPRESS  */
  YYSYMBOL_TRAILING = 16,                  /* TRAILING  */
  YYSYMBOL_DOT = 17,                       /* "."  */
  YYSYMBOL_GARBAGE = 18,                   /* "word"  */
  YYSYMBOL_LISTING_DIRECTIVE = 19,         /* LISTING_DIRECTIVE  */
  YYSYMBOL_LISTING_STATEMENT = 20,         /* LISTING_STATEMENT  */
  YYSYMBOL_TITLE_STATEMENT = 21,           /* TITLE_STATEMENT  */
  YYSYMBOL_COBOL_WORDS_DIRECTIVE = 22,     /* COBOL_WORDS_DIRECTIVE  */
  YYSYMBOL_EQUATE = 23,                    /* EQUATE  */
  YYSYMBOL_UNDEFINE = 24,                  /* UNDEFINE  */
  YYSYMBOL_SUBSTITUTE = 25,                /* SUBSTITUTE  */
  YYSYMBOL_RESERVE = 26,                   /* RESERVE  */
  YYSYMBOL_CONTROL_STATEMENT = 27,         /* CONTROL_STATEMENT  */
  YYSYMBOL_SOURCE = 28,                    /* SOURCE  */
  YYSYMBOL_NOSOURCE = 29,                  /* NOSOURCE  */
  YYSYMBOL_LIST = 30,                      /* LIST  */
  YYSYMBOL_NOLIST = 31,                    /* NOLIST  */
  YYSYMBOL_MAP = 32,                       /* MAP  */
  YYSYMBOL_NOMAP = 33,                     /* NOMAP  */
  YYSYMBOL_LEAP_SECOND_DIRECTIVE = 34,     /* LEAP_SECOND_DIRECTIVE  */
  YYSYMBOL_CONTROL_DIVISION = 35,          /* "CONTROL DIVISION"  */
  YYSYMBOL_SUBSTITUTION_SECTION = 36,      /* "SUBSTITUTION SECTION"  */
  YYSYMBOL_SOURCE_DIRECTIVE = 37,          /* SOURCE_DIRECTIVE  */
  YYSYMBOL_FORMAT = 38,                    /* FORMAT  */
  YYSYMBOL_IS = 39,                        /* IS  */
  YYSYMBOL_CALL_DIRECTIVE = 40,            /* CALL_DIRECTIVE  */
  YYSYMBOL_COBOL = 41,                     /* COBOL  */
  YYSYMBOL_TOK_EXTERN = 42,                /* "EXTERN"  */
  YYSYMBOL_STDCALL = 43,                   /* STDCALL  */
  YYSYMBOL_STATIC = 44,                    /* STATIC  */
  YYSYMBOL_DEFINE_DIRECTIVE = 45,          /* DEFINE_DIRECTIVE  */
  YYSYMBOL_AS = 46,                        /* AS  */
  YYSYMBOL_PARAMETER = 47,                 /* PARAMETER  */
  YYSYMBOL_OVERRIDE = 48,                  /* OVERRIDE  */
  YYSYMBOL_REFMOD_DIRECTIVE = 49,          /* REFMOD_DIRECTIVE  */
  YYSYMBOL_SET_DIRECTIVE = 50,             /* SET_DIRECTIVE  */
  YYSYMBOL_ADDRSV = 51,                    /* ADDRSV  */
  YYSYMBOL_ADDSYN = 52,                    /* ADDSYN  */
  YYSYMBOL_AREACHECK = 53,                 /* AREACHECK  */
  YYSYMBOL_NOAREACHECK = 54,               /* NOAREACHECK  */
  YYSYMBOL_ASSIGN = 55,                    /* ASSIGN  */
  YYSYMBOL_BOUND = 56,                     /* BOUND  */
  YYSYMBOL_CALLFH = 57,                    /* CALLFH  */
  YYSYMBOL_CHECKNUM = 58,                  /* CHECKNUM  */
  YYSYMBOL_COMP1 = 59,                     /* COMP1  */
  YYSYMBOL_CONSTANT = 60,                  /* CONSTANT  */
  YYSYMBOL_DPC_IN_DATA = 61,               /* "DPC-IN-DATA"  */
  YYSYMBOL_FOLDCOPYNAME = 62,              /* FOLDCOPYNAME  */
  YYSYMBOL_MAKESYN = 63,                   /* MAKESYN  */
  YYSYMBOL_NOBOUND = 64,                   /* NOBOUND  */
  YYSYMBOL_NOCHECKNUM = 65,                /* NOCHECKNUM  */
  YYSYMBOL_NODPC_IN_DATA = 66,             /* "NODPC-IN-DATA"  */
  YYSYMBOL_NOFOLDCOPYNAME = 67,            /* NOFOLDCOPYNAME  */
  YYSYMBOL_NOODOSLIDE = 68,                /* NOODOSLIDE  */
  YYSYMBOL_NOSPZERO = 69,                  /* NOSPZERO  */
  YYSYMBOL_NOSSRANGE = 70,                 /* NOSSRANGE  */
  YYSYMBOL_NOTRACE = 71,                   /* NOTRACE  */
  YYSYMBOL_ODOSLIDE = 72,                  /* ODOSLIDE  */
  YYSYMBOL_REMOVE = 73,                    /* REMOVE  */
  YYSYMBOL_SOURCEFORMAT = 74,              /* SOURCEFORMAT  */
  YYSYMBOL_SPZERO = 75,                    /* SPZERO  */
  YYSYMBOL_SSRANGE = 76,                   /* SSRANGE  */
  YYSYMBOL_TRACE = 77,                     /* TRACE  */
  YYSYMBOL_IF_DIRECTIVE = 78,              /* IF_DIRECTIVE  */
  YYSYMBOL_ELSE_DIRECTIVE = 79,            /* ELSE_DIRECTIVE  */
  YYSYMBOL_ENDIF_DIRECTIVE = 80,           /* ENDIF_DIRECTIVE  */
  YYSYMBOL_ELIF_DIRECTIVE = 81,            /* ELIF_DIRECTIVE  */
  YYSYMBOL_GE = 82,                        /* ">="  */
  YYSYMBOL_LE = 83,                        /* "<="  */
  YYSYMBOL_LT = 84,                        /* "<"  */
  YYSYMBOL_GT = 85,                        /* ">"  */
  YYSYMBOL_EQ = 86,                        /* "="  */
  YYSYMBOL_NE = 87,                        /* "<>"  */
  YYSYMBOL_NOT = 88,                       /* NOT  */
  YYSYMBOL_THAN = 89,                      /* THAN  */
  YYSYMBOL_TO = 90,                        /* TO  */
  YYSYMBOL_OR = 91,                        /* OR  */
  YYSYMBOL_EQUAL = 92,                     /* EQUAL  */
  YYSYMBOL_GREATER = 93,                   /* GREATER  */
  YYSYMBOL_LESS = 94,                      /* LESS  */
  YYSYMBOL_SET = 95,                       /* SET  */
  YYSYMBOL_DEFINED = 96,                   /* DEFINED  */
  YYSYMBOL_TURN_DIRECTIVE = 97,            /* TURN_DIRECTIVE  */
  YYSYMBOL_ON = 98,                        /* ON  */
  YYSYMBOL_CHECKING = 99,                  /* CHECKING  */
  YYSYMBOL_WITH = 100,                     /* WITH  */
  YYSYMBOL_LOCATION = 101,                 /* LOCATION  */
  YYSYMBOL_IMP_DIRECTIVE = 102,            /* IMP_DIRECTIVE  */
  YYSYMBOL_INCLUDE = 103,                  /* INCLUDE  */
  YYSYMBOL_TERMINATOR = 104,               /* "end of line"  */
  YYSYMBOL_TOKEN = 105,                    /* "Word or Literal"  */
  YYSYMBOL_TEXT_NAME = 106,                /* "Text-Name"  */
  YYSYMBOL_VARIABLE_NAME = 107,            /* "Variable"  */
  YYSYMBOL_LITERAL = 108,                  /* "Literal"  */
  YYSYMBOL_109_ = 109,                     /* '('  */
  YYSYMBOL_110_ = 110,                     /* ')'  */
  YYSYMBOL_YYACCEPT = 111,                 /* $accept  */
  YYSYMBOL_program_structure = 112,        /* program_structure  */
  YYSYMBOL_program_with_control_division = 113, /* program_with_control_division  */
  YYSYMBOL_control_division_no_replace = 114, /* control_division_no_replace  */
  YYSYMBOL_control_division_with_replace = 115, /* control_division_with_replace  */
  YYSYMBOL_statement_list = 116,           /* statement_list  */
  YYSYMBOL_statement_no_replace_list = 117, /* statement_no_replace_list  */
  YYSYMBOL_statement = 118,                /* statement  */
  YYSYMBOL_statement_no_replace = 119,     /* statement_no_replace  */
  YYSYMBOL_directive = 120,                /* directive  */
  YYSYMBOL_121_1 = 121,                    /* $@1  */
  YYSYMBOL_122_2 = 122,                    /* $@2  */
  YYSYMBOL_123_3 = 123,                    /* $@3  */
  YYSYMBOL_if_directive_if = 124,          /* if_directive_if  */
  YYSYMBOL_if_directive_elif = 125,        /* if_directive_elif  */
  YYSYMBOL_set_directive = 126,            /* set_directive  */
  YYSYMBOL_set_choice = 127,               /* set_choice  */
  YYSYMBOL_alnum_list = 128,               /* alnum_list  */
  YYSYMBOL_alnum_equality_list = 129,      /* alnum_equality_list  */
  YYSYMBOL_alnum_equality = 130,           /* alnum_equality  */
  YYSYMBOL_alnum_with_list = 131,          /* alnum_with_list  */
  YYSYMBOL_alnum_with = 132,               /* alnum_with  */
  YYSYMBOL_alnum_by_list = 133,            /* alnum_by_list  */
  YYSYMBOL_alnum_by = 134,                 /* alnum_by  */
  YYSYMBOL_set_options = 135,              /* set_options  */
  YYSYMBOL_refmod_directive = 136,         /* refmod_directive  */
  YYSYMBOL_source_directive = 137,         /* source_directive  */
  YYSYMBOL__literal = 138,                 /* _literal  */
  YYSYMBOL_define_directive = 139,         /* define_directive  */
  YYSYMBOL_cobol_words_directive = 140,    /* cobol_words_directive  */
  YYSYMBOL_listing_directive = 141,        /* listing_directive  */
  YYSYMBOL_listing_statement = 142,        /* listing_statement  */
  YYSYMBOL_control_options = 143,          /* control_options  */
  YYSYMBOL_control_option = 144,           /* control_option  */
  YYSYMBOL__dot = 145,                     /* _dot  */
  YYSYMBOL_leap_second_directive = 146,    /* leap_second_directive  */
  YYSYMBOL_imp_directive = 147,            /* imp_directive  */
  YYSYMBOL_imp_include_sources = 148,      /* imp_include_sources  */
  YYSYMBOL_turn_directive = 149,           /* turn_directive  */
  YYSYMBOL_ec_list = 150,                  /* ec_list  */
  YYSYMBOL_on_or_off = 151,                /* on_or_off  */
  YYSYMBOL_on_with_loc = 152,              /* on_with_loc  */
  YYSYMBOL_with_loc = 153,                 /* with_loc  */
  YYSYMBOL_call_directive = 154,           /* call_directive  */
  YYSYMBOL_call_choice = 155,              /* call_choice  */
  YYSYMBOL_if_directive = 156,             /* if_directive  */
  YYSYMBOL_garbage = 157,                  /* garbage  */
  YYSYMBOL_variable_or_literal = 158,      /* variable_or_literal  */
  YYSYMBOL_object_id = 159,                /* object_id  */
  YYSYMBOL_condition_clause = 160,         /* condition_clause  */
  YYSYMBOL_copy_statement = 161,           /* copy_statement  */
  YYSYMBOL_copy_source = 162,              /* copy_source  */
  YYSYMBOL__copy_in = 163,                 /* _copy_in  */
  YYSYMBOL_in_or_of = 164,                 /* in_or_of  */
  YYSYMBOL__copy_suppress = 165,           /* _copy_suppress  */
  YYSYMBOL__copy_replacing = 166,          /* _copy_replacing  */
  YYSYMBOL_replace_statement_with_dot = 167, /* replace_statement_with_dot  */
  YYSYMBOL_replace_statement = 168,        /* replace_statement  */
  YYSYMBOL_replacing_list = 169,           /* replacing_list  */
  YYSYMBOL_text_src = 170,                 /* text_src  */
  YYSYMBOL_text_dst = 171,                 /* text_dst  */
  YYSYMBOL_text_partial_src = 172,         /* text_partial_src  */
  YYSYMBOL_text_partial_dst = 173,         /* text_partial_dst  */
  YYSYMBOL_token_list = 174,               /* token_list  */
  YYSYMBOL_identifier = 175,               /* identifier  */
  YYSYMBOL_subscripts = 176,               /* subscripts  */
  YYSYMBOL_lead_trail = 177,               /* lead_trail  */
  YYSYMBOL_unquoted_literal = 178,         /* unquoted_literal  */
  YYSYMBOL__override = 179,                /* _override  */
  YYSYMBOL__not = 180,                     /* _not  */
  YYSYMBOL__also = 181,                    /* _also  */
  YYSYMBOL__last = 182,                    /* _last  */
  YYSYMBOL__as = 183,                      /* _as  */
  YYSYMBOL__format = 184,                  /* _format  */
  YYSYMBOL__is = 185,                      /* _is  */
  YYSYMBOL__printing = 186,                /* _printing  */
  YYSYMBOL__on = 187,                      /* _on  */
  YYSYMBOL__than = 188,                    /* _than  */
  YYSYMBOL__to = 189                       /* _to  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

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
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   322

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  111
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  79
/* YYNRULES -- Number of rules.  */
#define YYNRULES  227
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  324

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   363


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     109,   110,     2,     2,     2,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   794,   794,   795,   803,   804,   805,   806,   810,   815,
     818,   819,   822,   823,   827,   828,   832,   833,   834,   835,
     839,   840,   841,   842,   843,   844,   845,   846,   847,   849,
     848,   854,   853,   858,   862,   867,   866,   879,   880,   888,
     889,   897,   898,   904,   920,   921,   928,   935,   941,   953,
     958,   962,   966,   971,   983,   997,  1009,  1013,  1019,  1024,
    1029,  1033,  1037,  1041,  1046,  1054,  1059,  1063,  1070,  1077,
    1088,  1093,  1098,  1136,  1143,  1147,  1154,  1155,  1162,  1170,
    1171,  1178,  1186,  1187,  1194,  1203,  1206,  1213,  1218,  1226,
    1235,  1243,  1244,  1248,  1257,  1290,  1294,  1310,  1317,  1325,
    1332,  1340,  1350,  1353,  1354,  1358,  1359,  1363,  1364,  1368,
    1369,  1370,  1371,  1372,  1373,  1376,  1377,  1380,  1382,  1386,
    1391,  1396,  1407,  1411,  1418,  1425,  1429,  1436,  1440,  1444,
    1451,  1452,  1456,  1457,  1461,  1462,  1466,  1471,  1476,  1481,
    1488,  1495,  1502,  1512,  1527,  1535,  1536,  1537,  1541,  1542,
    1546,  1559,  1573,  1577,  1581,  1585,  1589,  1593,  1597,  1601,
    1605,  1609,  1613,  1620,  1625,  1632,  1641,  1654,  1657,  1664,
    1665,  1668,  1670,  1675,  1678,  1685,  1686,  1693,  1697,  1704,
    1708,  1712,  1716,  1723,  1727,  1752,  1756,  1760,  1764,  1771,
    1781,  1785,  1793,  1797,  1801,  1808,  1812,  1819,  1823,  1830,
    1837,  1852,  1856,  1864,  1868,  1875,  1895,  1898,  1906,  1909,
    1917,  1920,  1928,  1931,  1937,  1937,  1938,  1938,  1939,  1939,
    1940,  1940,  1941,  1941,  1942,  1942,  1943,  1943
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ALSO", "BY", "COPY",
  "\"==\"", "IN", "LAST", "LEADING", "OF", "OFF", "PRINTING", "REPLACE",
  "REPLACING", "SUPPRESS", "TRAILING", "\".\"", "\"word\"",
  "LISTING_DIRECTIVE", "LISTING_STATEMENT", "TITLE_STATEMENT",
  "COBOL_WORDS_DIRECTIVE", "EQUATE", "UNDEFINE", "SUBSTITUTE", "RESERVE",
  "CONTROL_STATEMENT", "SOURCE", "NOSOURCE", "LIST", "NOLIST", "MAP",
  "NOMAP", "LEAP_SECOND_DIRECTIVE", "\"CONTROL DIVISION\"",
  "\"SUBSTITUTION SECTION\"", "SOURCE_DIRECTIVE", "FORMAT", "IS",
  "CALL_DIRECTIVE", "COBOL", "\"EXTERN\"", "STDCALL", "STATIC",
  "DEFINE_DIRECTIVE", "AS", "PARAMETER", "OVERRIDE", "REFMOD_DIRECTIVE",
  "SET_DIRECTIVE", "ADDRSV", "ADDSYN", "AREACHECK", "NOAREACHECK",
  "ASSIGN", "BOUND", "CALLFH", "CHECKNUM", "COMP1", "CONSTANT",
  "\"DPC-IN-DATA\"", "FOLDCOPYNAME", "MAKESYN", "NOBOUND", "NOCHECKNUM",
  "\"NODPC-IN-DATA\"", "NOFOLDCOPYNAME", "NOODOSLIDE", "NOSPZERO",
  "NOSSRANGE", "NOTRACE", "ODOSLIDE", "REMOVE", "SOURCEFORMAT", "SPZERO",
  "SSRANGE", "TRACE", "IF_DIRECTIVE", "ELSE_DIRECTIVE", "ENDIF_DIRECTIVE",
  "ELIF_DIRECTIVE", "\">=\"", "\"<=\"", "\"<\"", "\">\"", "\"=\"",
  "\"<>\"", "NOT", "THAN", "TO", "OR", "EQUAL", "GREATER", "LESS", "SET",
  "DEFINED", "TURN_DIRECTIVE", "ON", "CHECKING", "WITH", "LOCATION",
  "IMP_DIRECTIVE", "INCLUDE", "\"end of line\"", "\"Word or Literal\"",
  "\"Text-Name\"", "\"Variable\"", "\"Literal\"", "'('", "')'", "$accept",
  "program_structure", "program_with_control_division",
  "control_division_no_replace", "control_division_with_replace",
  "statement_list", "statement_no_replace_list", "statement",
  "statement_no_replace", "directive", "$@1", "$@2", "$@3",
  "if_directive_if", "if_directive_elif", "set_directive", "set_choice",
  "alnum_list", "alnum_equality_list", "alnum_equality", "alnum_with_list",
  "alnum_with", "alnum_by_list", "alnum_by", "set_options",
  "refmod_directive", "source_directive", "_literal", "define_directive",
  "cobol_words_directive", "listing_directive", "listing_statement",
  "control_options", "control_option", "_dot", "leap_second_directive",
  "imp_directive", "imp_include_sources", "turn_directive", "ec_list",
  "on_or_off", "on_with_loc", "with_loc", "call_directive", "call_choice",
  "if_directive", "garbage", "variable_or_literal", "object_id",
  "condition_clause", "copy_statement", "copy_source", "_copy_in",
  "in_or_of", "_copy_suppress", "_copy_replacing",
  "replace_statement_with_dot", "replace_statement", "replacing_list",
  "text_src", "text_dst", "text_partial_src", "text_partial_dst",
  "token_list", "identifier", "subscripts", "lead_trail",
  "unquoted_literal", "_override", "_not", "_also", "_last", "_as",
  "_format", "_is", "_printing", "_on", "_than", "_to", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-188)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-215)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      13,    33,    26,   126,    10,  -188,    19,    55,     3,  -188,
     -56,   118,   186,     8,    18,  -188,   -37,    38,   215,  -188,
    -188,  -188,  -188,   -42,   -25,  -188,  -188,   -22,  -188,  -188,
    -188,    43,    72,  -188,   163,    80,   126,    83,  -188,  -188,
      57,  -188,  -188,     2,    91,  -188,  -188,  -188,    92,    20,
      59,    62,    59,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
      44,  -188,  -188,  -188,  -188,  -188,  -188,   135,   113,    58,
     -24,  -188,  -188,  -188,  -188,  -188,  -188,  -188,    69,    59,
      69,  -188,  -188,    70,  -188,    70,  -188,    70,    73,    70,
     133,    69,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,    59,   133,  -188,    78,  -188,   -29,   215,  -188,     9,
      14,  -188,  -188,   -60,    76,  -188,  -188,   170,  -188,   175,
    -188,  -188,  -188,  -188,  -188,   174,   -15,    86,  -188,  -188,
    -188,     2,   188,    17,    24,  -188,  -188,    89,    94,    20,
    -188,  -188,    87,   192,    62,  -188,    87,  -188,    95,  -188,
      -3,  -188,  -188,  -188,  -188,   113,  -188,   133,  -188,    -5,
     112,    69,  -188,    87,  -188,  -188,  -188,  -188,  -188,    93,
    -188,    70,  -188,    87,    15,  -188,  -188,  -188,   101,  -188,
    -188,     4,     6,  -188,  -188,    11,  -188,  -188,  -188,  -188,
      -2,  -188,  -188,    97,  -188,  -188,   126,   163,   198,   197,
    -188,  -188,    25,   216,    24,    22,   116,   117,   119,   120,
    -188,   222,  -188,   121,  -188,  -188,   122,  -188,  -188,  -188,
    -188,  -188,   123,  -188,   179,   179,   124,  -188,  -188,  -188,
    -188,  -188,  -188,   145,   145,  -188,  -188,  -188,  -188,    51,
     149,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,     2,
     217,  -188,  -188,    22,   247,    27,  -188,  -188,  -188,    17,
    -188,  -188,  -188,   -51,   246,    28,  -188,  -188,   179,  -188,
    -188,  -188,  -188,  -188,   153,     1,  -188,  -188,     2,  -188,
    -188,    28,  -188,    29,  -188,  -188,  -188,    30,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,   164,   166,   166,
    -188,  -188,    54,    54,  -188,  -188,  -188,   250,  -188,  -188,
    -188,   162,   167,  -188,  -188,  -188,  -188,  -188,   165,   169,
     164,   164,  -188,  -188
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      10,     0,     0,     3,    10,     1,     0,   210,   102,   105,
       0,     0,     0,   117,   216,    35,     0,   222,     0,    29,
      33,    34,    31,     0,     0,    11,    14,     0,    18,    16,
      15,     0,     0,     2,     6,     0,     4,     0,   165,   166,
     167,   211,   213,     0,     0,   104,   103,    26,   115,     0,
       0,     0,     0,    22,   109,   110,   111,   112,   113,   114,
     115,   107,   119,   118,    27,   217,    20,   218,     0,     0,
     214,   149,    21,    97,    88,   223,    24,    87,     0,     0,
       0,    47,    57,     0,    49,    51,    52,     0,     0,     0,
     214,     0,    58,    59,    60,    61,    62,    63,    64,    65,
      66,     0,   214,    71,    91,    73,    85,    23,    41,     0,
       0,   125,    25,     0,   120,    28,    17,     0,   175,     8,
      10,    12,   164,   169,   170,   171,     0,     0,   203,   204,
     197,   177,     0,   184,     0,   178,   116,     0,     0,    98,
      79,    74,    99,     0,   100,    82,   101,   108,     0,   219,
       0,   136,   137,   138,   139,    36,   134,   214,   215,     0,
       0,    67,    76,    45,    46,   205,    48,    50,    53,     0,
      54,     0,    56,    68,     0,    92,    72,    44,     0,    42,
      38,   218,   218,    30,    37,     0,   145,    40,    32,    39,
       0,   126,   122,   121,   176,     9,     5,     7,   220,   173,
     168,   195,     0,     0,     0,     0,     0,     0,     0,     0,
     191,     0,   106,     0,    80,    75,     0,    83,    19,    89,
      90,   135,     0,    95,   206,   206,     0,    77,    43,    55,
      70,    69,    86,   208,   208,   147,   148,   146,   129,   128,
       0,   133,   124,   127,   131,   123,    13,   221,   172,     0,
       0,   183,   196,     0,     0,     0,   188,   189,   179,   187,
     198,   199,   201,     0,     0,     0,    81,    84,   206,   207,
      94,    93,    78,   209,     0,     0,   130,   132,   174,   163,
     181,     0,   185,     0,   202,   200,   190,     0,   194,   180,
      96,   157,   159,   160,   158,   161,   162,   226,   224,   224,
     141,   140,     0,     0,   182,   186,   192,     0,   227,   156,
     225,   153,   155,   151,   150,   142,   143,   193,     0,     0,
     226,   226,   152,   154
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -188,  -188,  -188,  -188,  -188,     0,  -188,  -188,   -33,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,   152,   -39,  -188,   -23,
    -188,   125,  -188,   150,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,   202,   233,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,    56,  -188,   141,   187,  -188,   -16,    -4,    23,
    -188,   176,  -188,  -188,  -188,  -188,  -188,   181,    52,  -129,
      50,   100,    31,    53,  -184,  -188,  -128,   -34,  -187,    71,
    -188,  -188,   -65,  -188,    -9,  -188,  -188,     7,  -162
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     2,    33,    34,    35,     3,   197,    25,    26,    27,
     109,   110,    68,   183,   188,   107,   108,   142,   161,   162,
     139,   140,   144,   145,   177,    76,    66,   176,    72,    53,
      47,    28,    60,    61,   137,    64,   115,   193,   112,   113,
     242,   243,   244,   155,   156,   184,   185,   186,   315,   302,
      29,    40,   125,   126,   199,   250,    30,    31,   131,   132,
     258,   211,   289,   202,   133,   263,   134,   166,   270,   274,
      43,    44,   159,    67,   150,   248,    77,   311,   309
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      73,   120,   203,   204,    36,  -148,   223,  -149,   127,   238,
     180,   128,   235,   146,    45,   187,   230,   158,   129,    62,
      37,   259,   158,    69,   206,   171,     5,   207,   255,   256,
     209,   251,   257,   282,   287,   305,   306,   174,   271,   190,
     163,   178,   224,   149,   117,   149,    32,   191,     1,    74,
       4,   167,    48,   168,   284,   170,    65,   164,    41,   285,
     118,   136,   173,    42,   123,   111,  -212,   124,   172,   259,
      70,    71,    54,    55,    56,    57,    58,    59,   114,  -214,
    -148,   290,   116,   291,   292,   293,   294,   295,   296,   119,
      38,    39,   222,   297,   298,   299,   239,   121,   240,   241,
     122,    46,   135,   225,   219,   220,    63,   130,  -148,   136,
    -149,  -148,  -148,  -149,  -149,  -144,   181,   182,   236,    71,
     196,   181,   182,   165,    38,    39,   208,   130,   138,   210,
     252,     6,   201,   288,   252,   307,    75,   229,   227,     7,
     231,    49,    50,    51,    52,     8,     9,    10,    11,   203,
     204,   240,   241,    12,   151,   152,   153,   154,   322,   323,
      13,   313,   314,    14,   246,   157,    15,   141,     6,   237,
     143,    16,   233,   234,   149,    17,    18,   160,   165,   158,
     169,   192,     8,     9,    10,    11,   175,   194,     7,   198,
      12,   201,   205,   212,   213,   215,   216,    13,   226,   218,
      14,   228,   245,    15,    19,    20,    21,    22,    16,   232,
     247,   249,    17,    18,    54,    55,    56,    57,    58,    59,
     253,   260,   261,    23,   262,   264,   265,   269,    24,   266,
     267,   268,   272,   273,   279,   291,   292,   293,   294,   295,
     296,    19,    20,    21,    22,   297,   298,   299,   300,   301,
     277,   281,   286,   318,   308,   310,   317,   320,   319,   179,
      23,   321,   147,    78,   214,    24,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   148,   217,   276,   221,   189,   303,   316,
     195,   278,   200,   280,   254,   275,   312,     0,   283,     0,
       0,     0,   304,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   106
};

static const yytype_int16 yycheck[] =
{
      16,    34,   131,   131,     4,     1,    11,     1,     6,    11,
       1,     9,     1,    52,    11,     1,     1,    46,    16,    11,
       1,   205,    46,    60,     7,    90,     0,    10,     6,     7,
       6,     6,    10,     6,     6,     6,     6,   102,   225,    99,
      79,   106,    47,    39,     1,    39,    36,   107,    35,    11,
      17,    85,   108,    87,   105,    89,    38,    80,     3,   110,
      17,    17,   101,     8,     7,   107,    11,    10,    91,   253,
     107,   108,    28,    29,    30,    31,    32,    33,   103,   108,
     104,   268,   104,    82,    83,    84,    85,    86,    87,    17,
     105,   106,   157,    92,    93,    94,    98,    17,   100,   101,
      17,    98,    11,   108,   107,   108,    98,   105,   104,    17,
     104,   107,   108,   107,   108,   104,   107,   108,   107,   108,
     120,   107,   108,   108,   105,   106,   109,   105,   108,   105,
     105,     5,   105,   105,   105,   105,    98,   171,   161,    13,
     174,    23,    24,    25,    26,    19,    20,    21,    22,   278,
     278,   100,   101,    27,    41,    42,    43,    44,   320,   321,
      34,   107,   108,    37,   197,   107,    40,   108,     5,   185,
     108,    45,   181,   182,    39,    49,    50,   108,   108,    46,
     107,   105,    19,    20,    21,    22,   108,    17,    13,    15,
      27,   105,     4,   104,   100,   108,     4,    34,    86,   104,
      37,   108,   105,    40,    78,    79,    80,    81,    45,   108,
      12,    14,    49,    50,    28,    29,    30,    31,    32,    33,
       4,   105,   105,    97,   105,   105,     4,    48,   102,   108,
     108,   108,   108,    88,    17,    82,    83,    84,    85,    86,
      87,    78,    79,    80,    81,    92,    93,    94,    95,    96,
     101,     4,     6,    91,    90,    89,     6,    92,    91,   107,
      97,    92,    60,    48,   139,   102,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    60,   144,   239,   155,   110,   275,   303,
     119,   249,   126,   253,   204,   234,   299,    -1,   255,    -1,
      -1,    -1,   281,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    35,   112,   116,    17,     0,     5,    13,    19,    20,
      21,    22,    27,    34,    37,    40,    45,    49,    50,    78,
      79,    80,    81,    97,   102,   118,   119,   120,   142,   161,
     167,   168,    36,   113,   114,   115,   116,     1,   105,   106,
     162,     3,     8,   181,   182,    11,    98,   141,   108,    23,
      24,    25,    26,   140,    28,    29,    30,    31,    32,    33,
     143,   144,    11,    98,   146,    38,   137,   184,   123,    60,
     107,   108,   139,   158,    11,    98,   136,   187,    48,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,   107,   126,   127,   121,
     122,   107,   149,   150,   103,   147,   104,     1,    17,    17,
     119,    17,    17,     7,    10,   163,   164,     6,     9,    16,
     105,   169,   170,   175,   177,    11,    17,   145,   108,   131,
     132,   108,   128,   108,   133,   134,   128,   144,   145,    39,
     185,    41,    42,    43,    44,   154,   155,   107,    46,   183,
     108,   129,   130,   128,   130,   108,   178,   178,   178,   107,
     178,   183,   130,   128,   183,   108,   138,   135,   183,   127,
       1,   107,   108,   124,   156,   157,   158,     1,   125,   156,
      99,   107,   105,   148,    17,   168,   116,   117,    15,   165,
     162,   105,   174,   170,   177,     4,     7,    10,   109,     6,
     105,   172,   104,   100,   132,   108,     4,   134,   104,   107,
     108,   155,   183,    11,    47,   108,    86,   130,   108,   178,
       1,   178,   108,   185,   185,     1,   107,   158,    11,    98,
     100,   101,   151,   152,   153,   105,   119,    12,   186,    14,
     166,     6,   105,     4,   172,     6,     7,    10,   171,   175,
     105,   105,   105,   176,   105,     4,   108,   108,   108,    48,
     179,   179,   108,    88,   180,   180,   153,   101,   169,    17,
     171,     4,     6,   174,   105,   110,     6,     6,   105,   173,
     179,    82,    83,    84,    85,    86,    87,    92,    93,    94,
      95,    96,   160,   160,   173,     6,     6,   105,    90,   189,
      89,   188,   188,   107,   108,   159,   159,     6,    91,    91,
      92,    92,   189,   189
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   111,   112,   112,   113,   113,   113,   113,   114,   115,
     116,   116,   117,   117,   118,   118,   119,   119,   119,   119,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   121,
     120,   122,   120,   120,   120,   123,   120,   124,   124,   125,
     125,   126,   126,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   128,   128,   129,   129,   130,   131,
     131,   132,   133,   133,   134,   135,   135,   136,   136,   137,
     137,   138,   138,   139,   139,   139,   139,   139,   140,   140,
     140,   140,   141,   141,   141,   142,   142,   143,   143,   144,
     144,   144,   144,   144,   144,   145,   145,   146,   146,   146,
     147,   147,   148,   148,   149,   150,   150,   151,   151,   151,
     152,   152,   153,   153,   154,   154,   155,   155,   155,   155,
     156,   156,   156,   156,   156,   157,   157,   157,   158,   158,
     159,   159,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   161,   161,   162,   162,   163,   163,   164,
     164,   165,   165,   166,   166,   167,   167,   168,   168,   169,
     169,   169,   169,   170,   170,   171,   171,   171,   171,   171,
     172,   172,   173,   173,   173,   174,   174,   175,   175,   175,
     175,   176,   176,   177,   177,   178,   179,   179,   180,   180,
     181,   181,   182,   182,   183,   183,   184,   184,   185,   185,
     186,   186,   187,   187,   188,   188,   189,   189
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     3,     1,     1,     3,     1,     3,     2,     3,
       0,     2,     0,     2,     1,     1,     1,     2,     1,     4,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     0,
       3,     0,     3,     1,     1,     0,     3,     1,     1,     1,
       1,     1,     2,     3,     2,     2,     2,     1,     2,     1,
       2,     1,     1,     2,     2,     3,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     3,
       3,     1,     2,     1,     1,     2,     1,     2,     3,     1,
       2,     3,     1,     2,     3,     0,     2,     1,     1,     3,
       3,     0,     1,     4,     4,     3,     5,     1,     2,     2,
       2,     2,     0,     1,     1,     1,     4,     1,     2,     1,
       1,     1,     1,     1,     1,     0,     1,     0,     1,     1,
       1,     2,     1,     2,     3,     1,     2,     1,     1,     1,
       2,     1,     2,     1,     1,     2,     1,     1,     1,     1,
       4,     4,     5,     5,     1,     1,     2,     2,     1,     1,
       1,     1,     5,     2,     5,     2,     2,     1,     1,     1,
       1,     1,     1,     6,     3,     1,     1,     0,     2,     1,
       1,     0,     2,     0,     2,     2,     3,     3,     3,     3,
       4,     4,     5,     3,     1,     2,     3,     1,     1,     1,
       3,     1,     2,     3,     1,     1,     2,     1,     3,     3,
       4,     1,     2,     1,     1,     1,     0,     1,     0,     1,
       0,     1,     0,     1,     0,     1,     0,     1,     0,     1,
       0,     1,     0,     1,     0,     1,     0,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
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
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
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
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
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
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
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
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
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
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
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
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

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
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= TOKEN_EOF)
    {
      yychar = TOKEN_EOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
  switch (yyn)
    {
  case 29: /* $@1: %empty  */
#line 849 "ppparse.y"
  {
	current_cmd = PLEX_ACT_IF;
  }
#line 2547 "ppparse.c"
    break;

  case 31: /* $@2: %empty  */
#line 854 "ppparse.y"
  {
	current_cmd = PLEX_ACT_ELIF;
  }
#line 2555 "ppparse.c"
    break;

  case 33: /* directive: ELSE_DIRECTIVE  */
#line 859 "ppparse.y"
  {
	plex_action_directive (PLEX_ACT_ELSE, 0);
  }
#line 2563 "ppparse.c"
    break;

  case 34: /* directive: ENDIF_DIRECTIVE  */
#line 863 "ppparse.y"
  {
	plex_action_directive (PLEX_ACT_END, 0);
  }
#line 2571 "ppparse.c"
    break;

  case 35: /* $@3: %empty  */
#line 867 "ppparse.y"
  {
	current_call_convention = 0;
  }
#line 2579 "ppparse.c"
    break;

  case 36: /* directive: CALL_DIRECTIVE $@3 call_directive  */
#line 871 "ppparse.y"
  {
	if (current_call_convention == CB_CONV_STATIC_LINK) {
		current_call_convention |= CB_CONV_COBOL;
	};
  }
#line 2589 "ppparse.c"
    break;

  case 38: /* if_directive_if: error  */
#line 881 "ppparse.y"
  {
	cb_error (_("invalid %s directive"), "IF");
	yyerrok;
  }
#line 2598 "ppparse.c"
    break;

  case 40: /* if_directive_elif: error  */
#line 890 "ppparse.y"
  {
	cb_error (_("invalid %s directive"), "ELIF");
	yyerrok;
  }
#line 2607 "ppparse.c"
    break;

  case 43: /* set_choice: CONSTANT "Variable" "Literal"  */
#line 905 "ppparse.y"
  {
	/* note: the old version was _as LITERAL but MF doesn't support this */
	struct cb_define_struct	*p;

	p = ppp_define_add (ppp_setvar_list, (yyvsp[-1].s), (yyvsp[0].s), 1);
	if (p) {
		ppp_setvar_list = p;
		p = p->last;
		if (p->deftype == PLEX_DEF_NUM) {
			fprintf (ppout, "#DEFLIT %s %s\n", (yyvsp[-1].s), p->value);
		} else {
			fprintf (ppout, "#DEFLIT %s \"%s\"\n", (yyvsp[-1].s), p->value);
		}
	}
  }
#line 2627 "ppparse.c"
    break;

  case 45: /* set_choice: ADDRSV alnum_list  */
#line 922 "ppparse.y"
  {
	struct cb_text_list	*l;
	for (l = (yyvsp[0].l); l; l = l->next) {
		fprintf (ppout, "#ADDRSV %s\n", l->text);
	}
  }
#line 2638 "ppparse.c"
    break;

  case 46: /* set_choice: ADDSYN alnum_equality  */
#line 929 "ppparse.y"
  {
	struct cb_text_list	*l;
	for (l = (yyvsp[0].l); l; l = l->next->next) {
		fprintf (ppout, "#ADDSYN %s %s\n", l->text, l->next->text);
	}
  }
#line 2649 "ppparse.c"
    break;

  case 47: /* set_choice: AREACHECK  */
#line 936 "ppparse.y"
  {
	if (cobc_has_areacheck_directive ("AREACHECK")) {
		fprintf (ppout, "#AREACHECK\n");
	}
  }
#line 2659 "ppparse.c"
    break;

  case 48: /* set_choice: ASSIGN unquoted_literal  */
#line 942 "ppparse.y"
  {
	char	*p = (yyvsp[0].s);

	if (!cb_strcasecmp (p, "EXTERNAL")) {
		fprintf (ppout, "#ASSIGN %d\n", (int)CB_ASSIGN_EXT_FILE_NAME_REQUIRED);
	} else if (!cb_strcasecmp (p, "DYNAMIC")) {
		fprintf (ppout, "#ASSIGN %d\n", (int)CB_ASSIGN_VARIABLE_DEFAULT);
	} else {
		ppp_error_invalid_option ("ASSIGN", p);
	}
  }
#line 2675 "ppparse.c"
    break;

  case 49: /* set_choice: BOUND  */
#line 954 "ppparse.y"
  {
	/* Enable EC-BOUND-SUBSCRIPT checking */
	append_to_turn_list (ppp_list_add (NULL, "EC-BOUND-SUBSCRIPT"), 1, 0);
  }
#line 2684 "ppparse.c"
    break;

  case 50: /* set_choice: CALLFH unquoted_literal  */
#line 959 "ppparse.y"
  {
	fprintf (ppout, "#CALLFH \"%s\"\n", (yyvsp[0].s));
  }
#line 2692 "ppparse.c"
    break;

  case 51: /* set_choice: CALLFH  */
#line 963 "ppparse.y"
  {
	fprintf (ppout, "#CALLFH \"EXTFH\"\n");
  }
#line 2700 "ppparse.c"
    break;

  case 52: /* set_choice: CHECKNUM  */
#line 967 "ppparse.y"
  {
	/* Enable EC-DATA-INCOMPATIBLE checking */
	append_to_turn_list (ppp_list_add (NULL, "EC-DATA-INCOMPATIBLE"), 1, 0);
  }
#line 2709 "ppparse.c"
    break;

  case 53: /* set_choice: COMP1 unquoted_literal  */
#line 972 "ppparse.y"
  {
	char	*p = (yyvsp[0].s);

	if (!cb_strcasecmp (p, "BINARY")) {
		cb_binary_comp_1 = 1;
	} else if (!cb_strcasecmp (p, "FLOAT")) {
		cb_binary_comp_1 = 0;
	} else {
		ppp_error_invalid_option ("COMP1", p);
	}
  }
#line 2725 "ppparse.c"
    break;

  case 54: /* set_choice: "DPC-IN-DATA" unquoted_literal  */
#line 984 "ppparse.y"
  {
	char	*p = (yyvsp[0].s);

	if (!cb_strcasecmp (p, "XML")) {
		cb_dpc_in_data = CB_DPC_IN_XML;
	} else if (!cb_strcasecmp (p, "JSON")) {
		cb_dpc_in_data = CB_DPC_IN_JSON;
	} else if (!cb_strcasecmp (p, "ALL")) {
		cb_dpc_in_data = CB_DPC_IN_ALL;
	} else {
		ppp_error_invalid_option ("DPC-IN-DATA", p);
	}
  }
#line 2743 "ppparse.c"
    break;

  case 55: /* set_choice: FOLDCOPYNAME _as unquoted_literal  */
#line 998 "ppparse.y"
  {
	char	*p = (yyvsp[0].s);

	if (!cb_strcasecmp (p, "UPPER")) {
		cb_fold_copy = COB_FOLD_UPPER;
	} else if (!cb_strcasecmp (p, "LOWER")) {
		cb_fold_copy = COB_FOLD_LOWER;
	} else {
		ppp_error_invalid_option ("FOLD-COPY-NAME", p);
	}
  }
#line 2759 "ppparse.c"
    break;

  case 56: /* set_choice: MAKESYN alnum_equality  */
#line 1010 "ppparse.y"
  {
	fprintf (ppout, "#MAKESYN %s %s\n", (yyvsp[0].l)->text, (yyvsp[0].l)->next->text);
  }
#line 2767 "ppparse.c"
    break;

  case 57: /* set_choice: NOAREACHECK  */
#line 1014 "ppparse.y"
  {
	if (cobc_has_areacheck_directive ("NOAREACHECK")) {
		fprintf (ppout, "#NOAREACHECK\n");
	}
  }
#line 2777 "ppparse.c"
    break;

  case 58: /* set_choice: NOBOUND  */
#line 1020 "ppparse.y"
  {
	/* Disable EC-BOUND-SUBSCRIPT checking */
	append_to_turn_list (ppp_list_add (NULL, "EC-BOUND-SUBSCRIPT"), 0, 0);
  }
#line 2786 "ppparse.c"
    break;

  case 59: /* set_choice: NOCHECKNUM  */
#line 1025 "ppparse.y"
  {
	/* Disable EC-DATA-INCOMPATIBLE checking */
	append_to_turn_list (ppp_list_add (NULL, "EC-DATA-INCOMPATIBLE"), 0, 0);
  }
#line 2795 "ppparse.c"
    break;

  case 60: /* set_choice: "NODPC-IN-DATA"  */
#line 1030 "ppparse.y"
  {
	cb_dpc_in_data = CB_DPC_IN_NONE;
  }
#line 2803 "ppparse.c"
    break;

  case 61: /* set_choice: NOFOLDCOPYNAME  */
#line 1034 "ppparse.y"
  {
	cb_fold_copy = 0;
  }
#line 2811 "ppparse.c"
    break;

  case 62: /* set_choice: NOODOSLIDE  */
#line 1038 "ppparse.y"
  {
	fprintf (ppout, "#ODOSLIDE 0\n");
  }
#line 2819 "ppparse.c"
    break;

  case 63: /* set_choice: NOSPZERO  */
#line 1042 "ppparse.y"
  {
	CB_PENDING ("SPZERO");
	/* TODO: cb_space_is_zero = 0; */
  }
#line 2828 "ppparse.c"
    break;

  case 64: /* set_choice: NOSSRANGE  */
#line 1047 "ppparse.y"
  {
	/* Disable EC-BOUND-SUBSCRIPT and -REF-MOD checking */
	struct cb_text_list	*txt = ppp_list_add (NULL, "EC-BOUND-SUBSCRIPT");
	txt = ppp_list_add (txt, "EC-BOUND-REF-MOD");

	append_to_turn_list (txt, 0, 0);
  }
#line 2840 "ppparse.c"
    break;

  case 65: /* set_choice: NOTRACE  */
#line 1055 "ppparse.y"
  {
	cb_flag_traceall = 0;
	cb_flag_trace = 0;
  }
#line 2849 "ppparse.c"
    break;

  case 66: /* set_choice: ODOSLIDE  */
#line 1060 "ppparse.y"
  {
	fprintf (ppout, "#ODOSLIDE 1\n");
  }
#line 2857 "ppparse.c"
    break;

  case 67: /* set_choice: OVERRIDE alnum_equality_list  */
#line 1064 "ppparse.y"
  {
	struct cb_text_list	*l;
	for (l = (yyvsp[0].l); l; l = l->next->next) {
		fprintf (ppout, "#OVERRIDE %s %s\n", l->text, l->next->text);
	}
  }
#line 2868 "ppparse.c"
    break;

  case 68: /* set_choice: REMOVE alnum_list  */
#line 1071 "ppparse.y"
  {
	struct cb_text_list	*l;
	for (l = (yyvsp[0].l); l; l = l->next) {
		fprintf (ppout, "#REMOVE %s\n", l->text);
	}
  }
#line 2879 "ppparse.c"
    break;

  case 69: /* set_choice: SOURCEFORMAT _as unquoted_literal  */
#line 1078 "ppparse.y"
  {
	char	*p = (yyvsp[0].s);

	if (cobc_deciph_source_format (p) != 0) {
		ppp_error_invalid_option ("SOURCEFORMAT", p);
	}
	if (cb_src_list_file) {
		cb_current_file->source_format = cobc_get_source_format ();
	}
  }
#line 2894 "ppparse.c"
    break;

  case 70: /* set_choice: SOURCEFORMAT _as error  */
#line 1089 "ppparse.y"
  {
	/* FIXME: we should consume until end of line here! */
	ppp_error_invalid_option ("SOURCEFORMAT", NULL);
  }
#line 2903 "ppparse.c"
    break;

  case 71: /* set_choice: SPZERO  */
#line 1094 "ppparse.y"
  {
	CB_PENDING ("SPZERO");
	/* TODO: cb_space_is_zero = 1; */
  }
#line 2912 "ppparse.c"
    break;

  case 72: /* set_choice: SSRANGE _literal  */
#line 1099 "ppparse.y"
  {
	char	*p = (yyvsp[0].s);
	char	ep = 0;

	/* Remove surrounding quotes/brackets */
	if (p) {
		size_t	size;
		++p;
		size = strlen (p) - 1;
		p[size] = '\0';
		if (size == 1 && *p >= '1' && *p <= '3') {
			ep = *p;
		}
	} else {
		ep = '2';
	}

	/* Enable EC-BOUND-SUBSCRIPT and -REF-MOD checking */
	if (ep) {
		struct cb_text_list	*txt;
		if (ep == '3') {
			/* SSRANGE"3": REF-MOD, with zero length allowed (at runtime) */
			fprintf (ppout, "#REFMOD_ZERO 1\n");
		} else if (ep == '2') {
			/* SSRANGE"2": REF-MOD, zero length not allowed */
			fprintf (ppout, "#REFMOD_ZERO 0\n");
		} else /* if (ep == '1') */ {
			/* SSRANGE"1": REF-MOD minimal - check only for zero/negative */
			fprintf (ppout, "#REFMOD_ZERO 2\n");
		}
		txt = ppp_list_add (NULL, "EC-BOUND-SUBSCRIPT");
		txt = ppp_list_add (txt, "EC-BOUND-REF-MOD");
		append_to_turn_list (txt, 1, 0);
	} else {
		ppp_error_invalid_option ("SSRANGE", p);
	}
  }
#line 2954 "ppparse.c"
    break;

  case 73: /* set_choice: TRACE  */
#line 1137 "ppparse.y"
  {
	cb_flag_trace = 1;	/* no traceall with Visual COBOL, leave untouched */
  }
#line 2962 "ppparse.c"
    break;

  case 74: /* alnum_list: "Literal"  */
#line 1144 "ppparse.y"
  {
	(yyval.l) = ppp_list_add (NULL, (yyvsp[0].s));
  }
#line 2970 "ppparse.c"
    break;

  case 75: /* alnum_list: alnum_list "Literal"  */
#line 1148 "ppparse.y"
  {
	(yyval.l) = ppp_list_add ((yyvsp[-1].l), (yyvsp[0].s));
  }
#line 2978 "ppparse.c"
    break;

  case 77: /* alnum_equality_list: alnum_equality_list alnum_equality  */
#line 1156 "ppparse.y"
  {
	(yyval.l) = ppp_list_append ((yyvsp[-1].l), (yyvsp[0].l));
  }
#line 2986 "ppparse.c"
    break;

  case 78: /* alnum_equality: "Literal" "=" "Literal"  */
#line 1163 "ppparse.y"
  {
	(yyval.l) = ppp_list_add (NULL, (yyvsp[-2].s));
	(yyval.l) = ppp_list_add ((yyval.l), (yyvsp[0].s));
  }
#line 2995 "ppparse.c"
    break;

  case 80: /* alnum_with_list: alnum_with_list alnum_with  */
#line 1172 "ppparse.y"
  {
	(yyval.l) = ppp_list_append ((yyvsp[-1].l), (yyvsp[0].l));
  }
#line 3003 "ppparse.c"
    break;

  case 81: /* alnum_with: "Literal" WITH "Literal"  */
#line 1179 "ppparse.y"
  {
	(yyval.l) = ppp_list_add (NULL, (yyvsp[-2].s));
	(yyval.l) = ppp_list_add ((yyval.l), (yyvsp[0].s));
  }
#line 3012 "ppparse.c"
    break;

  case 83: /* alnum_by_list: alnum_by_list alnum_by  */
#line 1188 "ppparse.y"
  {
	(yyval.l) = ppp_list_append ((yyvsp[-1].l), (yyvsp[0].l));
  }
#line 3020 "ppparse.c"
    break;

  case 84: /* alnum_by: "Literal" BY "Literal"  */
#line 1195 "ppparse.y"
  {
	(yyval.l) = ppp_list_add (NULL, (yyvsp[-2].s));
	(yyval.l) = ppp_list_add ((yyval.l), (yyvsp[0].s));
  }
#line 3029 "ppparse.c"
    break;

  case 85: /* set_options: %empty  */
#line 1203 "ppparse.y"
  {
	fprintf (ppout, "#OPTION %s\n", (yyvsp[0].s));
  }
#line 3037 "ppparse.c"
    break;

  case 86: /* set_options: _as "Literal"  */
#line 1207 "ppparse.y"
  {
	fprintf (ppout, "#OPTION %s %s\n", (yyvsp[-2].s), (yyvsp[0].s));
  }
#line 3045 "ppparse.c"
    break;

  case 87: /* refmod_directive: _on  */
#line 1214 "ppparse.y"
  {
	cb_ref_mod_zero_length = 1;
	fprintf (ppout, "#OPTION REFMOD_ZERO 1\n");
  }
#line 3054 "ppparse.c"
    break;

  case 88: /* refmod_directive: OFF  */
#line 1219 "ppparse.y"
  {
	cb_ref_mod_zero_length = 0;
	fprintf (ppout, "#OPTION REFMOD_ZERO 0\n");
  }
#line 3063 "ppparse.c"
    break;

  case 89: /* source_directive: _format _is "Variable"  */
#line 1227 "ppparse.y"
  {
	  if (cobc_deciph_source_format ((yyvsp[0].s)) != 0) {
		  ppp_error_invalid_option ("SOURCE", (yyvsp[0].s));
	  }
	  if (cb_src_list_file) {
		  cb_current_file->source_format = cobc_get_source_format ();
	  }
  }
#line 3076 "ppparse.c"
    break;

  case 90: /* source_directive: _format _is "Literal"  */
#line 1236 "ppparse.y"
  {
	ppp_error_invalid_option ("SOURCE", (yyvsp[0].s));
	YYERROR;
  }
#line 3085 "ppparse.c"
    break;

  case 91: /* _literal: %empty  */
#line 1243 "ppparse.y"
                { (yyval.s) = NULL; }
#line 3091 "ppparse.c"
    break;

  case 93: /* define_directive: "Variable" _as "Literal" _override  */
#line 1249 "ppparse.y"
  {
	struct cb_define_struct	*p;

	p = ppp_define_add (ppp_setvar_list, (yyvsp[-3].s), (yyvsp[-1].s), (yyvsp[0].ui));
	if (p) {
		ppp_setvar_list = p;
	}
  }
#line 3104 "ppparse.c"
    break;

  case 94: /* define_directive: "Variable" _as PARAMETER _override  */
#line 1258 "ppparse.y"
  {
	char			*s;
	char			*q;
	struct cb_define_struct	*p;

	s = getenv ((yyvsp[-3].s));
	q = NULL;
	if (s && *s && *s != ' ') {
		if (*s == '"' || *s == '\'') {
			const size_t	size = strlen (s) - 1U;
			/* Ignore if improperly quoted */
			if (s[0] == s[size]) {
				q = s;
			}
		} else {
			if (ppp_check_needs_quote (s)) {
				/* Alphanumeric literal */
				q = cobc_plex_malloc (strlen (s) + 4U);
				sprintf (q, "'%s'", s);
			} else {
				/* Numeric literal */
				q = s;
			}
		}
	}
	if (q) {
		p = ppp_define_add (ppp_setvar_list, (yyvsp[-3].s), q, (yyvsp[0].ui));
		if (p) {
			ppp_setvar_list = p;
		}
	}
  }
#line 3141 "ppparse.c"
    break;

  case 95: /* define_directive: "Variable" _as OFF  */
#line 1291 "ppparse.y"
  {
	ppp_define_del ((yyvsp[-2].s));
  }
#line 3149 "ppparse.c"
    break;

  case 96: /* define_directive: CONSTANT "Variable" _as "Literal" _override  */
#line 1295 "ppparse.y"
  {
  /* OpenCOBOL/GnuCOBOL 2.0 extension: MF $SET CONSTANT in 2002+ style as
     >> DEFINE CONSTANT var [AS] literal  archaic extension:
     use plain  >> DEFINE var [AS] literal  for conditional compilation and
     use        01 CONSTANT with/without FROM clause  for constant definitions */
	struct cb_define_struct	*p;

	if (cb_verify (cb_define_constant_directive, ">> DEFINE CONSTANT var")) {
		p = ppp_define_add (ppp_setvar_list, (yyvsp[-3].s), (yyvsp[-1].s), (yyvsp[0].ui));
		if (p) {
			ppp_setvar_list = p;
			fprintf (ppout, "#DEFLIT %s %s%s\n", (yyvsp[-3].s), (yyvsp[-1].s), (yyvsp[0].ui) ? " OVERRIDE" : "");
		}
	}
  }
#line 3169 "ppparse.c"
    break;

  case 97: /* define_directive: variable_or_literal  */
#line 1311 "ppparse.y"
  {
	cb_error (_("invalid %s directive"), "DEFINE/SET");
  }
#line 3177 "ppparse.c"
    break;

  case 98: /* cobol_words_directive: EQUATE alnum_with_list  */
#line 1318 "ppparse.y"
  {
	struct cb_text_list* l;
	/* GC-Extension: standard has only one literal combination here */
	for (l = (yyvsp[0].l); l; l = l->next->next) {
		fprintf (ppout, "#ADDSYN-STD %s %s\n", l->text, l->next->text);
	}
  }
#line 3189 "ppparse.c"
    break;

  case 99: /* cobol_words_directive: UNDEFINE alnum_list  */
#line 1326 "ppparse.y"
  {
	struct cb_text_list	*l;
	for (l = (yyvsp[0].l); l; l = l->next) {
		fprintf (ppout, "#REMOVE-STD %s\n", l->text);
	}
  }
#line 3200 "ppparse.c"
    break;

  case 100: /* cobol_words_directive: SUBSTITUTE alnum_by_list  */
#line 1333 "ppparse.y"
  {
	struct cb_text_list* l;
	/* GC-Extension: standard has only one literal combination here */
	for (l = (yyvsp[0].l); l; l = l->next->next) {
		fprintf (ppout, "#OVERRIDE-STD %s %s\n", l->text, l->next->text);
	}
  }
#line 3212 "ppparse.c"
    break;

  case 101: /* cobol_words_directive: RESERVE alnum_list  */
#line 1341 "ppparse.y"
  {
	struct cb_text_list	*l;
	for (l = (yyvsp[0].l); l; l = l->next) {
		fprintf (ppout, "#ADDRSV %s\n", l->text);
	}
  }
#line 3223 "ppparse.c"
    break;

  case 118: /* leap_second_directive: ON  */
#line 1383 "ppparse.y"
  {
	CB_PENDING (_("LEAP-SECOND ON directive"));
  }
#line 3231 "ppparse.c"
    break;

  case 120: /* imp_directive: INCLUDE  */
#line 1392 "ppparse.y"
  {
	cb_error (_("invalid %s directive"), "IMP INCLUDE");
	yyerrok;
  }
#line 3240 "ppparse.c"
    break;

  case 121: /* imp_directive: INCLUDE imp_include_sources  */
#line 1397 "ppparse.y"
  {
	struct cb_text_list *p = (yyvsp[0].l);
	while (p != NULL) {
		fprintf (ppout, "#INCLUDE %s\n", p->text);
		p = p->next;
	}
  }
#line 3252 "ppparse.c"
    break;

  case 122: /* imp_include_sources: "Word or Literal"  */
#line 1408 "ppparse.y"
  {
	(yyval.l) = ppp_list_add (NULL, fix_filename ((yyvsp[0].s)));
  }
#line 3260 "ppparse.c"
    break;

  case 123: /* imp_include_sources: imp_include_sources "Word or Literal"  */
#line 1412 "ppparse.y"
  {
	(yyval.l) = ppp_list_add ((yyvsp[-1].l), fix_filename ((yyvsp[0].s)));
  }
#line 3268 "ppparse.c"
    break;

  case 124: /* turn_directive: ec_list CHECKING on_or_off  */
#line 1419 "ppparse.y"
  {
	append_to_turn_list ((yyvsp[-2].l), !!(yyvsp[0].ui), (yyvsp[0].ui) == 2U);
  }
#line 3276 "ppparse.c"
    break;

  case 125: /* ec_list: "Variable"  */
#line 1426 "ppparse.y"
  {
	(yyval.l) = ppp_list_add (NULL, (yyvsp[0].s));
  }
#line 3284 "ppparse.c"
    break;

  case 126: /* ec_list: ec_list "Variable"  */
#line 1430 "ppparse.y"
  {
	(yyval.l) = ppp_list_add ((yyvsp[-1].l), (yyvsp[0].s));
  }
#line 3292 "ppparse.c"
    break;

  case 127: /* on_or_off: on_with_loc  */
#line 1437 "ppparse.y"
  {
	(yyval.ui) = 2U;
  }
#line 3300 "ppparse.c"
    break;

  case 128: /* on_or_off: ON  */
#line 1441 "ppparse.y"
  {
	(yyval.ui) = 1U;
  }
#line 3308 "ppparse.c"
    break;

  case 129: /* on_or_off: OFF  */
#line 1445 "ppparse.y"
  {
	(yyval.ui) = 0;
  }
#line 3316 "ppparse.c"
    break;

  case 136: /* call_choice: COBOL  */
#line 1467 "ppparse.y"
  {
	current_call_convention |= CB_CONV_COBOL;
	current_call_convention &= ~CB_CONV_STDCALL;
  }
#line 3325 "ppparse.c"
    break;

  case 137: /* call_choice: "EXTERN"  */
#line 1472 "ppparse.y"
  {
	current_call_convention &= ~CB_CONV_STDCALL;
	current_call_convention &= ~CB_CONV_COBOL;
  }
#line 3334 "ppparse.c"
    break;

  case 138: /* call_choice: STDCALL  */
#line 1477 "ppparse.y"
  {
	current_call_convention |= CB_CONV_STDCALL;
	current_call_convention &= ~CB_CONV_COBOL;
  }
#line 3343 "ppparse.c"
    break;

  case 139: /* call_choice: STATIC  */
#line 1482 "ppparse.y"
  {
	current_call_convention |= CB_CONV_STATIC_LINK;
  }
#line 3351 "ppparse.c"
    break;

  case 140: /* if_directive: "Variable" _is _not DEFINED  */
#line 1489 "ppparse.y"
  {
	unsigned int		found;

	found = (ppp_search_lists ((yyvsp[-3].s)) != NULL);
	plex_action_directive (current_cmd, found ^ (yyvsp[-1].ui));
  }
#line 3362 "ppparse.c"
    break;

  case 141: /* if_directive: "Variable" _is _not SET  */
#line 1496 "ppparse.y"
  {
	unsigned int		found;

	found = ppp_search_comp_vars ((yyvsp[-3].s));
	plex_action_directive (current_cmd, found ^ (yyvsp[-1].ui));
  }
#line 3373 "ppparse.c"
    break;

  case 142: /* if_directive: "Variable" _is _not condition_clause object_id  */
#line 1503 "ppparse.y"
  {
	struct cb_define_struct	*p;
	unsigned int		found;

	found = 0;
	p = ppp_search_lists ((yyvsp[-4].s));
	found = ppp_compare_vals (p, (yyvsp[0].ds), (yyvsp[-1].ui));
	plex_action_directive (current_cmd, found ^ (yyvsp[-2].ui));
  }
#line 3387 "ppparse.c"
    break;

  case 143: /* if_directive: "Literal" _is _not condition_clause object_id  */
#line 1513 "ppparse.y"
  {
	struct cb_define_struct	*p;
	unsigned int		found;

	found = 0;
	p = cobc_plex_malloc (sizeof (struct cb_define_struct));
	p->next = NULL;
	if (ppp_set_value (p, (yyvsp[-4].s))) {
		cb_error (_("invalid constant"));
	} else {
		found = ppp_compare_vals (p, (yyvsp[0].ds), (yyvsp[-1].ui));
	}
	plex_action_directive (current_cmd, found ^ (yyvsp[-2].ui));
  }
#line 3406 "ppparse.c"
    break;

  case 144: /* if_directive: garbage  */
#line 1528 "ppparse.y"
  {
	plex_action_directive (current_cmd, 0);
	YYERROR;
  }
#line 3415 "ppparse.c"
    break;

  case 150: /* object_id: "Literal"  */
#line 1547 "ppparse.y"
  {
	struct cb_define_struct	*p;

	p = cobc_plex_malloc (sizeof (struct cb_define_struct));
	p->next = NULL;
	if (ppp_set_value (p, (yyvsp[0].s))) {
		cb_error (_("invalid constant"));
		(yyval.ds) = NULL;
	} else {
		(yyval.ds) = p;
	}
  }
#line 3432 "ppparse.c"
    break;

  case 151: /* object_id: "Variable"  */
#line 1560 "ppparse.y"
  {
	struct cb_define_struct	*p;

	p = ppp_search_lists ((yyvsp[0].s));
	if (p != NULL && p->deftype != PLEX_DEF_NONE) {
		(yyval.ds) = p;
	} else {
		(yyval.ds) = NULL;
	}
  }
#line 3447 "ppparse.c"
    break;

  case 152: /* condition_clause: GREATER _than OR EQUAL _to  */
#line 1574 "ppparse.y"
  {
	(yyval.ui) = COND_GE;
  }
#line 3455 "ppparse.c"
    break;

  case 153: /* condition_clause: GREATER _than  */
#line 1578 "ppparse.y"
  {
	(yyval.ui) = COND_GT;
  }
#line 3463 "ppparse.c"
    break;

  case 154: /* condition_clause: LESS _than OR EQUAL _to  */
#line 1582 "ppparse.y"
  {
	(yyval.ui) = COND_LE;
  }
#line 3471 "ppparse.c"
    break;

  case 155: /* condition_clause: LESS _than  */
#line 1586 "ppparse.y"
  {
	(yyval.ui) = COND_LT;
  }
#line 3479 "ppparse.c"
    break;

  case 156: /* condition_clause: EQUAL _to  */
#line 1590 "ppparse.y"
  {
	(yyval.ui) = COND_EQ;
  }
#line 3487 "ppparse.c"
    break;

  case 157: /* condition_clause: ">="  */
#line 1594 "ppparse.y"
  {
	(yyval.ui) = COND_GE;
  }
#line 3495 "ppparse.c"
    break;

  case 158: /* condition_clause: ">"  */
#line 1598 "ppparse.y"
  {
	(yyval.ui) = COND_GT;
  }
#line 3503 "ppparse.c"
    break;

  case 159: /* condition_clause: "<="  */
#line 1602 "ppparse.y"
  {
	(yyval.ui) = COND_LE;
  }
#line 3511 "ppparse.c"
    break;

  case 160: /* condition_clause: "<"  */
#line 1606 "ppparse.y"
  {
	(yyval.ui) = COND_LT;
  }
#line 3519 "ppparse.c"
    break;

  case 161: /* condition_clause: "="  */
#line 1610 "ppparse.y"
  {
	(yyval.ui) = COND_EQ;
  }
#line 3527 "ppparse.c"
    break;

  case 162: /* condition_clause: "<>"  */
#line 1614 "ppparse.y"
  {
	(yyval.ui) = COND_NE;
  }
#line 3535 "ppparse.c"
    break;

  case 163: /* copy_statement: COPY copy_source _copy_in _copy_suppress _copy_replacing "."  */
#line 1621 "ppparse.y"
  {
	fputc ('\n', ppout);
	ppcopy ((yyvsp[-4].s), (yyvsp[-3].s), (yyvsp[-1].r));
  }
#line 3544 "ppparse.c"
    break;

  case 164: /* copy_statement: COPY error "."  */
#line 1626 "ppparse.y"
  {
	yyerrok;
  }
#line 3552 "ppparse.c"
    break;

  case 165: /* copy_source: "Word or Literal"  */
#line 1633 "ppparse.y"
  {
	(yyval.s) = fix_filename ((yyvsp[0].s));
	if (cb_fold_copy == COB_FOLD_LOWER) {
		(yyval.s) = fold_lower ((yyval.s));
	} else if (cb_fold_copy == COB_FOLD_UPPER) {
		(yyval.s) = fold_upper ((yyval.s));
	}
  }
#line 3565 "ppparse.c"
    break;

  case 166: /* copy_source: "Text-Name"  */
#line 1642 "ppparse.y"
  {
	(yyval.s) = (yyvsp[0].s);
	if (cb_fold_copy == COB_FOLD_LOWER) {
		(yyval.s) = fold_lower ((yyval.s));
	} else {
		(yyval.s) = fold_upper ((yyval.s));
	}
  }
#line 3578 "ppparse.c"
    break;

  case 167: /* _copy_in: %empty  */
#line 1654 "ppparse.y"
  {
	(yyval.s) = NULL;
  }
#line 3586 "ppparse.c"
    break;

  case 168: /* _copy_in: in_or_of copy_source  */
#line 1658 "ppparse.y"
  {
	(yyval.s) = (yyvsp[0].s);
  }
#line 3594 "ppparse.c"
    break;

  case 173: /* _copy_replacing: %empty  */
#line 1675 "ppparse.y"
  {
	(yyval.r) = NULL;
  }
#line 3602 "ppparse.c"
    break;

  case 174: /* _copy_replacing: REPLACING replacing_list  */
#line 1679 "ppparse.y"
  {
	(yyval.r) = (yyvsp[0].r);
  }
#line 3610 "ppparse.c"
    break;

  case 176: /* replace_statement_with_dot: replace_statement error "."  */
#line 1687 "ppparse.y"
  {
	yyerrok;
  }
#line 3618 "ppparse.c"
    break;

  case 177: /* replace_statement: REPLACE _also replacing_list  */
#line 1694 "ppparse.y"
  {
	cb_set_replace_list ((yyvsp[0].r), (yyvsp[-1].ui));
  }
#line 3626 "ppparse.c"
    break;

  case 178: /* replace_statement: REPLACE _last OFF  */
#line 1698 "ppparse.y"
  {
	cb_set_replace_list (NULL, (yyvsp[-1].ui));
  }
#line 3634 "ppparse.c"
    break;

  case 179: /* replacing_list: text_src BY text_dst  */
#line 1705 "ppparse.y"
  {
	(yyval.r) = ppp_replace_list_add (NULL, (yyvsp[-2].p), (yyvsp[0].l), 0);
  }
#line 3642 "ppparse.c"
    break;

  case 180: /* replacing_list: lead_trail text_partial_src BY text_partial_dst  */
#line 1709 "ppparse.y"
  {
	(yyval.r) = ppp_replace_list_add (NULL, (yyvsp[-2].p), (yyvsp[0].l), (yyvsp[-3].ui));
  }
#line 3650 "ppparse.c"
    break;

  case 181: /* replacing_list: replacing_list text_src BY text_dst  */
#line 1713 "ppparse.y"
  {
	(yyval.r) = ppp_replace_list_add ((yyvsp[-3].r), (yyvsp[-2].p), (yyvsp[0].l), 0);
  }
#line 3658 "ppparse.c"
    break;

  case 182: /* replacing_list: replacing_list lead_trail text_partial_src BY text_partial_dst  */
#line 1717 "ppparse.y"
  {
	(yyval.r) = ppp_replace_list_add ((yyvsp[-4].r), (yyvsp[-2].p), (yyvsp[0].l), (yyvsp[-3].ui));
  }
#line 3666 "ppparse.c"
    break;

  case 183: /* text_src: "==" token_list "=="  */
#line 1724 "ppparse.y"
  {
	(yyval.p) = ppp_replace_src ((yyvsp[-1].l), 0);
  }
#line 3674 "ppparse.c"
    break;

  case 184: /* text_src: identifier  */
#line 1728 "ppparse.y"
  {
	(yyval.p) = ppp_replace_src ((yyvsp[0].l), 0);
/* CHECKME later (parser conflict)
  }
| IN
  {
	/ * as we need this word, which is valid as replacement,
	   also for qualification, we need to explicit make it
	   a word if given alone * /
	$$ = ppp_list_add (NULL, "IN");
	$$ = ppp_replace_src ($$, 0);
  }
| OF
  {
	/ * as we need this word, which is valid as replacement,
	   also for qualification, we need to explicit make it
	   a word if given alone * /
	$$ = ppp_list_add (NULL, "OF");
	$$ = ppp_replace_src ($$, 0);
*/
  }
#line 3700 "ppparse.c"
    break;

  case 185: /* text_dst: "==" "=="  */
#line 1753 "ppparse.y"
  {
	(yyval.l) = NULL;
  }
#line 3708 "ppparse.c"
    break;

  case 186: /* text_dst: "==" token_list "=="  */
#line 1757 "ppparse.y"
  {
	(yyval.l) = (yyvsp[-1].l);
  }
#line 3716 "ppparse.c"
    break;

  case 187: /* text_dst: identifier  */
#line 1761 "ppparse.y"
  {
	(yyval.l) = (yyvsp[0].l);
  }
#line 3724 "ppparse.c"
    break;

  case 188: /* text_dst: IN  */
#line 1765 "ppparse.y"
  {
	/* as we need this word, which is valid as replacement,
	   also for qualification, we need to explicit make it
	   a word if given alone */
	(yyval.l) = ppp_list_add (NULL, "IN");
  }
#line 3735 "ppparse.c"
    break;

  case 189: /* text_dst: OF  */
#line 1772 "ppparse.y"
  {
	/* as we need this word, which is valid as replacement,
	   also for qualification, we need to explicit make it
	   a word if given alone */
	(yyval.l) = ppp_list_add (NULL, "OF");
  }
#line 3746 "ppparse.c"
    break;

  case 190: /* text_partial_src: "==" "Word or Literal" "=="  */
#line 1782 "ppparse.y"
  {
	(yyval.p) = ppp_replace_src (ppp_list_add (NULL, (yyvsp[-1].s)), 0);
  }
#line 3754 "ppparse.c"
    break;

  case 191: /* text_partial_src: "Word or Literal"  */
#line 1786 "ppparse.y"
  {
	(yyval.p) = ppp_replace_src (ppp_list_add (NULL, literal_token ((yyvsp[0].s), 0)),
			      ((yyvsp[0].s)[0] == '\'' || (yyvsp[0].s)[0] == '"'));
  }
#line 3763 "ppparse.c"
    break;

  case 192: /* text_partial_dst: "==" "=="  */
#line 1794 "ppparse.y"
  {
	(yyval.l) = NULL;
  }
#line 3771 "ppparse.c"
    break;

  case 193: /* text_partial_dst: "==" "Word or Literal" "=="  */
#line 1798 "ppparse.y"
  {
	(yyval.l) = ppp_list_add (NULL, (yyvsp[-1].s));
  }
#line 3779 "ppparse.c"
    break;

  case 194: /* text_partial_dst: "Word or Literal"  */
#line 1802 "ppparse.y"
  {
	(yyval.l) = ppp_list_add (NULL, literal_token ((yyvsp[0].s), 1));
  }
#line 3787 "ppparse.c"
    break;

  case 195: /* token_list: "Word or Literal"  */
#line 1809 "ppparse.y"
  {
	(yyval.l) = ppp_list_add (NULL, (yyvsp[0].s));
  }
#line 3795 "ppparse.c"
    break;

  case 196: /* token_list: token_list "Word or Literal"  */
#line 1813 "ppparse.y"
  {
	(yyval.l) = ppp_list_add ((yyvsp[-1].l), (yyvsp[0].s));
  }
#line 3803 "ppparse.c"
    break;

  case 197: /* identifier: "Word or Literal"  */
#line 1820 "ppparse.y"
  {
	(yyval.l) = ppp_list_add (NULL, (yyvsp[0].s));
  }
#line 3811 "ppparse.c"
    break;

  case 198: /* identifier: identifier IN "Word or Literal"  */
#line 1824 "ppparse.y"
  {
	(yyval.l) = ppp_list_add ((yyvsp[-2].l), " ");
	(yyval.l) = ppp_list_add ((yyval.l), "IN");
	(yyval.l) = ppp_list_add ((yyval.l), " ");
	(yyval.l) = ppp_list_add ((yyval.l), (yyvsp[0].s));
  }
#line 3822 "ppparse.c"
    break;

  case 199: /* identifier: identifier OF "Word or Literal"  */
#line 1831 "ppparse.y"
  {
	(yyval.l) = ppp_list_add ((yyvsp[-2].l), " ");
	(yyval.l) = ppp_list_add ((yyval.l), "OF");
	(yyval.l) = ppp_list_add ((yyval.l), " ");
	(yyval.l) = ppp_list_add ((yyval.l), (yyvsp[0].s));
  }
#line 3833 "ppparse.c"
    break;

  case 200: /* identifier: identifier '(' subscripts ')'  */
#line 1838 "ppparse.y"
  {
	struct cb_text_list *l;

	(yyval.l) = ppp_list_add ((yyvsp[-3].l), " ");
	(yyval.l) = ppp_list_add ((yyval.l), "(");
	(yyvsp[-1].l) = ppp_list_add ((yyvsp[-1].l), ")");
	for (l = (yyval.l); l->next; l = l->next) {
		;
	}
	l->next = (yyvsp[-1].l);
  }
#line 3849 "ppparse.c"
    break;

  case 201: /* subscripts: "Word or Literal"  */
#line 1853 "ppparse.y"
  {
	(yyval.l) = ppp_list_add (NULL, (yyvsp[0].s));
  }
#line 3857 "ppparse.c"
    break;

  case 202: /* subscripts: subscripts "Word or Literal"  */
#line 1857 "ppparse.y"
  {
	(yyval.l) = ppp_list_add ((yyvsp[-1].l), " ");
	(yyval.l) = ppp_list_add ((yyval.l), (yyvsp[0].s));
  }
#line 3866 "ppparse.c"
    break;

  case 203: /* lead_trail: LEADING  */
#line 1865 "ppparse.y"
  {
	(yyval.ui) = CB_REPLACE_LEADING;
  }
#line 3874 "ppparse.c"
    break;

  case 204: /* lead_trail: TRAILING  */
#line 1869 "ppparse.y"
  {
	(yyval.ui) = CB_REPLACE_TRAILING;
  }
#line 3882 "ppparse.c"
    break;

  case 205: /* unquoted_literal: "Literal"  */
#line 1876 "ppparse.y"
  {
	/* Do not reuse unquote as some literals here may be delimited with
	   parentheses */
	char	*p = (yyvsp[0].s);
	size_t	size;

	/* Remove surrounding quotes/brackets */
	++p;
	size = strlen (p) - 1;
	p[size] = '\0';

	(yyval.s) = p;
  }
#line 3900 "ppparse.c"
    break;

  case 206: /* _override: %empty  */
#line 1895 "ppparse.y"
  {
	(yyval.ui) = 0;
  }
#line 3908 "ppparse.c"
    break;

  case 207: /* _override: OVERRIDE  */
#line 1899 "ppparse.y"
  {
	(yyval.ui) = 1U;
  }
#line 3916 "ppparse.c"
    break;

  case 208: /* _not: %empty  */
#line 1906 "ppparse.y"
  {
	(yyval.ui) = 0;
  }
#line 3924 "ppparse.c"
    break;

  case 209: /* _not: NOT  */
#line 1910 "ppparse.y"
  {
	(yyval.ui) = 1U;
  }
#line 3932 "ppparse.c"
    break;

  case 210: /* _also: %empty  */
#line 1917 "ppparse.y"
  {
	(yyval.ui) = 0;
  }
#line 3940 "ppparse.c"
    break;

  case 211: /* _also: ALSO  */
#line 1921 "ppparse.y"
  {
	(yyval.ui) = 1U;
  }
#line 3948 "ppparse.c"
    break;

  case 212: /* _last: %empty  */
#line 1928 "ppparse.y"
  {
	(yyval.ui) = 0;
  }
#line 3956 "ppparse.c"
    break;

  case 213: /* _last: LAST  */
#line 1932 "ppparse.y"
  {
	(yyval.ui) = 1U;
  }
#line 3964 "ppparse.c"
    break;


#line 3968 "ppparse.c"

      default: break;
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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

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
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= TOKEN_EOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == TOKEN_EOF)
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
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
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
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 1945 "ppparse.y"

