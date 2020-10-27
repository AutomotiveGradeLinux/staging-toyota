/*
 * @copyright Copyright (c) 2016-2019 TOYOTA MOTOR CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file apidef.cc
 * @brief RPC tools--Main processing(Implementations of APIDef classes)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include <other_service/rpc.h>

#include "apidef.h"
#include "apidef.tab.h"

static APIDef *API_Def;

extern "C" {  // C interface functions

void
ApidefDefineId(const char *id) {
  if (id != NULL) {
    if (API_Def == NULL) {
      API_Def = new APIDef();
    }
    API_Def->DefineId(id);
  }
}

void
ApidefAddHeader(const char *filename) {
  if (filename != NULL) {
    if (API_Def == NULL) {
      API_Def = new APIDef();
    }
    API_Def->AddHeader(filename);
  }
}

int
ApidefPushFunctionArg(int arg_code, int buffer_bytes, int is_pointer,
       int is_vararray, int is_array_size,
       const char *var_type_name, int in_out,
       const char *var_name) {
  if (API_Def == NULL) {
    API_Def = new APIDef();
  }
  return API_Def->AddFunctionArg(arg_code, buffer_bytes, is_pointer,
             is_vararray, is_array_size,
           var_type_name, in_out, var_name);
}

void
ApidefProcessFunction(const char *name) {
  if (API_Def == NULL) {
    API_Def = new APIDef();
  }
  API_Def->NameFunction(name);
}

void
ApidefListFunctions(int with_args) {
  if (API_Def != NULL) {
    API_Def->Print(with_args);
  }
}

void
ApidefFreeAllocation(void) {
  if (API_Def != NULL) {
    delete API_Def;
    API_Def = NULL;
  }
}

int
ApidefMakeStubs(void) {
  if (API_Def != NULL) {
    API_Def->MakeStubs();
    return 0;
  } else {
    return 1;
  }
}

} // extern "C"

/*
 * Retrieving Argument Names Without Decorators
 */
void
Arg::GetUndecolatedName(string& name /* OUT */, int num) {
  name = "v";
  if (num > 0) {
    /* Making Variable Names Sequential Numbers */
    char num_str[5];
    sprintf(num_str, "%d", num);
    name += num_str;
  } else {
    /* Use a user-supplied pseudo-variable name */
    name = m_name;
  }
}

int
Arg::PrintUndecoratedName(fstream& out, int num) {
  /* Pointer argument */
  if (m_is_pointer) {
    out << "*";
  }

  if (num > 0) {
    /* Making Variable Names Sequential Numbers */
    out << "v" << num;
  } else {
    /* Use a user-supplied pseudo-variable name */
    out << m_name;
  }

  return 0;
}

/*
 * Outputs one argument in a function prototype declaration
 * out: Destination file
 * num: Handling of Formal Argument Names
 *      0 :Formal argument name specified in the API definition file
 *      Non-zero: Use "v" + specified number(Examples: v1, v2, ...)
 */
int
Arg::PrintPrototype(fstream& out, int num) {
  const char *type = TypeCodeString(m_code);
  if (type == NULL) {
    type = m_type_name.c_str();
  }
  if (type == NULL) {
    cout << "Internal Error Occurrence!!\n";
    return 1;
  }

  /* Const non-output pointers */
  if ((m_in_out & RPC_OUT_ARG) == 0 && (m_bytes != 0 || m_is_pointer != 0)) {
    out << "const ";
  }
  out << type << " ";
  if (m_is_pointer && m_bytes == 0) {/* Non-string pointer arguments */
    out << "*";
  }

  if (num > 0) {/* Making variable names sequential numbers */
    out << "v" << num;
  } else {/* Use a user-supplied pseudo-variable name */
    out << m_name;
  }

  /* Variable-length arrays */
  if (m_is_vararray != 0) {
    out << "/* VAR_ARRAY */";
  }

  /* Variable-length arrays length */
  if (m_is_array_size != 0) {
    out << "/* VAR_SIZE */";
  }

  /*
   * Commenting on OUT/INOUT Arguments
   * Maximum number of bytes of a string argument added to a comment
   */
  if ((m_in_out & RPC_OUT_ARG) != 0 || m_bytes != 0) {
    out << "/* ";
    if ((m_in_out & RPC_OUT_ARG) != 0) {
      if ((m_in_out & RPC_IN_ARG) != 0) {
        out << "IN";
      }
      out << "OUT ";
    }
    if (m_bytes) {
      out << m_bytes << "Byte ";
    }
    out << "*/";
  }

  return 0;
}

int
Arg::CreateMarshallArgs(fstream& out, int num) {
  return CreateMarshallArgs(out, num, (RPC_IN_ARG|RPC_OUT_ARG));
}

int
Arg::CreateMarshallArgs(fstream& out, int num, string &array_size_name) {
  return CreateMarshallArgs(out, num, (RPC_IN_ARG|RPC_OUT_ARG), array_size_name);
}

int
Arg::CreateMarshallArgs(fstream& out, int num, int in_out) {
  string dummy = "";
  return CreateMarshallArgs(out, num, in_out, dummy);
}

/*
 * The arguments to rpc_marshall_args() are generated for each API argument.
 * for each API argument.
 * out: Destination file
 * num: Number given to the variable name of the argument (Examples: 1 for v1)
 * in_out: Specifying the IN or OUT Attributes
 *         Output only if the argument has the specified attribute
 */
int
Arg::CreateMarshallArgs(fstream& out, int num, int in_out, string &array_size_name) {
  if ((m_in_out & in_out) != 0) {
    /*
     * Collectively passes the argument type, pointer/non-pointer, IN/OUT attribute, 
     * and the maximum number of bytes of the argument type to one integer.
     */
    RPC_marshall_flag flag;
    flag.bits.code = m_code & ((1<<RPC_MARSHALL_FLAG_BITS_CODE)-1);
    flag.bits.is_vararray = ((array_size_name.size() != 0) ? 1: 0) &
                            ((1<<RPC_MARSHALL_FLAG_BITS_IS_VARARRAY)-1);
    flag.bits.is_pointer = m_is_pointer & ((1<<RPC_MARSHALL_FLAG_BITS_IS_POINTER)-1);
    flag.bits.in_out = m_in_out & ((1<<RPC_MARSHALL_FLAG_BITS_IN_OUT)-1);
    flag.bits.bytes = (UINT16)(m_bytes);
    char str[11];
    sprintf(str, "0x%x", htonl(flag.all));
    out << "\t\t\t" << str;

    /*
     * Add size of user-defined type to argument
     */
    if (m_code == rpc_USER_DEFINED) {
      if (array_size_name.size() != 0) {
        out << " + ntohs(sizeof(" << m_type_name << ") * " << array_size_name << ")";
      } else {
        out << " + ntohs(sizeof(" << m_type_name << "))";
      }
    } else if (array_size_name.size() != 0) {
      out << " + ntohs(sizeof(" << TypeCodeString(m_code) << ") * " << array_size_name << ")";
    }

    out << ", v" << num;
    return 1;
  } else {
    return 0;
  }
}

int
Arg::CreateDemarshallArgs(fstream& out, int deliver_pointer, int num) {
  return CreateDemarshallArgs(out, deliver_pointer, num, (RPC_IN_ARG|RPC_OUT_ARG));
}

int
Arg::CreateDemarshallArgs(fstream& out, int deliver_pointer, int num, string &array_size_name) {
  return CreateDemarshallArgs(out, deliver_pointer, num, (RPC_IN_ARG|RPC_OUT_ARG), array_size_name);
}

int
Arg::CreateDemarshallArgs(fstream& out, int deliver_pointer, int num, int in_out) {
  string dummy = "";
  return CreateDemarshallArgs(out, deliver_pointer, num, in_out, dummy);
}

/*
 * The arguments to rpc_demarshall_args() are generated for each API argument.
 * for each API argument.
 * out: Destination file
 * deliver_pointer: Passing a Variable Pointer(non-0)
 * num: Number given to the variable name of the argument (Examples: 1 for v1)
 * in_out: Specifying the IN or OUT Attributes
 *         Output only if the argument has the specified attribute
 */
int
Arg::CreateDemarshallArgs(fstream& out, int deliver_pointer, int num, int in_out, string &array_size_name) {
  if ((m_in_out & in_out) != 0) {
    /*
     * Collectively passes the argument type, pointer/non-pointer, IN/OUT attribute,
     * and the maximum number of bytes of the argument type to one integer.
     */
    RPC_marshall_flag flag;
    flag.bits.code = m_code & ((1<<RPC_MARSHALL_FLAG_BITS_CODE)-1);
    flag.bits.is_vararray = ((array_size_name.size() != 0) ? 1: 0) &
                            ((1<<RPC_MARSHALL_FLAG_BITS_IS_VARARRAY)-1);
    flag.bits.is_pointer = m_is_pointer & ((1<<RPC_MARSHALL_FLAG_BITS_IS_POINTER)-1);
    flag.bits.in_out = m_in_out & ((1<<RPC_MARSHALL_FLAG_BITS_IN_OUT)-1);
    flag.bits.bytes = (UINT16)(m_bytes);
    char str[11];
    sprintf(str, "0x%x", htonl(flag.all));
    out << "\t\t\t" << str;

    /*
     * Add size of user-defined type to argument
     */
    if (m_code == rpc_USER_DEFINED) {
        out << " + ntohs(sizeof(" << m_type_name << "))";
    }

    out << ", ";
    if (deliver_pointer) {
      /* Pass a pointer */
      out << "&";
    }
    out << "v" << num;
    return 1;
  } else {
    return 0;
  }
}

int
Function::AppendArg(int code, int bytes, int is_pointer,
         int is_vararray, int is_array_size,
         const char *var_type_name, int in_out, const char *var_name) {
  if (NumOfArgs() >= RPC_MAX_API_ARG_NUM) {
    cout << "Too many API function arguments.\n";
    return -1;
  }
  Arg *a = new Arg(code, bytes, is_pointer, is_vararray, is_array_size,
       var_type_name, in_out, var_name);
  m_args.push_back(*a);
  delete a;
  return 0;
}

/*
 * Generate prototype declarations for a single API function
 * out: Destination file
 */
int
Function::PrintPrototype(fstream& out, int server) {
  int ret = 0;

  const char *return_type
    = server ? RPC_API_SERVER_RETURN : RPC_API_CLIENT_RETURN;
#ifdef DBG_ENABLE
  if ((server) || (!rpc_log_enable)) {
    out << return_type << " " << m_name << "(";
  } else {
    out << return_type << " __" << m_name << "(";
  }
#else
  out << return_type << " " << m_name << "(";
#endif

  list<Arg>::size_type num_args = m_args.size();
  if (num_args > 0) {
    list<Arg>::iterator a;
    a = m_args.begin();
    for (list<Arg>::size_type i = 1; i <= num_args; ++i, ++a) {
      a->PrintPrototype(out);
#ifdef DBG_ENABLE
      if ((server) || (!rpc_log_enable)) {
        if (i != num_args) {
          out << ", ";
        }
      } else {
  out << ", ";
      }
#else
      if (i != num_args) {
  out << ", ";
      }
#endif
    }
  } else {
#ifdef DBG_ENABLE
    if ((server) || (!rpc_log_enable)) {
      out << "void";
    }
#else
    out << "void";
#endif
  }
#ifdef DBG_ENABLE
  /* Debug information */
  if ((!server) && (rpc_log_enable)) {
    out << "const char *filename, const char *funcname, int line ";
  }
#endif
  out << ");\n";
  return ret;
}

#ifdef DBG_ENABLE
int
Function::PrintMacro(fstream& out) {
  int ret = 0;
  out << "#define " << m_name << "(";

  int num_args = m_args.size();
  if (num_args > 0) {
    int i;
    char c;
    for (i = 1, c = 'a'; i <= num_args; i++, c++) {
      out << c;
      if (i != num_args) {
        out << ", ";
      }
    }
    out << ") \\\n\t";

    /* Entity */
    out << "__" << m_name << "(";
    for (i = 1, c = 'a'; i <= num_args; i++, c++) {
      out << c << ", ";
    }
    out << "__FILE__, __func__, __LINE__)\n";
  } else { /* Without arguments */
    out << ") " << "__" << m_name << "(__FILE__, __func__, __LINE__)\n";
  }

  return ret;
}
#endif

int
APIDef::MakeHeaderFiles(int server) {
  char filename[MAX_FILENAME_LEN+1];
  sprintf(filename, "%s%s", m_lowerid.c_str(),
    (server ? SERVER_HEADER_FILE : CLIENT_HEADER_FILE));
  int ret = 0;

  fstream out(filename, ios::out);
  if (!out.is_open()) {
    cout << "ERROR: File " << filename << " Could not open.\n";
    return 1;
  }
  cout << "info: Header file " << filename << " Creating.\n";

  const char *define_str =
    server ? SERVER_HEADER_DEFINE : CLIENT_HEADER_DEFINE;
  const char *title = server ? SERVER_HEADER_TITLE : CLIENT_HEADER_TITLE;

  out << "#ifndef _" << m_lowerid << define_str << "\n";
  out << "#define _" << m_lowerid << define_str << "\n";
  out << "/*\n";
  out << " * " << m_id << " " << title << " " << filename << "\n";
  out << " *\n";
  out << RPCTOOL_WARNING_STRING << "\n";
  out << " */\n\n";

  list<string>::iterator i;
  for (i = m_headers.begin(); i != m_headers.end(); ++i) {
    out << "#include <" << *i << ">\n";
  }
  out << "\n";

  if (server) {

    out << EXTERN_C_START << "\n";

      out << RPC_API_DISPATCH_RETURN << " "
    << m_id << RPC_API_DISPATCH_FUNC_FULL << ";\n";

      out << "#ifdef RPC_DISPATCH_FUNC\n"
    << "#undef RPC_DISPATCH_FUNC\n"
    << "#define RPC_DISPATCH_FUNC " << m_id  << RPC_API_DISPATCH_FUNC_NAME
    << "\n#else /* !RPC_DISPATCH_FUNC */\n"
    << "#error \"Please include <rpc.h> first!!\"\n"
    << "#endif /* !RPC_DISPATCH_FUNC */\n\n";

    out << EXTERN_C_END << "\n";

  }
#ifdef DBG_ENABLE
  else if (rpc_log_enable) {
    /* DEBUG INFORMATION EMBEDDED MACRO */
    out << "/* Debug information embedded macro definition */\n";
    list<Function>::iterator f;
    for (f = m_funcs.begin(); f != m_funcs.end(); ++f) {
      if (f->PrintMacro(out) != 0) {
        ret = 1;
        break;
      }
    }
    out << "\n";
  }
#endif

  out << EXTERN_C_START << "\n";

  out << "/* API definitions */\n";
  list<Function>::iterator f;
  for (f = m_funcs.begin(); f != m_funcs.end(); ++f) {
    if (f->PrintPrototype(out, server) != 0) {
      ret = 1;
      break;
    }
  }
  out << "\n";

  out << EXTERN_C_END << "\n";

  out << "#endif /* _" << m_lowerid << define_str << " */\n";
  out.close();
  return ret;
}

int
Function::PrintServerStub(fstream& out) {
  int num_args = NumOfArgs();

  out << "\tcase " << RPC_API_NUM_PREFIX << m_name << ": {\n";

  list<Arg>::iterator a = m_args.begin();
  for (int i = 1; i <= num_args; ++i, ++a) {
    const char *type = TypeCodeString(a->Code());
    if (type == NULL) {
      type = (a->TypeName()).c_str();
    }
    if (type == NULL) {
      return 1;
    }
    out << "\t\t" << type << " ";
    if (a->Bytes() > 0 || a->IsPointer()) {/* Pointer-passing argument */
      if (a->Bytes() > 0) {
  out << "v" << i << " = NULL;\n";
      } else if (a->IsPointer()) {
  out << "*v" << i << " = NULL;\n";
      }
    } else {/* Pass-by-value argument */
      out << "v" << i << ";\n";
    }
  }
  if (num_args > 0) {
    out << "\t\tif (" << RPC_DEMARSHALL_FUNCTION
  << " (args_string, args_size, 1, " << num_args << ",\n";
    /* In the STUB of servers for all pre-call demarshall */
    /* Passing Pointers to Local Variables in Server STUB */
    /* Pointer/ Buffer will alloc the required space in demarshall_arguments() */

    a = m_args.begin();
    for (int i = 1; i <= num_args; ++i, ++a) {
      if (a->IsVararray() != 0) { /* Variable-length arrays */
        a->CreateDemarshallArgs(out, 1, i, m_array_size_name);
      } else {
        a->CreateDemarshallArgs(out, 1, i);
      }
      if (i < num_args) {
  out << ",";
      }
      out << "\n";
    }
    out << "\t\t   ) < 0) {\n";
    out << "\t\t\tgoto _" << m_name << "_Error;\n";
    out << "\t\t}\n";
  }

  out << "\t\tresult = " << m_name << "(";
  for (int i = 1; i <= num_args; i++) {
    out << "v" << i;
    if (i < num_args) {
      out << ", ";
    }
  }
  out << ");\n";

  int to_process = NumOfInOutArgs(RPC_OUT_ARG);
  if (to_process > 0) {
    /* Only OUT-arguments marshall after the server stub is called. */
    out << "\t\t/*if (result == RPC_OK) {*/\n";
    out << "\t\t    *ret_string = " << RPC_MARSHALL_FUNCTION
  << "(ret_size, 0, " << to_process << ",\n";

    a = m_args.begin();
    int processed = 0;
    int ret;
    for (int i = 1; i <= num_args; ++i, ++a) {
      if (a->IsVararray() != 0) { /* Variable-length arrays */
  ret = a->CreateMarshallArgs(out, i, RPC_OUT_ARG, m_array_size_name);
      } else {
  ret = a->CreateMarshallArgs(out, i, RPC_OUT_ARG);
      }
      if (ret) {
  processed++;
  if (processed < to_process) {
    out << ",";
  }
  out << "\n";
      }
    }
    out << "\t\t   );\n"
  << "\t\t    if (*ret_string == NULL) {\n"
  << "\t\t\tresult = " << RPC_API_SERVER_ERROR << ";\n"
  << "\t\t    }\n"
  << "\t\t/*}*/\n";
  }

  if (num_args > 0) {
    out << "_" << m_name << "_Error:\n";
  }
  int num_pointer_args = 0;
  a = m_args.begin();
  for (int i = 1; i <= num_args; ++i, ++a) {
    if (a->Bytes() || a->IsPointer()) {
      num_pointer_args++;
    }
  }
  if (num_pointer_args > 0) {
    int processed = 0;
    out << "\t\t" << RPC_MARSHALL_FREE_FUNCTION
  << "(" << num_pointer_args << ", ";
    a = m_args.begin();
    for (int i = 1; i <= num_args; ++i, ++a) {
      if (a->Bytes() || a->IsPointer()) {
  out << "v" << i;
  processed++;
  if (processed < num_pointer_args) {
    out << ", ";
  }
      }
    }
    out << ");\n";
  }
  out << "\t\tbreak;\n\t}\n";
  return 0;
}

int
APIDef::MakeServerStub(void) {
  char filename[MAX_FILENAME_LEN+1];
  sprintf(filename, "%s%s", m_lowerid.c_str(), SERVER_STUB_FILE);
  int ret = 0;

  fstream out(filename, ios::out);
  if (!out.is_open()) {
    cout << "File " << filename << " Could not open.\n";
    return 1;
  }
  cout << "info: Stub file " << filename << " Creating.\n";

  const char *title = SERVER_STUB_TITLE;

  out << "/*\n";
  out << " * " << m_id << " " << title << " " << filename << "\n";
  out << " *\n";
  out << RPCTOOL_WARNING_STRING << "\n";
  out << " */\n";
  out << "#include <" RPC_GLOBAL_HEADER_FILE << ">\n";
  out << "#include \"" << m_lowerid << SERVER_HEADER_FILE << "\"\n";
  out << "#include <netinet/in.h> /* for ntohs() */\n\n";

  int api_num = 1;
  list<Function>::iterator f;

  for (f = m_funcs.begin(); f != m_funcs.end(); ++f, ++api_num) {
    if (api_num >= (1<<16)) {
      cout << "Too many API functions.(Up to 65535)\n";
      return 1;
    }
    out << "#define " << RPC_API_NUM_PREFIX << f->Name()
  << " " << api_num << "\n";
  }
  out << "\n";

  out << "RPC_Result\n";
  out << m_id << RPC_API_DISPATCH_FUNC_FULL << "\n";
  out << "{\n";
  out << "\tRPC_Result result = " << RPC_API_SERVER_ERROR << ";\n";
  out << "\t*ret_string = NULL;\n";
  out << "\tswitch(api_num) {\n";

  for (f = m_funcs.begin(); f != m_funcs.end(); ++f) {
    if (f->PrintServerStub(out) != 0) {
      ret = 1;
      break;
    }
  }

  if (ret != 0) {
    return ret;
  }

  out << "\tdefault:\n\t\tbreak;\n\t}\n";
  out << "\treturn result;\n";
  out << "}\n";

  return ret;
}

int
Function::PrintClientStub(const char *moduleid, fstream& out) {
  int ret = 0;

  list<Arg>::iterator a;
  /* Function Names and Arguments */
  out << RPC_API_CLIENT_RETURN << "\n";
#ifdef DBG_ENABLE
  if (rpc_log_enable) {
    out << "__" << m_name << "(";
  } else {
    out << m_name << "(";
  }
#else
  out << m_name << "(";
#endif
  int num_args = (int)(m_args.size());
  if (num_args > 0) {
    a = m_args.begin();
    for (int i = 1; i <= num_args; ++i, ++a) {
      if (a->PrintPrototype(out, i)) {
  return 1;
      }
#ifdef DBG_ENABLE
      if (rpc_log_enable) {
        out << ", ";
      } else {
        if (i < num_args) {
          out << ", ";
        }
      }
#else
      if (i < num_args) {
        out << ", ";
      }
#endif
    }
  } else {
#ifndef DBG_ENABLE
    out << "void";
#else
    if (!rpc_log_enable) {
      out << "void";
    }
#endif
  }
#ifdef DBG_ENABLE
  if (rpc_log_enable) {
    out << "const char *filename, const char *funcname, int line ";
  }
#endif
  out << ")\n";

  out << "{\n";

  /* Contents of stub functions */

  /* If the argument is present and the variable-length array does not exist */
  if ((num_args > 0) && (m_array_size_pos == 0)) {
    int is_exist_mytype = 0;
    /* Restricted specifications */
    out << "#ifdef RPC_STATIC_ASSERT\n";
    out << "\tchar RPC_TOTAL_ARGSIZE_ERROR_in_" << m_name << "[\n";
    out << "\t(((";
    a = m_args.begin();
    for (int i = 1; i <= num_args; ++i, ++a) {
      if (i > 1) {
        out << " + ";
      }
      out << "sizeof(";
      a->PrintUndecoratedName(out, i);
      out << ")";
      /* For user types */
      if ((!is_exist_mytype) && (NULL == TypeCodeString(a->Code()))) {
        is_exist_mytype = 1;
      }
    }
    out << ") > RPC_MAX_API_ARG_TOTAL_SIZE) ? -1: 1)\n";
    out << "\t]__attribute__((unused));\n";

    /* Have a user type */
    if (is_exist_mytype) {
      char c[3];
      a = m_args.begin();
      for (int i = 1; i <= num_args; ++i, ++a) {
        if (NULL == TypeCodeString(a->Code())) {
          sprintf(c, "%d", i);
          out << "\tchar RPC_ARGSIZE_ERROR_in_" << m_name << "_arg" << c;
          out << "[\n\t(sizeof(";
          a->PrintUndecoratedName(out, i);
          out << ") > RPC_MAX_API_ARG_SIZE) ? -1: 1\n";
          out << "\t]__attribute__((unused));\n";
        }
      }
    }

    out << "#endif /* RPC_STATIC_ASSERT */\n";

  }

  out << "\tRPC_Result result = " << RPC_API_CLIENT_ERROR << ";\n";
  out << "\tunsigned int args_size = 0, ret_size;\n";
  out << "\tchar retcode[9];\n";

  if (num_args > 0) {
    /* Advance preparation==Marshalling of arguments */
    out << "\tchar *args_string = " << RPC_MARSHALL_FUNCTION
  << "(&args_size, 1, " << num_args << ",\n";
    /* In the clients STUB for all pre-call marshall */
    a = m_args.begin();
    for (int i = 1; i <= num_args; ++i, ++a) {
      if (a->IsVararray() != 0) { /* Variable-length arrays */
  a->CreateMarshallArgs(out, i, m_array_size_name);
      } else {
        a->CreateMarshallArgs(out, i);
      }
      if (i < num_args) {
  out << ",";
      }
      out << "\n";
    }
    out << "\t);\n";
    out << "\tif (args_string == NULL) {\n"
  << "\t\tgoto _" << m_name << "_Error;\n\t}\n";
  } else {
    out << "\tchar *args_string = NULL;\n";
  }

#ifdef DBG_ENABLE
  if (rpc_log_enable) {
    out << "\tif (" << moduleid << "_record_dbg_log(filename, funcname, line, \"";
    out << m_name << "\") != 0) {\n";
    out << "\t\tgoto _" << m_name << "_Error;\n\t}\n";
  }
#endif

  /* RPC API call */
  out << "\tchar *ret_string = NULL; /* pgr0431 */\n";
#ifdef RPC_STRING_ID
  out << "\tresult=RPC_API_call(\"" << moduleid << "\", ";
#else
  out << "\tresult=RPC_API_call(" << moduleid << "_RPC_ID, ";
#endif
  out << RPC_API_NUM_PREFIX << m_name;
  out << ", args_string, args_size, &ret_string, &ret_size);\n";

  // 2007.08.25 To avoid over run within the sscanf
  // Locally copies strings needed for determining return values and uses strtol instead of sscanf
  // (Since there is no guarantee that the byte sequence to be grabbed by communication is 0-terminated)
  out << "\n"
      << "\tif (result == RPC_OK && ret_string != NULL) {\n"
      << "\t    strncpy(retcode, ret_string, 8);\n"
      << "\t    retcode[8] = '\\0';\n"
      << "\t    result = (RPC_Result)strtoul(retcode, NULL, 16);\n";

  /* Post-processing==Arguments de-marshalling, free() */
  int num_of_out_args = NumOfInOutArgs(RPC_OUT_ARG);
  if (num_of_out_args > 0) {
    out << "\t    if (" << RPC_DEMARSHALL_FUNCTION
  << "(ret_string + RPC_RETCODE_LEN, ret_size - RPC_RETCODE_LEN, 0, "
  << num_of_out_args << ",\n";

    a = m_args.begin();
    int processed = 0;
    int ret;
    for (int i = 1; i <= num_args; ++i, ++a) {
      if (a->IsVararray() != 0) { /* Variable-length arrays */
        ret = a->CreateDemarshallArgs(out, 0, i, RPC_OUT_ARG, m_array_size_name);
      } else {
        ret = a->CreateDemarshallArgs(out, 0, i, RPC_OUT_ARG);
      }
      if (ret) {
  processed++;
  if (processed < num_of_out_args) {
    out << ",";
  }
  out << "\n";
      }
    }
    out << "\t   ) < 0) {\n"
  << "\t\tresult = " << RPC_API_CLIENT_ERROR << ";\n"
  << "\t    }\n";
  }

  out << "\t} else {\n\t    //result = " << RPC_API_CLIENT_ERROR << ";\n"
      << "\t}\n";

  out << "\t" << RPC_RETURN_FREE_FUNCTION << "(ret_string);\n";
  out << "\t" << RPC_MARSHALL_FREE_FUNCTION << "(1, args_string);\n";

  /* END, RETURN */
#ifndef DBG_ENABLE
  if (num_args)
#else
  if ((num_args)||(rpc_log_enable))
#endif
  {
    out << "_" << m_name << "_Error:\n";
  }

  out << "\treturn result;\n}\n";

  return ret;
}

int
APIDef::MakeClientStub(void) {
  char filename[MAX_FILENAME_LEN+1];
  sprintf(filename, "%s%s", m_lowerid.c_str(), CLIENT_STUB_FILE);
  int ret = 0;

  fstream out(filename, ios::out);
  if (!out.is_open()) {
    cout << "File " << filename << " Could not open.\n";
    return 1;
  }
  cout << "info: Stub file " << filename << " Creating.\n";

  const char *title = CLIENT_STUB_TITLE;

  out << "/*\n";
  out << " * " << m_id << " " << title << " " << filename << "\n";
  out << " *\n";
  out << RPCTOOL_WARNING_STRING << "\n";
  out << " */\n";
  out << "#include <" RPC_GLOBAL_HEADER_FILE << ">\n";
  out << "#include <" << m_lowerid << CLIENT_HEADER_FILE << ">\n\n";
  out << "#include <stdio.h> /* for sscanf() */\n";
  out << "#include <stdlib.h> /* for getenv() */\n";
  out << "#include <string.h> /* for strncpy() */\n";
  out << "#include <netinet/in.h> /* for ntohs() */\n\n";

  int api_num = 1;
  list<Function>::iterator f;
  for (f = m_funcs.begin(); f != m_funcs.end(); ++f, ++api_num) {
    if (api_num >= (1<<16)) {
      cout << "Too many API functions.(Up to 65535)\n";
      return 1;
    }
    out << "#define " << RPC_API_NUM_PREFIX << f->Name()
  << " " << api_num << "\n";
  }
  out << "\n";

  /* Specification Restriction Debug Constants */
  out << "/*#define RPC_STATIC_ASSERT*/\n";
  out << "\n";

#ifdef DBG_ENABLE
  if (rpc_log_enable) {
    int m_id_work_cnt;
    string m_id_work = m_id;
    for (m_id_work_cnt = 0; m_id_work_cnt < m_id_work.length(); m_id_work_cnt++) {
      m_id_work[m_id_work_cnt] = toupper(m_id_work[m_id_work_cnt]);
    }
    out << "int\n" << m_id << "_record_dbg_log(const char *filename, " <<
    "const char *funcname, int line, const char *apiname)\n";
    out << "{\n\t";
    out << "if (getenv(\"" << m_id_work << "_RPC_LOG\") != NULL) {\n\t\t";
    out << "return RPC_record_dbg_log(filename, funcname, line, apiname);\n\t";
    out << "}\n\t";
    out << "return 0;\n";
    out << "}\n\n";
  }
#endif

  /* API definitions */
  for (f = m_funcs.begin(); f != m_funcs.end(); ++f) {
    if (f->PrintClientStub(m_id.c_str(), out) != 0) {
      ret = 1;
      break;
    }
  }
  return ret;
}

int
Arg::IsArraySize(void) {
  if (m_is_array_size != 0) {
    if (IsTypeCodeNumeric(m_code) == 1) {
      return 1;
    } else {
      cout << "Variable-length array length specification variables must be integers.\n";
      return -1;
    }
  }
  return 0;
}

int
Function::CheckFuncArraySize(void) {
  int num_args = (int)(m_args.size());
  list<Arg>::iterator a;
  a = m_args.begin();
  for (int i = 1; i <= num_args; ++i, ++a) {
    int ret = a->IsArraySize();
    if (ret > 0) {
      if (m_array_size_pos != 0)
      {
        cout << "Two or more variable array length specification arguments exist.\n";
        return 1;
      }
      m_array_size_pos = i;
      a->GetUndecolatedName(m_array_size_name, i);
    } else if (ret < 0) {
      return 1;
    }
  }
  return 0;
}

int
APIDef::CheckAllArraySize(void) {
  list<Function>::iterator f;
  for (f = m_funcs.begin(); f != m_funcs.end(); ++f) {
    if (f->CheckFuncArraySize() != 0) {
      return 1;
    }
  }
  return 0;
}

int
APIDef::MakeStubs(void) {
  if (m_id.size() == 0) {
    cout << "The module name is not specified.\n";
    return 1;
  }

  if (m_id.size() > MAX_FILENAME_LEN - strlen(SERVER_STUB_FILE)) {
    cout << "The module name is too long.\n";
    return 1;
  }

  /* Pre-examine the ARRAYSIZE specification */
  //cout << "<check_all_array_size>\n";
  if (CheckAllArraySize() != 0) {
    return 1;
  }

  //cout << "<MakeHeaderFiles(0)>\n";
  if (MakeHeaderFiles(1) != 0) {
    return 1;
  }

  //cout << "<MakeHeaderFiles(1)>\n";
  if (MakeHeaderFiles(0) != 0) {
    return 1;
  }

  if (MakeServerStub() != 0) {
    return 1;
  }

  if (MakeClientStub() != 0) {
    return 1;
  }

  return 0;
}

void
Arg::Print(void) {
  cout << "\t";
  if (m_name.size() > 0) {
    cout << "Variable name=" << m_name << " ";
  }
  cout << "Type=" << TypeCodeString(m_code) << "In bytes=" << m_bytes << " ";
  if (m_is_pointer) {
    cout << "Pointer ";
  }
  if ((m_in_out & RPC_OUT_ARG) != 0) {
    cout << "Output ";
  }
  cout << "\n";
}

void
Function::Print(int with_args) {
  printf("Function name=%s\n", m_name.c_str());
  if (with_args) {
    list<Arg>::iterator a;
    for (a = m_args.begin(); a != m_args.end(); ++a) {
      a->Print();
    }
  }
}

void
APIDef::Print(int with_args) {
  list<Function>::iterator f;
  for (f = m_funcs.begin(); f != m_funcs.end(); ++f) {
    f->Print(with_args);
  }
}

void
APIDef::IdTolower(void) {
  char *lower = new char[m_id.size() + 1];
  strcpy(lower, m_id.c_str());
  char *p;
  for (p = lower; *p; p++) {
    *p = (char)(tolower(*p));
  }
  m_lowerid = lower;
  delete[] lower;
}
