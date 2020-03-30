/**
 * @file apidef.l
 * @brief RPC tools--Defining lexical analysis rules for API definition files
 *
 */
/** @ingroup RPCtool
 * Defining Lexical Analysis Rules for Use with RPC Tools (apidef)
 */
%{
#include <string.h>
#include <stdlib.h>
#include "apidef.tab.h"
%}

ALPHA    [A-Za-z_]
ALPHANUM  [0-9\-A-Za-z_]
ALPHANUMDOT  [0-9.\-A-Za-z_/]

%x include
%s apidef
%x str_num

%%
INCLUDE      { BEGIN(include); return rpc_INCLUDE; }
<include>\<    { return '<'; }
<include>{ALPHANUMDOT}+  {
      yylval.strval = malloc(strlen(yytext)+1);
      strcpy(yylval.strval, yytext);
      return rpc_NAME_DOT;
      }
<include>\>    { BEGIN(INITIAL); return '>'; }
<include>.    /* ignore */

RPC_Result    { BEGIN(apidef); return RPC_RESULT; }

<apidef>STRING/[0-9]+    { BEGIN(str_num);
      yylval.ival = rpc_STRING; return rpc_STRING; }
<str_num>[0-9]+    {
      BEGIN(apidef);
      sscanf(yytext, "%d", &yylval.ival);
      return rpc_NUM;
      }
<str_num>.    /* ignore */

<apidef>{
char  { yylval.ival = rpc_CHAR; return rpc_CHAR; }
int  { yylval.ival = rpc_INT; return rpc_INT; }
signed[ \t\n]+int  { yylval.ival = rpc_SINT; return rpc_SINT; }
unsigned[ \t\n]+int  { yylval.ival = rpc_UINT; return rpc_UINT; }
INT8  { yylval.ival = rpc_INT8; return rpc_INT8; }
INT16  { yylval.ival = rpc_INT16; return rpc_INT16; }
INT32  { yylval.ival = rpc_INT32; return rpc_INT32; }
INT64  { yylval.ival = rpc_INT64; return rpc_INT64; }
UINT8  { yylval.ival = rpc_UINT8; return rpc_UINT8; }
UINT16  { yylval.ival = rpc_UINT16; return rpc_UINT16; }
UINT32  { yylval.ival = rpc_UINT32; return rpc_UINT32; }
UINT64  { yylval.ival = rpc_UINT64; return rpc_UINT64; }
float  { yylval.ival = rpc_FLOAT; return rpc_FLOAT; }
double  { yylval.ival = rpc_DOUBLE; return rpc_DOUBLE; }
void  { yylval.ival = rpc_VOID; return rpc_VOID; }
IN  {
  fprintf(stderr,
  "Specification change: IN Pointer argument needs to be prefixed \"const\" instead of \"IN\".\n"
  );
  return rpc_UNKNOWN;
  }
OUT  {
  fprintf(stderr,
  "Specification change: OUT argument no longer needs to be prefixed with \"OUT\".\n"
  );
  /* ignore */
  }
INOUT  { yylval.ival = rpc_INOUT; return rpc_INOUT; }
const  { yylval.ival = rpc_CONST; return rpc_CONST; }
VARARRAY  { yylval.ival = rpc_VARARRAY; return rpc_VARARRAY; }
ARRAYSIZE  { yylval.ival = rpc_ARRAYSIZE; return rpc_ARRAYSIZE; }

{ALPHA}+{ALPHANUM}*  {
      yylval.strval = malloc(strlen(yytext)+1);
      strcpy(yylval.strval, yytext);
      return rpc_NAME;
      }

\(  { return '('; }
\)  { return ')'; }
\*  { return '*'; }
,  { return ','; }
\;  { BEGIN(INITIAL); return ';'; }

} /* end of <apidef> */

#.*\n  /* ignore CPP directives */

{ALPHANUMDOT}+  {
    fprintf(stderr, "Unknown keyword %s\n", yytext);
    return rpc_UNKNOWN;
    }

[ \t\n]+  /* eat up whitespace */

%%
#ifdef OLD_STUFFS
%x portdef
%x addr_num
PORT      { BEGIN(portdef); return DEFINE_PORT; }
<portdef>[0-9]+    {
      BEGIN(INITIAL);
      sscanf(yytext, "%d", &yylval.ival);
      return NUM;
      }

<apidef>ADDR/[0-9]+  { BEGIN(addr_num); yylval.ival = _ADDR; return _ADDR; }
<addr_num>[0-9]+  {
      BEGIN(apidef);
      sscanf(yytext, "%d", &yylval.ival);
      return NUM;
      }
#endif

/*
 * function to free the buffer which flex allocates but not frees
 */
void
free_flex_buffer(void)
{
  yy_delete_buffer(YY_CURRENT_BUFFER);
}

/*
 * dummy function to avoid a warning of not using 'yyunput'
 */
void
dummy(void)
{
  yyunput(0, 0);
}
