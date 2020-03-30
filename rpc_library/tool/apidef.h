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
 * @file apidef.h
 * @brief RPC tools--Function/macro definitions and APIDef class definitions
 */
/** @addtogroup RPCtool
 *
 * @brief Using the RPC Tool (rpc_apidef)
 *
 * rpc_apidef [CPP option...] API definition file name
 *
 * - CPP option
 *   The RPC tool calls the C preprocessor (CPP) internally 
 *   for processing comments and #if directives.
 *   Here, you can specify options to be given to the CPP, such as symbol definitions 
 *   for conditional branch descriptions in the API definition file.
 * - API definition file name
 *   Pathname to the API definition file.
 *   The file name must be thread ID + ".api".
 *
 * The output file is as follows.
 * "*" is a lowercase version of the API definition file name preceding ".api".
 * - *_api.h(Client header files)
 * - *_api_stub.c(Client stub file)
 * - *_srvr.h(Server header file)
 * - *_srvr_stub.c(Server stub file)
 *
 * Examples:
 * - Examples1: rpc_apidef -DTEST XXX.api
 * - Examples2: rpc_apidef XXX.api
 * - => Xxx_api.h, xxx_api_stub.c, ... To the output
 */
#ifndef RPC_LIBRARY_TOOL_APIDEF_H_
#define RPC_LIBRARY_TOOL_APIDEF_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef DBG_ENABLE
extern char rpc_log_enable;
#endif

/* C++ to C I/F functions */
const char *TypeCodeString(const int code);
int IsTypeCodeNumeric(const int code);

/*
 * C to C++ I/F functions (APIDef class)
 */
void ApidefDefineId(const char *id);
void ApidefAddHeader(const char *filename);
int ApidefPushFunctionArg(int arg_code, int buffer_bytes, int is_pointer,
           int is_vararray, int is_array_size,
           const char *var_type_name, int in_out,
           const char *var_name);
void ApidefProcessFunction(const char *funcname);
void ApidefListFunctions(int with_args);
void ApidefFreeAllocation(void);
int ApidefMakeStubs(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#define RPC_IN_ARG 0x01
#define RPC_OUT_ARG 0x02
#define RPC_INOUT_ARG (RPC_IN_ARG|RPC_OUT_ARG)

#define MAX_FILENAME_LEN 63
#define SERVER_HEADER_FILE "_srvr.h"
#define CLIENT_HEADER_FILE "_api.h"
#define SERVER_STUB_FILE "_srvr_stub.c"
#define CLIENT_STUB_FILE "_api_stub.c"
#define SERVER_HEADER_DEFINE "_srvr_h_"
#define CLIENT_HEADER_DEFINE "_api_h_"

#define SERVER_HEADER_TITLE "Server header files"
#define CLIENT_HEADER_TITLE "Client header files"
#define SERVER_STUB_TITLE "Server stub file"
#define CLIENT_STUB_TITLE "Client stub file"

#define RPC_MARSHALL_FUNCTION "RPC_marshall_arguments"
#define RPC_DEMARSHALL_FUNCTION "RPC_demarshall_arguments"
#define RPC_MARSHALL_FREE_FUNCTION "RPC_marshall_free"
#define RPC_RETURN_FREE_FUNCTION "RPC_free_return_string"

#define RPC_API_SERVER_RETURN "RPC_Result"
#define RPC_API_SERVER_ERROR "RPC_ERR_Fatal"
#define RPC_API_CLIENT_RETURN "RPC_Result"
#define RPC_API_CLIENT_ERROR "RPC_ERR_Fatal"
#define RPC_API_DISPATCH_TYPE "RPC_dispatch_func_t"
#define RPC_API_DISPATCH_RETURN "RPC_Result"
#define RPC_API_DISPATCH_FUNC_FULL "_API_dispatch(UINT16 api_num, const char *args_string, unsigned int args_size, char **ret_string, unsigned int *ret_size)"  // NOLINT (readability/nolint)
#define RPC_API_DISPATCH_FUNC_NAME "_API_dispatch"
#define RPC_API_NUM_PREFIX "RPC_API_NUM_"
#define RPC_GLOBAL_HEADER_FILE "other_service/rpc.h"

#define RPCTOOL_WARNING_STRING \
" * The file created by the RPC tool.This file should not be edited."

#define EXTERN_C_START \
"#ifdef __cplusplus\n" \
"extern \"C\" {\n" \
"#endif\n"

#define EXTERN_C_END \
"#ifdef __cplusplus\n" \
"}\n" \
"#endif /* __cplusplus */\n"

#ifdef __cplusplus
#include <iostream>
#include <fstream>
#include <string>
#include <list>

using namespace std;  // NOLINT (readability/nolint)

/** @ingroup RPCtool
 * @class Arg
 * @brief Class that stores and processes the arguments of functions read from the API definition file
 */
class Arg {
 protected:
  int m_code;
  int m_bytes;
  int m_is_pointer;
  int m_in_out; /* Bit OR of _IN_ARG and _OUT_ARG */
  int m_is_vararray;
  int m_is_array_size;
  string m_type_name;
  string m_name;

 public:
  Arg() {
    m_code = m_bytes = m_is_pointer = m_in_out = m_is_vararray = m_is_array_size = 0;
  }
  Arg(int code, int bytes, int is_pointer,
       int is_vararray, int is_array_size,
       const char *type_name, int in_out,
       const char *var_name) {
    m_code = code;
    m_bytes = bytes;
    m_is_pointer = is_pointer;
    m_is_vararray = is_vararray;
    m_is_array_size = is_array_size;
    m_in_out = in_out;
    if (type_name) {
      m_type_name = type_name;
    }
    if (var_name) {
      m_name = var_name;
    }
  }
  ~Arg() {}

  int Code(void) { return m_code; }
  int Bytes(void) { return m_bytes; }
  int IsPointer(void) { return m_is_pointer; }
  int InOut(void) { return m_in_out; }
  string& TypeName(void) { return m_type_name; }
  void GetUndecolatedName(string& name, int num);  // NOLINT (readability/nolint)

  int PrintUndecoratedName(fstream& out, int num);
  int PrintPrototype(fstream& out, int num);
  int PrintPrototype(fstream& out) { return PrintPrototype(out, 0); }

  int CreateMarshallArgs(fstream& out, int num);
  int CreateMarshallArgs(fstream& out, int num, string &array_size_name);  // NOLINT (readability/nolint)
  int CreateMarshallArgs(fstream& out, int num, int in_out);
  int CreateMarshallArgs(fstream& out, int num, int in_out, string &array_size_name);  // NOLINT (readability/nolint)
  int CreateDemarshallArgs(fstream& out, int deliver_pointer, int num);
  int CreateDemarshallArgs(fstream& out, int deliver_pointer, int num, string &array_size_name);  // NOLINT (readability/nolint)
  int CreateDemarshallArgs(fstream& out, int deliver_pointer, int num, int in_out);
  int CreateDemarshallArgs(fstream& out, int deliver_pointer, int num, int in_out, string &array_size_name);  // NOLINT (readability/nolint)

  int IsVararray(void) { return m_is_vararray; }
  int IsArraySize(void);
  void Print(void);
};

/** @ingroup RPCtool
 * @class Function
 * @brief Class that stores and processes function definitions read from the API definition file
 */
class Function {
 protected:
  string m_name;
  list<Arg> m_args;
  int m_array_size_pos;
  string m_array_size_name;

 public:
  Function():m_array_size_name("") { m_array_size_pos = 0; }
  ~Function() {
    list<Arg>::size_type size = m_args.size();
    for (list<Arg>::size_type i = 0 ; i < size ; i++) {
      m_args.begin();
      m_args.pop_front();
    }
  }

  const char *Name(void) { return m_name.c_str(); }
  void SetName(const char *funcname) {
    m_name = funcname;
  }

  int NumOfArgs(void) {
    return (int)(m_args.size());  // NOLINT (readability/nolint)
  }
  int NumOfInOutArgs(int in_out) {
    int count = 0;
    list<Arg>::iterator a;
    for (a = m_args.begin(); a != m_args.end(); ++a) {
      if ((a->InOut() & in_out) != 0) {
        ++count;
      }
    }
    return count;
  }

  int AppendArg(int code, int bytes, int is_pointer,
     int is_vararray, int is_array_size,
     const char *var_type_name, int in_out, const char *var_name);

  int PrintPrototype(fstream& out);
  int PrintPrototype(fstream& out, int server);
#ifdef DBG_ENABLE
  int PrintMacro(fstream& out);
#endif

  int PrintServerStub(fstream& out);
  int PrintClientStub(const char *moduleid, fstream& out);

  int CheckFuncArraySize(void);
  void Print(int with_args);
};

/** @ingroup RPCtool
 * @class APIDef
 * @brief Class that stores and processes the result of reading the API definition file
 */
class APIDef {
 protected:
  list<Function> m_funcs;
  Function *m_work;
  string m_id;
  string m_lowerid;
  list<string> m_headers;

 public:
  APIDef() { m_work = 0; }
  ~APIDef() {
    list<Function>::size_type size = m_funcs.size();
    for (list<Function>::size_type i = 0; i < size; i++) {
      m_funcs.begin();
      m_funcs.pop_front();
    }
    if (m_work != NULL) {
      delete m_work;
    }
    size = m_headers.size();
    for (list<Function>::size_type i = 0; i < size; i++) {
      m_headers.begin();
      m_headers.pop_front();
    }
  }

  void IdTolower(void);
  void DefineId(const char *id) {
    m_id = id;
    IdTolower();
  }
  void AddHeader(const char *filename) {
    string str = filename;
    m_headers.push_back(str);
  }

  int AddFunctionArg(int arg_code, int buffer_bytes, int is_pointer,
           int is_vararray, int is_array_size,
           const char *var_type_name, int in_out,
           const char *var_name) {
    if (m_work == NULL) {
      m_work = new Function();
    }
    return m_work->AppendArg(arg_code, buffer_bytes, is_pointer,
            is_vararray, is_array_size,
            var_type_name, in_out, var_name);
  }
  void NameFunction(const char *funcname) {
    if (m_work == NULL) {
      m_work = new Function();
    }
    m_work->SetName(funcname);

    AppendFunction(m_work);
    delete m_work;
    m_work = NULL;
  }

  void AppendFunction(Function *pfunc) {
    m_funcs.push_back(*pfunc);
  }

  int CheckAllArraySize(void);

  int MakeStubs(void);
  void Print(int with_args);

 private:
  int MakeHeaderFiles(int server);
  int MakeServerStub(void);
  int MakeClientStub(void);
};
#endif  /* __cplusplus */

#endif  // RPC_LIBRARY_TOOL_APIDEF_H_
