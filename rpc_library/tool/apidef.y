/**
 * @file apidef.y
 * @brief RPC tools--API definition file syntax definition and read/front-end processing
 *
 */

/*---------------------------------------------------------------------------*/
/* C declarative statement                                                   */
/*---------------------------------------------------------------------------*/
%{
#define YYDEBUG 1
#define YYERROR_VERBOSE 1

#include <stdio.h>
#include <stdlib.h>

/*
 * YACC to C I/F functions
 */
extern int yylex(void); /* LEX I/F */
extern int yyerror(const char *); /* dummy */

#include "apidef.h"

/* YACC to C I/F functions */
static void AddHeader(const char *header_file);
static int push_function_arg(int arg_type, int num_of_bytes,
           int is_pointer,
           int is_vararray, int is_array_size,
           const char *var_type_name,
           int is_out, const char *var_name);
static void process_function(const char *funcname);
static void free_string(char *funcname);

static int var_type;
static int num_of_bytes;
static int is_pointer;
static int in_out;
static int is_vararray;
static int is_array_size;
static char *var_type_name = NULL;

%}

/*---------------------------------------------------------------------------*/
/* Bison declarations                                                        */
/*---------------------------------------------------------------------------*/
%union {
  int ival;
  char *strval;
}

%token '"'
%left ','
%left '('
%right ')'
%left '*'
%token '{'
%token '}'
%token ';'
%token '\n'
%token <strval> rpc_INCLUDE 257
%token <strval> RPC_RESULT 258
%token <strval> rpc_NAME_DOT 259
%token <strval> rpc_NAME 260
%type <strval> funcdefs funcdef args
%type <strval> nonvoid_args arg var_type
%type <strval> var_primitive_type var_string_type var_user_defined_type
/*
%token <strval> rpc_OUT
*/
%token <strval> rpc_INOUT 261
%token <strval> rpc_CONST 262
%token <strval> rpc_VARARRAY 263
%token <strval> rpc_ARRAYSIZE 264
%token <ival> rpc_NUM 265
%token <strval> rpc_VOID 266
%token <ival> rpc_CHAR 267
%token <ival> rpc_INT 268
%token <ival> rpc_SINT 269
%token <ival> rpc_UINT 270
%token <ival> rpc_INT8 271
%token <ival> rpc_INT16 272
%token <ival> rpc_INT32 273
%token <ival> rpc_INT64 274
%token <ival> rpc_UINT8 275
%token <ival> rpc_UINT16 276
%token <ival> rpc_UINT32 277
%token <ival> rpc_UINT64 278
%token <ival> rpc_FLOAT 279
%token <ival> rpc_DOUBLE 280
%token <ival> rpc_STRING 281
%token <ival> rpc_USER_DEFINED 282
%token <ival> rpc_UNKNOWN 283

/*---------------------------------------------------------------------------*/
/* Grammar rule                                                              */
/*---------------------------------------------------------------------------*/
%%
input:  includes funcdefs
;

includes:  /* empty input */
  | includes include
;

include:  rpc_INCLUDE '<' rpc_NAME_DOT '>'
  { AddHeader($3); free_string($3); }
;

funcdefs:  funcdef
  | funcdefs funcdef
;

funcdef:  RPC_RESULT rpc_NAME '(' args ')' ';'
    { process_function($2); free_string($2);}
;

args:  rpc_VOID
  | nonvoid_args
;

nonvoid_args:  arg
    | nonvoid_args ',' arg
;

arg:  var_type rpc_NAME
    {
      if (push_function_arg(var_type, num_of_bytes, is_pointer,
          is_vararray, is_array_size,
          var_type_name, in_out, $2) < 0) {
        YYERROR;
      }
      if (var_type_name) { free_string(var_type_name); }
      var_type_name = NULL;
      free_string($2);
    }
  | var_type
    {
      if (push_function_arg(var_type, num_of_bytes, is_pointer,
          is_vararray, is_array_size,
          var_type_name, in_out, NULL) < 0) {
        YYERROR;
      }
      if (var_type_name) { free_string(var_type_name); }
      var_type_name = NULL;
    }
;

/*
 * Standard IN pointers are not allowed.
 *   When used as an array address, it is not clear how many bytes to copy.
 *   ->You are asked to declare the type and pass the address.
 *   Otherwise, you can simply pass it by value.
 */
var_type:  var_primitive_type /* INT8, .. DOUBLE */
    { is_pointer = 0; is_vararray = 0; is_array_size = 0; in_out =RPC_IN_ARG; }
  | var_primitive_type '*' /* OUT INT8 *, ... OUT DOUBLE * */
    { is_pointer = 1; is_vararray = 0; is_array_size = 0; in_out = RPC_OUT_ARG; }
  /* if allow primitive IN pointer
  | rpc_CONST var_primitive_type '*'
    { is_pointer = 1; in_out = RPC_IN_ARG; }
  */
  | var_string_type
    /* OUT STRING128, ... */
    { is_pointer = 1; is_vararray = 0; is_array_size = 0; in_out = RPC_OUT_ARG; }
  | rpc_CONST var_string_type
    /* IN STRING128, ... */
    { is_pointer = 1; is_vararray = 0; is_array_size = 0; in_out = RPC_IN_ARG; }
  | var_user_defined_type
    { is_pointer = 0; is_vararray = 0; is_array_size = 0; in_out = RPC_IN_ARG; }
  | var_user_defined_type '*'
    { is_pointer = 1; is_vararray = 0; is_array_size = 0; in_out = RPC_OUT_ARG; }
  | rpc_CONST var_user_defined_type '*'
    { is_pointer = 1; is_vararray = 0; is_array_size = 0; in_out = RPC_IN_ARG; }

  /* INOUT specification */
  | rpc_INOUT var_string_type
    /* IN STRING128, ... */
    { is_pointer = 1; is_vararray = 0; is_array_size = 0; in_out = RPC_INOUT_ARG; }
  | rpc_INOUT var_user_defined_type '*'
    { is_pointer = 1; is_vararray = 0; is_array_size = 0; in_out = RPC_INOUT_ARG; }

  /* Variable-length arrays */
  | rpc_CONST rpc_VARARRAY var_primitive_type '*'
    { is_pointer = 1; is_vararray = 1; is_array_size = 0; in_out = RPC_IN_ARG; }
  | rpc_VARARRAY var_primitive_type '*'
    { is_pointer = 1; is_vararray = 1; is_array_size = 0; in_out = RPC_OUT_ARG; }
  | rpc_INOUT rpc_VARARRAY var_primitive_type '*'
    { is_pointer = 1; is_vararray = 1; is_array_size = 0; in_out = RPC_INOUT_ARG; }
  | rpc_CONST rpc_VARARRAY var_user_defined_type '*'
    { is_pointer = 1; is_vararray = 1; is_array_size = 0; in_out = RPC_IN_ARG; }
  | rpc_VARARRAY var_user_defined_type '*'
    { is_pointer = 1; is_vararray = 1; is_array_size = 0; in_out = RPC_OUT_ARG; }
  | rpc_INOUT rpc_VARARRAY var_user_defined_type '*'
    { is_pointer = 1; is_vararray = 1; is_array_size = 0; in_out = RPC_INOUT_ARG; }

  /* Variable length array size */
  | rpc_ARRAYSIZE var_primitive_type
    { is_pointer = 0; is_vararray = 0; is_array_size = 1; in_out = RPC_IN_ARG; }
;

var_primitive_type:
  rpc_CHAR { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
  | rpc_INT { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
  | rpc_SINT { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
  | rpc_UINT { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
  | rpc_INT8 { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
  | rpc_UINT8 { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
  | rpc_INT16 { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
  | rpc_UINT16 { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
  | rpc_INT32 { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
  | rpc_UINT32 { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
  | rpc_INT64 { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
  | rpc_UINT64 { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
  | rpc_FLOAT { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
  | rpc_DOUBLE { var_type = $1; num_of_bytes = 0; var_type_name = NULL; }
;

var_string_type:  rpc_STRING rpc_NUM
    { var_type = $1; num_of_bytes = $2; var_type_name = NULL; }
;

var_user_defined_type:  rpc_NAME
  { var_type = rpc_USER_DEFINED; num_of_bytes = 0; var_type_name = $1; }
;

%%

/*---------------------------------------------------------------------------*/
/* C additional code                                                         */
/*---------------------------------------------------------------------------*/
/* Including an older bison results in an error */
/*#include "apidef.tab.h"*/

#include <unistd.h>
#include <assert.h>
#include <string.h>
/*
 * YACC/Lex interface functions/variables
 */
extern int yydebug; /* for YACC debug */
extern int yyparse(void);
extern FILE *yyin;
extern int yy_flex_debug; /* for FLEX debug */
/* my own function to free the buffer flex allocates */
extern void free_flex_buffer(void);

#ifdef DBG_ENABLE
char rpc_log_enable;
#endif

/**/
static void
AddHeader(const char *filename)
{
  ApidefAddHeader(filename);
}

static int
push_function_arg(int arg_type, int num_of_bytes, int is_pointer,
      int is_vararray, int is_array_size,
      const char *var_type_name, int in_out, const char *var_name)
{
  switch(arg_type) {
  case rpc_CHAR:
  case rpc_INT:
  case rpc_SINT:
  case rpc_UINT:
  case rpc_INT8:
  case rpc_INT16:
  case rpc_INT32:
  case rpc_INT64:
  case rpc_UINT8:
  case rpc_UINT16:
  case rpc_UINT32:
  case rpc_UINT64:
  case rpc_FLOAT:
  case rpc_DOUBLE:
  case rpc_STRING:
  case rpc_USER_DEFINED:
    return ApidefPushFunctionArg(arg_type, num_of_bytes, is_pointer,
            is_vararray, is_array_size,
            var_type_name, in_out, var_name);
    break;

  default:
    return -1;
    break;
  }
}

static void
process_function(const char *funcname)
{
  ApidefProcessFunction(funcname);
}

static void
free_string(char *s)
{
#ifdef DEBUG
  fprintf(stderr, "freed %s\n", s);
#endif
  free(s);
}

static void
extract_id(const char *filename, char **id)
{
  char *dotapi;
  char *slash;
  const char *start;

  if (id == NULL) {
    return;
  }
  dotapi = strrchr(filename, '.');
  if (dotapi == NULL) {
    return;
  }
  if (strcmp(dotapi, ".api")) {
    return;
  }

  slash = strrchr(filename, '/');
  start = filename;
  if (slash != NULL) {
    start = slash + 1;
  }

  *id = malloc((size_t)(dotapi - start + 1));
  if (*id == NULL) {
    return;
  }
  strncpy(*id, start, (size_t)(dotapi - start));
  (*id)[dotapi - start] = '\0';
}

static void
usage(const char *prog)
{
  fprintf(stdout,
    "How to use: %s [CPPFLAGS] ... API definition file name\n",
    prog);
  fprintf(stdout, "Examples1: %s XXX.api\n", prog);
  fprintf(stdout, "Examples2: %s -DSOME_DEFINES XXX.api\n", prog);
}

#define CPP_PROG "cpp"
static void free_cpp_argv(char **argv);

static char **
prepare_cpp_argv(int argc, char *argv[])
{
  char **cpp_argv;
  char **ret;

  cpp_argv = malloc(sizeof(char *) * (long unsigned int)(argc+1));
  if (cpp_argv == NULL) {
    return NULL;
  }
  memset(cpp_argv, 0, sizeof(char *) * (long unsigned int)(argc+1));
  ret = cpp_argv;

  cpp_argv[0] = malloc(strlen(CPP_PROG)+1);
  if (cpp_argv[0] == NULL) {
    free_cpp_argv(ret);
    return NULL;
  }
  strcpy(cpp_argv[0], CPP_PROG);
  cpp_argv++;

  for( ; *argv != NULL ; argv++, cpp_argv++) {
    *cpp_argv = malloc(strlen(*argv)+1);
    if (*cpp_argv == NULL) {
      free_cpp_argv(ret);
      return NULL;
    }
    strcpy(*cpp_argv, *argv);
  }
  *cpp_argv = NULL;
  return ret;
}

static void
free_cpp_argv(char **argv)
{
  char **orig; orig = argv;
  while(*argv != NULL) {
    free(*argv);
    argv++;
  }
  free(orig);
}

/** @ingroup RPCtool
 * @brief RPCtool main functions
 *
 * Perform the following processing.
 *
 * - Lexical context analysis of the API definition file(See apidef.y and apidef.l)
 * - The API function definition read from the definition file is stored in the APIDef class.
 * - Outputs stub and header files from function definitions stored in APIDef after a successful read
 */
int
main(int argc, char *argv[])
{
  int start;
  char **cpp_argv;
  int pipefd[2];
  int pid;
  int ret;

  if (argc < 2) {
    usage(argv[0]);
    return 1;
  }

  start = 1;
  yy_flex_debug = 0;

  if (!strcmp(argv[1], "-d")) {
    yydebug=1;
    yy_flex_debug = 1;
    if (argc > 2) {
      start++;
    } else {
      usage(argv[0]);
      return 1;
    }
  }

#ifdef DBG_ENABLE
  /*
   * If the second-to-last argument is a log specification
   */
  if ( ( argc >= 2 ) && (!strcmp(argv[argc-2], "-log")) ) {
    rpc_log_enable = 1;
  } else {
    rpc_log_enable = 0;
  }
#endif

  /*
   * Extract ID from last argument (API definition file name)
   */
  {
    char *moduleid;
    moduleid = NULL;
    extract_id(argv[argc-1], &moduleid);
    if (moduleid == NULL) {
      fprintf(stdout, "%s: The API definition file name is invalid.\n", argv[0]);
      usage(argv[0]);
      return 1;
    }
    ApidefDefineId(moduleid);
    free(moduleid);
  }

  /*
   * Preparing options to exec CPPs
   */
  cpp_argv = prepare_cpp_argv(argc, argv + start);
  if (cpp_argv == NULL) {
    printf("No Memory!\n");
    return 1;
  }

#define PIPE_READ 0
#define PIPE_WRITE 1

  if (pipe(pipefd) != 0) {
    perror("pipe");
    return 1;
  }

  pid = fork();
  if (pid < 0) {/* fork error */
    close(pipefd[PIPE_READ]);
    close(pipefd[PIPE_WRITE]);
    perror("fork");
    return 1;
  }
  if (pid == 0) {/* child process */
    int must_be_1;
    /*
     * force stdout to be pipefd[PIPE_WRITE]
     */
    close(pipefd[PIPE_READ]);
    close(1);
    must_be_1 = dup(pipefd[PIPE_WRITE]);
    assert(must_be_1 == 1);
    close(pipefd[PIPE_WRITE]);
    /*
     * invoke C preprocessor with flags
     */
    execvp(CPP_PROG, cpp_argv);
    perror("execvp");
    exit(0);
  } else {
    /*
     * parent process
     */
    int must_be_0;
    free_cpp_argv(cpp_argv);

    /*
     * force stdin to be pipefd[PIPE_READ]
     */
    close(pipefd[PIPE_WRITE]);
    close(0);
    must_be_0 = dup(pipefd[PIPE_READ]);
    assert(must_be_0 == 0);
    close(pipefd[PIPE_READ]);
  }

  ret = yyparse();

  free_flex_buffer();

  if (ret == 0) {/* Parsed successfully */
    //ApidefListFunctions(1);
    ApidefMakeStubs();
  } else {/* Parse error occurred */
    fputs("The APIs that have been analyzed so far are as follows.\n", stdout);
    fputs("-----start-----\n", stdout);
    ApidefListFunctions(0);
    fputs("------end------\n", stdout);
    fputs("Check this API definition\n", stdout);
  }
  ApidefFreeAllocation();

  return ret;
}

int
yyerror(const char *s)
{
  printf("\nError: %s\n", s);
  return 0;
}

int
yywrap()
{
  return 1;
}

static char const *Types[] = {
  "char",
  "int",
  "signed int",
  "unsigned int",
  "INT8",
  "INT16",
  "INT32",
  "INT64",
  "UINT8",
  "UINT16",
  "UINT32",
  "UINT64",
  "float",
  "double",
  "char *",
};

static const int TypeCodes[] = {
  rpc_CHAR,
  rpc_INT,
  rpc_SINT,
  rpc_UINT,
  rpc_INT8,
  rpc_INT16,
  rpc_INT32,
  rpc_INT64,
  rpc_UINT8,
  rpc_UINT16,
  rpc_UINT32,
  rpc_UINT64,
  rpc_FLOAT,
  rpc_DOUBLE,
  rpc_STRING,
};

const char *
TypeCodeString(const int code)
{
  int i;
  int num;
  num = sizeof(TypeCodes) / sizeof(int);

  for(i = 0 ; i < num ; i++) {
    if (code == TypeCodes[i]) {
      return Types[i];
    }
  }
  return NULL;
}

int
IsTypeCodeNumeric( const int code )
{
  int i;

  for(i = 0 ; i < sizeof(TypeCodes) / sizeof(TypeCodes[0]) ; i++) {
    if (TypeCodes[i] == rpc_FLOAT) {
      break;
    }
    if (code == TypeCodes[i]) {
      return 1;
    }
  }
  return 0;
}
