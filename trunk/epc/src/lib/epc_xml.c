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

#define string_defined
#include <epc_xml.h>

/* This structure contains the context while parsing an XML document */
typedef struct {
  struct epc__info *epc__info;
  struct epc__call *epc__call;
  long num_parameters; /* number of in or in/out parameters parsed yet */
} epc__xml_ctx_t;

typedef struct {
  epc__xml_ctx_t epc__xml_ctx;
  xmlsaxcb saxcb;
  xmlctx *xmlctx;
} xml_info_t;

/* SAX callback functions */

static
sword
start_document(void *epc__xml_ctx_ptr);

static
sword
end_document(void *epc__xml_ctx_ptr);

static
sword
start_element(void *epc__xml_ctx_ptr, const oratext *name, 
              const struct xmlnodes *attrs);
static
sword
end_element(void *epc__xml_ctx_ptr, const oratext *name);

static
sword
element_content(void *epc__xml_ctx_ptr, const oratext *ch, size_t len);

static
void
lookup_interface( const char *interface_name, epc__info_t *epc__info, epc__call_t *epc__call );

static
void
lookup_function( const char *function_name, epc__info_t *epc__info, epc__call_t *epc__call );

/* GLOBAL functions */

unsigned int
epc__xml_init( epc__info_t *epc__info )
{
  uword ecode = 0;
  const xmlsaxcb saxcb = {
    start_document,
    end_document,
    start_element,
    end_element,
    element_content
  };
  xml_info_t *xml_info;

  DBUG_ENTER("epc__xml_init");

  epc__info->xml_info = malloc(sizeof(xml_info_t));

  assert ( epc__info->xml_info != NULL );

  xml_info = (xml_info_t *)epc__info->xml_info;

  xml_info->epc__xml_ctx.epc__info = epc__info;
  xml_info->epc__xml_ctx.epc__call = NULL;
  xml_info->epc__xml_ctx.num_parameters = 0L;

  xml_info->saxcb = saxcb;
  
  xml_info->xmlctx = xmlinit(&ecode, (const oratext *) 0,
                             (void (*)(void *, const oratext *, uword)) 0,
                             (void *) 0, &xml_info->saxcb, (void *) &xml_info->epc__xml_ctx,
                             (const xmlmemcb *) 0, NULL,
                             (const oratext *) 0);

  DBUG_PRINT("info", ("xmlctx: %p", xml_info->xmlctx));
  DBUG_LEAVE();

  return ecode;
}

unsigned int
epc__xml_done( epc__info_t *epc__info )
{
  uword ecode = 0;
  xml_info_t *xml_info = (xml_info_t *)epc__info->xml_info;

  DBUG_ENTER("epc__xml_done");
  DBUG_PRINT("info", ("xmlctx: %p", xml_info->xmlctx));

  ecode = xmlterm(xml_info->xmlctx);                        /* terminate XML package */
  free(xml_info);
  epc__info->xml_info = NULL;

  DBUG_LEAVE();

  return ecode;
}

unsigned int
epc__xml_parse( epc__info_t *epc__info, epc__call_t *epc__call, const char *buf, const size_t len )
{
  uword ecode = 0;
  ub4 flags = XML_FLAG_DISCARD_WHITESPACE; /* | XML_FLAG_VALIDATE;*/
  xml_info_t *xml_info = (xml_info_t *)epc__info->xml_info;

  DBUG_ENTER("epc__xml_parse");
  DBUG_PRINT("info", ("xmlctx: %p", xml_info->xmlctx));
  DBUG_PRINT("input", ("buf: %*.*s", len, len, buf));

  xml_info->epc__xml_ctx.epc__call = epc__call;
  ecode = xmlparsebuf(xml_info->xmlctx, (oratext *) buf, len, (oratext *) 0, flags);

  DBUG_PRINT("output", ("ecode: %lu", (unsigned long)ecode));
  DBUG_LEAVE();

  return ecode;
}

/* LOCAL functions */

static
sword
start_document(void *epc__xml_ctx_ptr)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *)epc__xml_ctx_ptr;
  epc__call_t *epc__call = epc__xml_ctx->epc__call;

  epc__call->interface = NULL;
  epc__call->function = NULL;
  epc__call->epc__error = OK;
  epc__xml_ctx->num_parameters = 0;

  dbug_enter(__FILE__, "document", __LINE__, NULL);

  return 0;
}

static
sword
end_document(void *epc__xml_ctx_ptr)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *)epc__xml_ctx_ptr;
  epc__call_t *epc__call = (epc__call_t *)epc__xml_ctx->epc__call;
  int nr;

  dbug_print(__LINE__, "info", "method: %s", (epc__call->function != NULL ? epc__call->function->name : "(null)"));

  if (epc__call->function != NULL)
    {
      for (nr = 0; nr < epc__call->function->num_parameters; nr++)
        {
          dbug_print(__LINE__, "info", "argument %d; name: %s",
                     nr, 
                     epc__call->function->parameters[nr].name);
        }
    }

  dbug_leave(__LINE__, NULL);

  return 0;
}

static
sword
start_element(void *epc__xml_ctx_ptr, const oratext *name, 
              const struct xmlnodes *attrs)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *)epc__xml_ctx_ptr;
  epc__info_t *epc__info = epc__xml_ctx->epc__info;
  epc__call_t *epc__call = epc__xml_ctx->epc__call;
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
      if ( epc__call->function == NULL )
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
                      lookup_interface( interface_name, epc__info, epc__call );
                      lookup_function( function_name, epc__info, epc__call );

                      /* nullify all parameters */

                      if ( epc__call->interface != NULL &&
                           epc__call->function != NULL )
                        {
                          for ( nr = 0; nr < epc__call->function->num_parameters; nr++)
                            switch(epc__call->function->parameters[nr].type)
                              {
                              case C_STRING:
                                *((char*)epc__call->function->parameters[nr].data) = '\0';
                                break;
      
                              case C_INT:
                                *((int*)epc__call->function->parameters[nr].data) = 0;
                                break;

                              case C_LONG:
                                *((long*)epc__call->function->parameters[nr].data) = 0L;
                                break;

                              case C_FLOAT:
                                *((float*)epc__call->function->parameters[nr].data) = 0;
                                break;

                              case C_DOUBLE:
                                *((double*)epc__call->function->parameters[nr].data) = 0;
                                break;
                          
                              case C_VOID:
                                break;

                              default: 
                                assert( epc__call->function->parameters[nr].type >= C_DATATYPE_MIN &&
                                        epc__call->function->parameters[nr].type <= C_DATATYPE_MAX );
                              }
                        }
                    }
                  break;
                  
                case 'A': /* new argument */
                  /* get next in or inout argument */
                  for (nr = epc__xml_ctx->num_parameters; nr < epc__call->function->num_parameters; nr++)
                    {
                      if (epc__call->function->parameters[nr].mode != C_OUT) /* in or in/out */
                        {
                          dbug_print(__LINE__, "info", "parameter[%d]: %s; argument_name: %s",
                                     (int)nr, epc__call->function->parameters[nr].name, argument_name);
                          assert( strcmp(epc__call->function->parameters[nr].name, argument_name) == 0 );
                          break; /* found */
                        }
                    }

                  assert( nr < epc__call->function->num_parameters ); /* should be found */
                  if ( nr >= epc__call->function->num_parameters )
                    {
                      epc__call->epc__error = PARAMETER_UNKNOWN;
                    }
                  else 
                    {
                      epc__xml_ctx->num_parameters = nr + 1; /* next time: search from next parameter */
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

  ecode = epc__call->epc__error == OK ? 0 : 1;

  dbug_print(__LINE__, "info", "epc__error: %d; ecode: %u", (int)epc__call->epc__error, (unsigned)ecode);

  return ecode;
}

static
sword
end_element(void *epc__xml_ctx_ptr, const oratext *name)
{
  dbug_leave(__LINE__, NULL);

  return 0;
}

static
sword
element_content(void *epc__xml_ctx_ptr, const oratext *ch, size_t len)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *)epc__xml_ctx_ptr;
  epc__call_t *epc__call = epc__xml_ctx->epc__call;
  const dword_t nr = epc__xml_ctx->num_parameters - 1;

  assert( epc__call != NULL );
  assert( epc__call->function != NULL );
  assert( nr >= 0 && nr < epc__call->function->num_parameters );
  assert( epc__call->function->parameters[nr].mode != C_OUT); /* in or in/out */

  dbug_print(__LINE__, "info", "element content: %*s", len, (char *)ch);

  switch(epc__call->function->parameters[nr].type)
    {
    case C_STRING:
      assert( len < epc__call->function->parameters[nr].size );
      if ( len >= epc__call->function->parameters[nr].size )
        {
          len = epc__call->function->parameters[nr].size - 1;
        }
      (void) strncpy( (char*)epc__call->function->parameters[nr].data, (char*)ch, len );
      ((char*)epc__call->function->parameters[nr].data)[len] = '\0';
      break;
      
    case C_INT:
      *((int*)epc__call->function->parameters[nr].data) = (int)strtol(ch, NULL, 10);
      break;

    case C_LONG:
      *((long*)epc__call->function->parameters[nr].data) = strtol(ch, NULL, 10);
      break;

    case C_FLOAT:
      *((float*)epc__call->function->parameters[nr].data) = strtof(ch, NULL);
      break;

    case C_DOUBLE:
      *((double*)epc__call->function->parameters[nr].data) = strtod(ch, NULL);
      break;
                          
    case C_VOID: /* impossible */
      assert( epc__call->function->parameters[nr].type != C_VOID );
      break;

    default: 
      assert( epc__call->function->parameters[nr].type >= C_DATATYPE_MIN &&
              epc__call->function->parameters[nr].type <= C_DATATYPE_MAX );
    }

  return 0;
}

static
void
lookup_interface( const char *interface_name, epc__info_t *epc__info, epc__call_t *epc__call )
{
  int inr;
  int result;

  /* get the interface */
  for ( inr = 0, epc__call->interface = NULL; inr < epc__info->num_interfaces; inr++ )
    if ( (result = strcmp( interface_name, epc__info->interfaces[inr]->name )) == 0 )
      {
        epc__call->interface = epc__info->interfaces[inr];
        break;
      }
    else if ( result < 0 ) /* interfaces sorted ascending */
      {
        break;
      }

  if ( epc__call->interface == NULL )
    {
      /* interface not found */
      fprintf( stderr, "ERROR: interface %s not found\n", interface_name );
      epc__call->epc__error = INTERFACE_UNKNOWN;
    }
}

static
void
lookup_function( const char *function_name, epc__info_t *epc__info, epc__call_t *epc__call )
{  
  int fnr;
  int result;

  if ( epc__call->interface == NULL )
    {
      epc__call->function = NULL;      
    }
  else
    {
      /* get the function */
      for ( fnr = 0, epc__call->function = NULL; fnr < epc__call->interface->num_functions; fnr++ ) 
        if ( (result = strcmp( function_name, epc__call->interface->functions[fnr].name )) == 0 ) 
          {
            epc__call->function = &epc__call->interface->functions[fnr];
            break;
          }
        else if ( result < 0 ) /* interface functions sorted ascending */
          {
            break;
          }
    }

  if ( epc__call->function == NULL )
    {
      /* interface not found */
      fprintf( stderr, "ERROR: function '%s' not found\n", function_name );
      epc__call->epc__error = FUNCTION_UNKNOWN;
    }
}
