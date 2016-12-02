/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY_GETDATE_H_INCLUDED
# define YY_YY_GETDATE_H_INCLUDED
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
    tAGO = 258,
    tDAY = 259,
    tDAY_UNIT = 260,
    tDAYZONE = 261,
    tDST = 262,
    tHOUR_UNIT = 263,
    tID = 264,
    tMERIDIAN = 265,
    tMINUTE_UNIT = 266,
    tMONTH = 267,
    tMONTH_UNIT = 268,
    tSEC_UNIT = 269,
    tSNUMBER = 270,
    tUNUMBER = 271,
    tYEAR_UNIT = 272,
    tZONE = 273
  };
#endif
/* Tokens.  */
#define tAGO 258
#define tDAY 259
#define tDAY_UNIT 260
#define tDAYZONE 261
#define tDST 262
#define tHOUR_UNIT 263
#define tID 264
#define tMERIDIAN 265
#define tMINUTE_UNIT 266
#define tMONTH 267
#define tMONTH_UNIT 268
#define tSEC_UNIT 269
#define tSNUMBER 270
#define tUNUMBER 271
#define tYEAR_UNIT 272
#define tZONE 273

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 177 "getdate.y" /* yacc.c:1909  */

    int			Number;
    enum _MERIDIAN	Meridian;

#line 95 "getdate.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_GETDATE_H_INCLUDED  */
