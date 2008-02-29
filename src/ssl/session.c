/*
 * connection.c
 *
 * Copyright (C) AB Strakt 2001, All rights reserved
 *
 * SSL Connection objects and methods.
 * See the file RATIONALE for a short explanation of why this module was written.
 *
 * Reviewed 2001-07-23
 */
#include <Python.h>
#define SSL_MODULE
#include <openssl/err.h>
#include "ssl.h"


static char ssl_Session_free_doc[] = "\n\
Free the session.\n\
\n\
Arguments: self - The Connection object\n\
           args - The Python argument tuple, should be empty\n\
Returns:   None.\n\
";
static PyObject *ssl_Session_free( ssl_SessionObj * self, PyObject * args )
{
	if ( !PyArg_ParseTuple( args, ":free" ) )
		return NULL;

	if ( self->session )
	{
		SSL_SESSION_free( self->session );
		self->session = NULL;
	}

	Py_INCREF( Py_None );
	return Py_None;
}

static char ssl_Session_valid_doc[] = "\n\
Check wether the session is a valid one.\n\
\n\
Arguments: self - The Connection object\n\
           args - The Python argument tuple, should be empty\n\
Returns:   True if its a valid session.\n\
";
static PyObject *ssl_Session_valid( ssl_SessionObj * self,
									PyObject * args )
{
	int ret = 0;

	if ( !PyArg_ParseTuple( args, ":valid" ) )
		return NULL;

	if ( self->session )
		ret = 1;

	return PyInt_FromLong( ( long ) ret );
}

#define ADD_METHOD(name)        \
    { #name, (PyCFunction)ssl_Session_##name, METH_VARARGS, ssl_Session_##name##_doc }

/*
 * Member methods in the Connection object
 * ADD_METHOD(name) expands to a correct PyMethodDef declaration
 *   {  'name', (PyCFunction)ssl_Connection_name, METH_VARARGS }
 * for convenience
 * ADD_ALIAS(name,real) creates an "alias" of the ssl_Connection_real
 * function with the name 'name'
 */
static PyMethodDef ssl_Session_methods[] = {
	ADD_METHOD( free ),
	ADD_METHOD( valid ),
	{NULL, NULL}
};


/*
 * Constructor for Session objects
 *
 * Arguments: None
 * Returns:   The newly created Session object
 */
ssl_SessionObj *ssl_Session_New(  )
{
	ssl_SessionObj *self;

	self = PyObject_GC_New( ssl_SessionObj, &ssl_Session_Type );
	if ( self == NULL )
		return NULL;

	self->session = NULL;

	Py_INCREF( Py_None );
	self->app_data = Py_None;

	self->tstate = NULL;

	PyObject_GC_Track( self );

	return self;
}

/*
 * Find attribute
 *
 * Arguments: self - The Connection object
 *            name - The attribute name
 * Returns:   A Python object for the attribute, or NULL if something went
 *            wrong
 */
static PyObject *ssl_Session_getattr( ssl_SessionObj * self, char *name )
{
	PyObject *meth;

	meth = Py_FindMethod( ssl_Session_methods, ( PyObject * ) self, name );

	return meth;
}



/*
 * Decref all contained objects and zero the pointers.
 *
 * Arguments: self - The Session object
 * Returns:   Always 0.
 */
static int ssl_Session_clear( ssl_SessionObj * self )
{
	Py_CLEAR( self->app_data );
	return 0;
}

/*
 * Deallocate the memory used by the Session object
 *
 * Arguments: self - The Sonnection object
 * Returns:   None
 */
static void ssl_Session_dealloc( ssl_SessionObj * self )
{
	PyObject_GC_UnTrack( self );
	ssl_Session_clear( self );
	if ( self->session )
	{
		SSL_SESSION_free( self->session );
		self->session = NULL;
	}
	PyObject_GC_Del( self );
}


/*
 * Call the visitproc on all contained objects.
 *
 * Arguments: self - The Connection object
 *            visit - Function to call
 *            arg - Extra argument to visit
 * Returns:   0 if all goes well, otherwise the return code from the first
 *            call that gave non-zero result.
 */
static int
ssl_Session_traverse( ssl_SessionObj * self, visitproc visit, void *arg )
{
	Py_VISIT( self->app_data );
	return 0;
}

PyTypeObject ssl_Session_Type = {
	PyObject_HEAD_INIT( NULL ) 0,
	"Session",
	sizeof( ssl_SessionObj ),
	0,
	( destructor ) ssl_Session_dealloc,
	NULL,						/* print */
	( getattrfunc ) ssl_Session_getattr,
	NULL,						/* setattr */
	NULL,						/* compare */
	NULL,						/* repr */
	NULL,						/* as_number */
	NULL,						/* as_sequence */
	NULL,						/* as_mapping */
	NULL,						/* hash */
	NULL,						/* call */
	NULL,						/* str */
	NULL,						/* getattro */
	NULL,						/* setattro */
	NULL,						/* as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,
	NULL,						/* doc */
	( traverseproc ) ssl_Session_traverse,
	( inquiry ) ssl_Session_clear,
};


/*
 * Initiailze the Session part of the SSL sub module
 *
 * Arguments: dict - Dictionary of the OpenSSL.SSL module
 * Returns:   1 for success, 0 otherwise
 */
int init_ssl_session( PyObject * dict )
{
	ssl_Session_Type.ob_type = &PyType_Type;
	Py_INCREF( &ssl_Session_Type );
	if ( PyDict_SetItemString
		 ( dict, "SessionType", ( PyObject * ) & ssl_Session_Type ) != 0 )
		return 0;

	return 1;
}