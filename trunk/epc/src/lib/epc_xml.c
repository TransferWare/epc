#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#include <dbug.h>

#include <oratypes.h>
#include <oraxml.h>

#include "epc_xml.h"
#define string_defined
#include "epc_defs.h"

/* SAX callback functions */

static
sword
start_document(void *epc_xml_ctx_ptr);

static
sword
end_document(void *epc_xml_ctx_ptr);

static
sword
start_element(void *epc_xml_ctx_ptr, const oratext *name, 
              const struct xmlnodes *attrs);
static
sword
end_element(void *epc_xml_ctx_ptr, const oratext *name);

static
sword
element_content(void *epc_xml_ctx_ptr, const oratext *ch, size_t len);

static
void
lookup_interface( const char *interface_name, epc_info_t *epc_info, epc_call_t *epc_call );

static
void
lookup_function( const char *function_name, epc_info_t *epc_info, epc_call_t *epc_call );

/* GLOBAL functions */

unsigned int
epc_xml_init( struct xmlctx **xmlctx, epc_xml_ctx_t *epc_xml_ctx_ptr )
{
  uword ecode = 0;
  /* GJP 15-10-2004 
     The saxcb structure must be globally allocated at least for Oracle 8i.
     Removing the static keyword leads to coredumps on Windows
  */
  static const xmlsaxcb saxcb = {
    start_document,
    end_document,
    start_element,
    end_element,
    element_content
  };

  DBUG_ENTER("epc_xml_init");

  *xmlctx = xmlinit(&ecode, (const oratext *) 0,
                    (void (*)(void *, const oratext *, uword)) 0,
                    (void *) 0, &saxcb, (void *) epc_xml_ctx_ptr,
                    (const xmlmemcb *) 0, NULL,
                    (const oratext *) 0);

  DBUG_PRINT("info", ("xmlctx: %p", *xmlctx));
  DBUG_LEAVE();

  return ecode;
}

unsigned int
epc_xml_done( struct xmlctx **xmlctx )
{
  uword ecode = 0;

  DBUG_ENTER("epc_xml_done");
  DBUG_PRINT("info", ("xmlctx: %p", *xmlctx));

  ecode = xmlterm(*xmlctx);                        /* terminate XML package */
  *xmlctx = NULL;

  DBUG_LEAVE();

  return ecode;
}

unsigned int
epc_xml_parse( struct xmlctx *xmlctx, const char *buf, const size_t len )
{
  uword ecode = 0;
  ub4 flags = XML_FLAG_DISCARD_WHITESPACE; /* | XML_FLAG_VALIDATE;*/

  DBUG_ENTER("epc_xml_parse");
  DBUG_PRINT("info", ("xmlctx: %p", xmlctx));
  DBUG_PRINT("input", ("buf: %*.*s", len, len, buf));

  ecode = xmlparsebuf(xmlctx, (oratext *) buf, len, (oratext *) 0, flags);

  DBUG_PRINT("output", ("ecode: %lu", (unsigned long)ecode));
  DBUG_LEAVE();

  return ecode;
}

/* LOCAL functions */

static
sword
start_document(void *epc_xml_ctx_ptr)
{
  epc_xml_ctx_t *epc_xml_ctx = (epc_xml_ctx_t *)epc_xml_ctx_ptr;
  epc_call_t *epc_call = epc_xml_ctx->epc_call;

  epc_call->interface = NULL;
  epc_call->function = NULL;
  epc_call->epc_error = OK;

  dbug_enter(__FILE__, "document", __LINE__, NULL);

  return 0;
}

static
sword
end_document(void *epc_xml_ctx_ptr)
{
  epc_xml_ctx_t *epc_xml_ctx = (epc_xml_ctx_t *)epc_xml_ctx_ptr;
  epc_call_t *epc_call = (epc_call_t *)epc_xml_ctx->epc_call;
  int nr;

  dbug_print(__LINE__, "info", "method: %s", (epc_call->function != NULL ? epc_call->function->name : "(null)"));

  if (epc_call->function != NULL)
    {
      for (nr = 0; nr < epc_call->function->num_parameters; nr++)
        {
          dbug_print(__LINE__, "info", "argument %d; name: %s",
                     nr, 
                     epc_call->function->parameters[nr].name);
        }
    }

  dbug_leave(__LINE__, NULL);

  return 0;
}

static
sword
start_element(void *epc_xml_ctx_ptr, const oratext *name, 
              const struct xmlnodes *attrs)
{
  epc_xml_ctx_t *epc_xml_ctx = (epc_xml_ctx_t *)epc_xml_ctx_ptr;
  epc_info_t *epc_info = epc_xml_ctx->epc_info;
  epc_call_t *epc_call = epc_xml_ctx->epc_call;
  char element_type = 0; /* (S)oap, (M)ethod, (A)rgument */
  const size_t len = strlen((const char*)name);
  char *interface_name = NULL, *function_name = NULL, *argument_name = NULL;
  uword ecode;

  dbug_enter(__FILE__, (char*)name, __LINE__, NULL);

  if ( strncmp((const char *)name, "SOAP", 4) == 0 ||
       strncmp((const char *)name, "soap", 4) == 0 )
    {
      dbug_print(__LINE__, "info", "skipping SOAP element");
      element_type = 'S';
    }
  else 
    {
      if ( epc_call->function == NULL )
        {
          dbug_print(__LINE__, "info", "method: %s", (char*)name);

          function_name = (char *)name;
          element_type = 'M';
        }
      else
        {
          dbug_print(__LINE__, "info", "argument: %s", (char*)name);

          argument_name = (char *)name;
          element_type = 'A';
        }

      if ( attrs != NULL ) 
        {
          size_t idx;
          xmlnode *attr;
          dword_t nr;

          for ( idx = 0; idx < numAttributes(attrs); idx++ )
            {
              attr = getAttributeIndex(attrs, idx);

              switch(element_type)
                {
                case 'M': /* new method */
                  /* namespace is the interface name */
                  if ( strcmp((char*)getAttrName(attr), "xmlns") == 0 )
                    {
                      interface_name = (char*)getAttrValue(attr);
                      lookup_interface( interface_name, epc_info, epc_call );
                      lookup_function( function_name, epc_info, epc_call );

                      /* nullify all parameters */

                      if ( epc_call->interface != NULL &&
                           epc_call->function != NULL )
                        {
                          for ( nr = 0; nr < epc_call->function->num_parameters; nr++)
                            switch(epc_call->function->parameters[nr].type)
                              {
                              case C_STRING:
                                *((char*)epc_call->function->parameters[nr].data) = '\0';
                                break;
      
                              case C_INT:
                                *((int*)epc_call->function->parameters[nr].data) = 0;
                                break;

                              case C_LONG:
                                *((long*)epc_call->function->parameters[nr].data) = 0L;
                                break;

                              case C_FLOAT:
                                *((float*)epc_call->function->parameters[nr].data) = 0;
                                break;

                              case C_DOUBLE:
                                *((double*)epc_call->function->parameters[nr].data) = 0;
                                break;
                          
                              case C_VOID: /* impossible */
                                assert( epc_call->function->parameters[nr].type != C_VOID );
                                break;

                              default: 
                                assert( epc_call->function->parameters[nr].type >= C_DATATYPE_MIN &&
                                        epc_call->function->parameters[nr].type <= C_DATATYPE_MAX );
                              }
                        }
                    }
                  break;
                  
                case 'A': /* new argument */
                  /* get next in or inout argument */
                  for (nr = epc_xml_ctx->num_parameters; nr < epc_call->function->num_parameters; nr++)
                    {
                      if (epc_call->function->parameters[nr].mode != C_OUT) /* in or in/out */
                        {
                          dbug_print(__LINE__, "info", "parameter[%d]: %s; argument_name: %s",
                                     (int)nr, epc_call->function->parameters[nr].name, argument_name);
                          assert( strcmp(epc_call->function->parameters[nr].name, argument_name) == 0 );
                          break; /* found */
                        }
                    }

                  assert( nr < epc_call->function->num_parameters ); /* should be found */
                  if ( nr >= epc_call->function->num_parameters )
                    {
                      epc_call->epc_error = PARAMETER_UNKNOWN;
                    }
                  else 
                    {
                      epc_xml_ctx->num_parameters = nr + 1; /* next time: search from next parameter */
                    }
                  break;

                case 'S': /* SOAP element */
                default:
                  break;
                }
              dbug_print(__LINE__, "info", "attribute %s: %s", (char*)getAttrName(attr), (char*)getAttrValue(attr));
            }
        }
    }

  ecode = epc_call->epc_error == OK ? 0 : 1;

  dbug_print(__LINE__, "info", "epc_error: %d; ecode: %u", (int)epc_call->epc_error, (unsigned)ecode);

  return ecode;
}

static
sword
end_element(void *epc_xml_ctx_ptr, const oratext *name)
{
  dbug_leave(__LINE__, NULL);

  return 0;
}

static
sword
element_content(void *epc_xml_ctx_ptr, const oratext *ch, size_t len)
{
  epc_xml_ctx_t *epc_xml_ctx = (epc_xml_ctx_t *)epc_xml_ctx_ptr;
  epc_call_t *epc_call = epc_xml_ctx->epc_call;
  const dword_t nr = epc_xml_ctx->num_parameters - 1;

  assert( epc_call != NULL );
  assert( epc_call->function != NULL );
  assert( nr >= 0 && nr < epc_call->function->num_parameters );
  assert( epc_call->function->parameters[nr].mode != C_OUT); /* in or in/out */

  dbug_print(__LINE__, "info", "element content: %*s", len, (char *)ch);

  switch(epc_call->function->parameters[nr].type)
    {
    case C_STRING:
      assert( len <= epc_call->function->parameters[nr].size );
      if ( len > epc_call->function->parameters[nr].size )
        {
          len = epc_call->function->parameters[nr].size;
        }
      (void) strncpy( (char*)epc_call->function->parameters[nr].data, (char*)ch, len );
      ((char*)epc_call->function->parameters[nr].data)[len] = '\0';
      break;
      
    case C_INT:
      *((int*)epc_call->function->parameters[nr].data) = (int)strtol(ch, NULL, 10);
      break;

    case C_LONG:
      *((long*)epc_call->function->parameters[nr].data) = strtol(ch, NULL, 10);
      break;

    case C_FLOAT:
      *((float*)epc_call->function->parameters[nr].data) = strtof(ch, NULL);
      break;

    case C_DOUBLE:
      *((double*)epc_call->function->parameters[nr].data) = strtod(ch, NULL);
      break;
                          
    case C_VOID: /* impossible */
      assert( epc_call->function->parameters[nr].type != C_VOID );
      break;

    default: 
      assert( epc_call->function->parameters[nr].type >= C_DATATYPE_MIN &&
              epc_call->function->parameters[nr].type <= C_DATATYPE_MAX );
    }

  return 0;
}

static
void
lookup_interface( const char *interface_name, epc_info_t *epc_info, epc_call_t *epc_call )
{
  int inr;
  int result;

  /* get the interface */
  for ( inr = 0, epc_call->interface = NULL; inr < epc_info->num_interfaces; inr++ )
    if ( (result = strcmp( interface_name, epc_info->interfaces[inr]->name )) == 0 )
      {
        epc_call->interface = epc_info->interfaces[inr];
        break;
      }
    else if ( result < 0 ) /* interfaces sorted ascending */
      {
        break;
      }

  if ( epc_call->interface == NULL )
    {
      /* interface not found */
      fprintf( stderr, "ERROR: interface %s not found\n", interface_name );
      epc_call->epc_error = INTERFACE_UNKNOWN;
    }
}

static
void
lookup_function( const char *function_name, epc_info_t *epc_info, epc_call_t *epc_call )
{  
  int fnr;
  int result;

  if ( epc_call->interface == NULL )
    {
      epc_call->function = NULL;      
    }
  else
    {
      /* get the function */
      for ( fnr = 0, epc_call->function = NULL; fnr < epc_call->interface->num_functions; fnr++ ) 
        if ( (result = strcmp( function_name, epc_call->interface->functions[fnr].name )) == 0 ) 
          {
            epc_call->function = &epc_call->interface->functions[fnr];
            break;
          }
        else if ( result < 0 ) /* interface functions sorted ascending */
          {
            break;
          }
    }

  if ( epc_call->function == NULL )
    {
      /* interface not found */
      fprintf( stderr, "ERROR: function '%s' not found\n", function_name );
      epc_call->epc_error = FUNCTION_UNKNOWN;
    }
}
