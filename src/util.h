/*
 * util.h
 *
 * Copyright (C) AB Strakt 2001, All rights reserved
 *
 * Export utility functions and macros.
 * See the file RATIONALE for a short explanation of why this module was written.
 *
 * Reviewed 2001-07-23
 *
 * @(#) $Id: util.h,v 1.1 2008/02/29 18:46:03 acasajus Exp $
 */
#ifndef PyOpenSSL_UTIL_H_
#define PyOpenSSL_UTIL_H_

#include <Python.h>
#include <time.h>
#include <openssl/err.h>

/*
 * pymemcompat written by Michael Hudson and lets you program to the
 * Python 2.3 memory API while keeping backwards compatability.
 */
#include "pymemcompat.h"

extern  PyObject *error_queue_to_list(void);
extern  void      flush_error_queue(void);

/*
 * These are needed because there is no "official" way to specify
 * WHERE to save the thread state.
 */

#ifdef WITH_THREAD
#  define MY_BEGIN_ALLOW_THREADS(st)    							\
    st = PyEval_SaveThread();
#  define MY_BEGIN_ALLOW_THREADS_RETURN(st, ret)    				\
    st = PyEval_SaveThread(); 									\
	return ret
#  define MY_END_ALLOW_THREADS(st)      							\
    if(st) 															\
	{ 																\
		PyEval_RestoreThread(st); 									\
		st = NULL; 													\
	} else assert("Thread State not set");
#  define MY_END_ALLOW_THREADS_RETURN(st,ret)      					\
    MY_END_ALLOW_THREADS( st );										\
	return ret
#else
#  define MY_BEGIN_ALLOW_THREADS(st)
#  define MY_BEGIN_ALLOW_THREADS_RETURN(st,ret)						\
		return ret
#  define MY_END_ALLOW_THREADS(st)      { st = NULL; }
#  define MY_END_ALLOW_THREADS_RETURN(st,ret)      { st = NULL; }   \
		return ret
#endif

#if !defined(PY_MAJOR_VERSION) || PY_VERSION_HEX < 0x02000000
static int
PyModule_AddObject(PyObject *m, char *name, PyObject *o)
{
    PyObject *dict;
    if (!PyModule_Check(m) || o == NULL)
        return -1;
    dict = PyModule_GetDict(m);
    if (dict == NULL)
        return -1;
    if (PyDict_SetItemString(dict, name, o))
        return -1;
    Py_DECREF(o);
    return 0;
}

static int
PyModule_AddIntConstant(PyObject *m, char *name, long value)
{
    return PyModule_AddObject(m, name, PyInt_FromLong(value));
}

static int PyObject_AsFileDescriptor(PyObject *o)
{
    int fd;
    PyObject *meth;

    if (PyInt_Check(o)) {
        fd = PyInt_AsLong(o);
    }
    else if (PyLong_Check(o)) {
        fd = PyLong_AsLong(o);
    }
    else if ((meth = PyObject_GetAttrString(o, "fileno")) != NULL)
    {
        PyObject *fno = PyEval_CallObject(meth, NULL);
        Py_DECREF(meth);
        if (fno == NULL)
            return -1;

        if (PyInt_Check(fno)) {
            fd = PyInt_AsLong(fno);
            Py_DECREF(fno);
        }
        else if (PyLong_Check(fno)) {
            fd = PyLong_AsLong(fno);
            Py_DECREF(fno);
        }
        else {
            PyErr_SetString(PyExc_TypeError, "fileno() returned a non-integer");
            Py_DECREF(fno);
            return -1;
        }
    }
    else {
        PyErr_SetString(PyExc_TypeError, "argument must be an int, or have a fileno() method.");
        return -1;
    }

    if (fd < 0) {
        PyErr_Format(PyExc_ValueError, "file descriptor cannot be a negative integer (%i)", fd);
        return -1;
    }
    return fd;
}
#endif

time_t GRSTasn1TimeToTimeT(char *asn1time, size_t len);

#endif