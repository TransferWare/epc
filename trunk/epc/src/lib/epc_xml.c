#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dbug.h>

#include "epc_xml.h"

/* SAX callback functions */

static
sword
start_document(void *ctx);

static
sword
end_document(void *ctx);

static
sword
start_element(void *ctx, const oratext *name, 
              const struct xmlnodes *attrs);
static
sword
end_element(void *ctx, const oratext *name);

static
sword
element_content(void *ctx, const oratext *ch, size_t len);

static
void
lookup_interface( const char *interface_name, epc_info_t *epc_info, epc_call_t *epc_call );

static
void
lookup_function( const char *function_name, epc_info_t *epc_info, epc_call_t *epc_call );

unsigned int
epc_xml_init( struct xmlctx **xmlctx, void *ctx )
{
  uword ecode = 0;
  const xmlsaxcb saxcb = {
    start_document,
    end_document,
    start_element,
    end_element,
    element_content
  };

  DBUG_ENTER("epc_xml_init");

  *xmlctx = xmlinit(&ecode, (const oratext *) 0,
                    (void (*)(void *, const oratext *, uword)) 0,
                    (void *) 0, &saxcb, (void *) 0,
                    (const xmlmemcb *) 0, ctx,
                    (const oratext *) 0);

  DBUG_LEAVE();

  return ecode;
}

unsigned int
epc_xml_done( struct xmlctx **xmlctx )
{
  uword ecode = 0;

  DBUG_ENTER("epc_xml_done");

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

  ecode = xmlparsebuf(xmlctx, (oratext *) buf, len, (oratext *) 0, flags);

  DBUG_LEAVE();

  return ecode;
}

static
sword
start_document(void *ctx)
{
  dbug_enter(__FILE__, "document", __LINE__, NULL);

  return 0;
}

static
sword
end_document(void *ctx)
{
  dbug_leave(__LINE__, NULL);

  return 0;
}

static
sword
start_element(void *ctx, const oratext *name, 
             const struct xmlnodes *attrs)
{
  oratext *elem = (oratext *) name;
  size_t len = strlen((const char*)name);

  dbug_enter(__FILE__, elem, __LINE__, NULL);

  if ( len >= 4 && strncmp((const char *)&elem[len - 4], "Body", 4) == 0 )
    {
      /* Body */
      if ( attrs != NULL ) {
        size_t idx;
        xmlnode *attr;

        for ( idx = 0; idx < numAttributes(attrs); idx++ )
          {
            attr = getAttributeIndex(attrs, idx);
            dbug_print(__LINE__, "info", "attribute %s: %s", (char*)getAttrName(attr), (char*)getAttrValue(attr));
          }
      }
    }

  return 0;
}

static
sword
end_element(void *ctx, const oratext *name)
{
  dbug_leave(__LINE__, NULL);

  return 0;
}

static
sword
element_content(void *ctx, const oratext *ch, size_t len)
{
  dbug_print(__LINE__, "info", "%*s", len, (char *)ch);

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
      fprintf( stderr, "ERROR: function %s not found\n", function_name );
      epc_call->epc_error = FUNCTION_UNKNOWN;
    }
}
