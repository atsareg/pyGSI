/*
 * ssl.h
 *
 * Copyright (C) AB Strakt 2001, All rights reserved
 *
 * Export functions and exceptions from the SSL sub module.
 * See the file RATIONALE for a short explanation of why this module was written.
 *
 * Reviewed 2001-07-23
 *
 * @(#) $Id: ssl.h,v 1.1 2008/02/29 18:46:04 acasajus Exp $
 */
#ifndef PyOpenSSL_SSL_H_
#define PyOpenSSL_SSL_H_

#include <Python.h>
#include "gsi.h"
#include "thread_safe.h"
#include "context.h"
#include "connection.h"
#include "session.h"
#include "../util.h"
#include "../crypto/crypto.h"

extern PyObject *ssl_Error,		/* Base class */
*ssl_ZeroReturnError,			/* Used with SSL_get_erorr */
*ssl_WantReadError,				/* ...  */
*ssl_WantWriteError,			/* ...  */
*ssl_WantX509LookupError,		/* ...  */
*ssl_SysCallError;				/* Uses (errno,errstr) */

#ifdef exception_from_error_queue
#  undef exception_from_error_queue
#endif
#define exception_from_error_queue()    do { \
    PyObject *errlist = error_queue_to_list(); \
    PyErr_SetObject(ssl_Error, errlist); \
    Py_DECREF(errlist); \
} while (0)

#define ssl_Context_New_NUM       0
#define ssl_Context_New_RETURN    ssl_ContextObj *
#define ssl_Context_New_PROTO     (int method)

#define ssl_Connection_New_NUM    1
#define ssl_Connection_New_RETURN ssl_ConnectionObj *
#define ssl_Connection_New_PROTO  (ssl_ContextObj *ctx, PyObject *sock)

#define ssl_Session_New_NUM       2
#define ssl_Session_New_RETURN    ssl_SessionObj *
#define ssl_Session_New_PROTO     (void)

#define ssl_API_pointers          3

#ifdef SSL_MODULE

extern ssl_Context_New_RETURN ssl_Context_New ssl_Context_New_PROTO;
extern ssl_Connection_New_RETURN ssl_Connection_New
	ssl_Connection_New_PROTO;
extern ssl_Session_New_RETURN ssl_Session_New ssl_Session_New_PROTO;

#else							/* SSL_MODULE */

extern void **ssl_API;

#define ssl_Context_New \
 (*(ssl_Context_New_RETURN (*)ssl_Context_New_PROTO) ssl_API[ssl_Context_New_NUM])
#define ssl_Connection_New \
 (*(ssl_Connection_New_RETURN (*)ssl_Connection_New_PROTO) ssl_API[ssl_Connection_New_NUM])
#define ssl_Session_New \
 (*(ssl_Session_New_RETURN (*)ssl_Session_New_PROTO) ssl_API[ssl_Session_New_NUM])

#define import_SSL() \
{ \
  PyObject *module = PyImport_ImportModule("OpenSSL.SSL"); \
  if (module != NULL) { \
    PyObject *module_dict = PyModule_GetDict(module); \
    PyObject *c_api_object = PyDict_GetItemString(module_dict, "_C_API"); \
    if (PyCObject_Check(c_api_object)) { \
      ssl_API = (void **)PyCObject_AsVoidPtr(c_api_object); \
    } \
  } \
}

#endif							/* SSL_MODULE */

#endif							/* PyOpenSSL_SSL_H_ */