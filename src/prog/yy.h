#ifndef _YY_H_
#define _YY_H_

extern FILE *yyout, *yyin;
extern int yyprevious, *yyfnd;
extern char yyextra [];
extern char yytext [];
extern int yylex(void);
extern int yyleng;
extern int yyback(int *, int);
extern int yyless(int);
extern int yyinput(void);
extern int yyoutput(int);
extern int yyunput(int);
extern int yyreject(void);
extern int yyracc(int);
extern int yywrap(void);

#endif

