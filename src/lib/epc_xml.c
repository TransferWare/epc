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
typedef struct
{
  struct epc__info *epc__info;
  struct epc__call *epc__call;
  long num_parameters;          /* number of in or in/out parameters parsed yet */
} epc__xml_ctx_t;

typedef struct
{
  epc__xml_ctx_t epc__xml_ctx;
  xmlsaxcb saxcb;
  xmlctx *xmlctx;
} xml_info_t;

/* SAX callback functions */

static
  void
error_handler (void *epc__xml_ctx_ptr, const oratext * msg, uword errcode);

static sword start_document (void *epc__xml_ctx_ptr);

static sword end_document (void *epc__xml_ctx_ptr);

static
  sword
start_element (void *epc__xml_ctx_ptr, 
               const oratext *qname,
               const oratext *name,
               const oratext *namespace,
               const struct xmlnodes *attrs);

static sword end_element (void *epc__xml_ctx_ptr, const oratext * name);

static
  sword
element_content (void *epc__xml_ctx_ptr, const oratext * ch, size_t len);

static
  void
lookup_interface (const char *interface_name, epc__info_t * epc__info,
                  epc__call_t * epc__call);

static
  void
lookup_function (const char *function_name, epc__info_t * epc__info,
                 epc__call_t * epc__call);

/* GLOBAL functions */

/**
 * @brief Initialize the SAX XML parser.
 * 
 * Set-up the SAX XML parser. The callback information is stored in the heap
 * in order to prevent a stack dump.
 *
 * @param epc__info  Epc run-time information.
 *
 * @return error code returned by xmlinit
 *
 ******************************************************************************/
unsigned int
epc__xml_init (epc__info_t * epc__info)
{
  uword ecode = 0;
  const xmlsaxcb saxcb = {
    start_document,
    end_document,
    NULL,
    end_element,
    element_content,
    NULL,
    NULL,
    NULL,
    NULL,
    start_element,
  };
  xml_info_t *xml_info;

  DBUG_ENTER ("epc__xml_init");

  epc__info->xml_info = malloc (sizeof (xml_info_t));

  assert (epc__info->xml_info != NULL);

  xml_info = (xml_info_t *) epc__info->xml_info;

  xml_info->epc__xml_ctx.epc__info = epc__info;
  xml_info->epc__xml_ctx.epc__call = NULL;
  xml_info->epc__xml_ctx.num_parameters = 0L;

  xml_info->saxcb = saxcb;

  xml_info->xmlctx = xmlinit (&ecode,
                              NULL,
                              error_handler,
                              &xml_info->epc__xml_ctx,
                              &xml_info->saxcb,
                              &xml_info->epc__xml_ctx, NULL, NULL, NULL);

  DBUG_PRINT ("info", ("xmlctx: %p", xml_info->xmlctx));
  DBUG_LEAVE ();

  return ecode;
}

/**
 * @brief Terminate the SAX XML parser.
 * 
 * Terminates the SAX XML parser. The callback information is freed.
 *
 * @param epc__info  Epc run-time information.
 *
 * @return error code returned by xmlterm
 *
 ******************************************************************************/
unsigned int
epc__xml_done (epc__info_t * epc__info)
{
  uword ecode = XMLERR_OK;
  xml_info_t *xml_info = (xml_info_t *) epc__info->xml_info;

  DBUG_ENTER ("epc__xml_done");
  DBUG_PRINT ("info", ("xmlctx: %p", xml_info->xmlctx));

  ecode = xmlterm (xml_info->xmlctx);   /* terminate XML package */
  free (xml_info);
  epc__info->xml_info = NULL;

  DBUG_LEAVE ();

  return ecode;
}

/**
 * @brief Parse an XML buffer.
 * 
 * @param epc__info  Epc run-time information.
 * @param epc__call  Epc call information.
 * @param buf        XML buffer.
 * @param len        XML buffer length (excluding terminating zero).
 *
 * @return error code returned by xmlparsebuf
 *
 ******************************************************************************/
unsigned int
epc__xml_parse (epc__info_t * epc__info, epc__call_t * epc__call,
                const char *buf, const size_t len)
{
  uword ecode = XMLERR_OK;
  ub4 flags = XML_FLAG_DISCARD_WHITESPACE;      /* | XML_FLAG_VALIDATE; */
  xml_info_t *xml_info = (xml_info_t *) epc__info->xml_info;

  DBUG_ENTER ("epc__xml_parse");
  DBUG_PRINT ("info", ("xmlctx: %p", xml_info->xmlctx));
  DBUG_PRINT ("input", ("buf: %*.*s", len, len, buf));

  xml_info->epc__xml_ctx.epc__call = epc__call;
  ecode = xmlparsebuf (xml_info->xmlctx, (oratext *) buf, len, NULL, flags);
  xmlclean (xml_info->xmlctx);

  DBUG_PRINT ("output", ("ecode: %lu", (unsigned long) ecode));
  DBUG_LEAVE ();

  return ecode;
}

/* LOCAL functions */

/**
 * @brief Translates error into a SOAP Fault element.
 *
 * This is an error handler callback specified during xmlinit.
 * 
 * Error codes:
 *
 * <table summary="error codes">
 *   <tr><th>Range</th><th>Description</th></tr>
 *   <tr><td>0000 - 0099</td><td>Generic</td></tr>
 *   <tr><td>0100 - 0199</td><td>VC and other Warnings</td></tr>
 *   <tr><td>0200 - 0299</td><td>Parser</td></tr>
 *   <tr><td>0300 - 0399</td><td>XSL</td></tr>
 *   <tr><td>0400 - 0499</td><td>XPATH</td></tr>
 * </table>
 *
 * Excerpt from http://www.w3schools.com/soap/soap_fault.asp:
 *
 * The SOAP Fault Element
 *
 * An error message from a SOAP message is carried inside a Fault element.
 *
 * If a Fault element is present, it must appear as a child element of
 * the Body element. A Fault element can only appear once in a SOAP
 * message.
 *
 * The SOAP Fault element has the following sub elements: 
 * <dl>
 *   <dt>&lt;faultcode&gt;</dt><dd>A code for identifying the fault</dd>
 *   <dt>&lt;faultstring&gt;</dt><dd>A human readable explanation of the fault</dd>
 *   <dt>&lt;faultactor&gt;</dt><dd>Information about who caused the fault to happen</dd>
 *   <dt>&lt;detail&gt;</dt><dd>Holds application specific error information related to the Body element</dd>
 * </dl>
 *
 * SOAP Fault Codes
 *
 * The faultcode values defined below must be used in the faultcode element when describing faults: 
 * <dl>
 *   <dt>VersionMismatch</dt>
 *   <dd>Found an invalid namespace for the SOAP Envelope element</dd>
 *   <dt>MustUnderstand</dt>
 *   <dd>An immediate child element of the Header element, with the mustUnderstand attribute set to "1", was not understood</dd>
 *   <dt>Client</dt>
 *   <dd>The message was incorrectly formed or contained incorrect information</dd>
 *   <dt>Server</dt>
 *   <dd>There was a problem with the server so the message could not proceed</dd>
 * </dl>
 *
 * Error codes between 0 and 99 are converted into a Server faultcode.
 * Other error codes are converted into a Client faultcode.
 *
 * The fields set are:
 * <ul>
 * <li>((epc__xml_ctx_t *) epc__xml_ctx_ptr)->epc__call->epc__error</li>
 * <li>((epc__xml_ctx_t *) epc__xml_ctx_ptr)->epc__call->msg_response</li>
 * </ul>
 *
 * @param epc__xml_ctx_ptr  Callback data.
 * @param msg               Error message.
 * @param errcode           Error code.
 *
 ******************************************************************************/
static void
error_handler (void *epc__xml_ctx_ptr, const oratext * msg, uword errcode)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *) epc__xml_ctx_ptr;
  epc__call_t *epc__call = epc__xml_ctx->epc__call;

  epc__call->epc__error = PARSE_ERROR;

  if (errcode >= 0 && errcode <= 99)
    {
      /* server side error */
      (void) snprintf (epc__call->msg_response,
                       MAX_MSG_RESPONSE_LEN + 1,
                       SOAP_HEADER_START "<SOAP-ENV:Fault>\
<faultcode>Server</faultcode><faultstring>%s</faultstring>\
</SOAP-ENV:Fault>" SOAP_HEADER_END, (char *) msg);
    }
  else
    {
      /* client side error */
      (void) snprintf (epc__call->msg_response,
                       MAX_MSG_RESPONSE_LEN + 1,
                       SOAP_HEADER_START "<SOAP-ENV:Fault>\
<faultcode>Client</faultcode><faultstring>%s</faultstring>\
</SOAP-ENV:Fault>" SOAP_HEADER_END, (char *) msg);
    }
}


/**
 * @brief Start parsing a document.
 *
 * This is a callback called at the start of parsing a document.
 *
 * The fields initialized are:
 * <ul>
 * <li>((epc__xml_ctx_t *) epc__xml_ctx_ptr)->epc__call->interface</li>
 * <li>((epc__xml_ctx_t *) epc__xml_ctx_ptr)->epc__call->function</li>
 * <li>((epc__xml_ctx_t *) epc__xml_ctx_ptr)->epc__call->epc__error</li>
 * <li>((epc__xml_ctx_t *) epc__xml_ctx_ptr)->num_parameter</li>
 * </ul>
 *
 * @param epc__xml_ctx_ptr  Callback data.
 *
 * @return XMLERR_OK
 *
 ******************************************************************************/
static sword
start_document (void *epc__xml_ctx_ptr)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *) epc__xml_ctx_ptr;
  epc__call_t *epc__call = epc__xml_ctx->epc__call;

  epc__call->interface = NULL;
  epc__call->function = NULL;
  epc__call->epc__error = OK;
  epc__xml_ctx->num_parameters = 0;

  dbug_enter (__FILE__, "document", __LINE__, NULL);

  return XMLERR_OK;
}

/**
 * @brief Stop parsing a document.
 *
 * This is a callback called at the end of parsing a document. 
 * Used for debugging purposes only.
 *
 * @param epc__xml_ctx_ptr  Callback data.
 *
 * @return XMLERR_OK
 *
 ******************************************************************************/
static sword
end_document (void *epc__xml_ctx_ptr)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *) epc__xml_ctx_ptr;
  epc__call_t *epc__call = (epc__call_t *) epc__xml_ctx->epc__call;
  int nr;

  dbug_print (__LINE__, "info", "method: %s",
              (epc__call->function !=
               NULL ? epc__call->function->name : "(null)"));

  if (epc__call->function != NULL)
    {
      for (nr = 0; nr < epc__call->function->num_parameters; nr++)
        {
          dbug_print (__LINE__, "info", "argument %d; name: %s",
                      nr, epc__call->function->parameters[nr].name);
        }
    }

  dbug_leave (__LINE__, NULL);

  return XMLERR_OK;
}

/**
 * @brief Start parsing an element.
 *
 * This is a callback called at the start of parsing an element.
 * <ul>
 * <li>When the request element is parsed the function name (parameter name) and
 * interface (parameter namespace) are set. The interface and function are looked
 * up the list of EPC interfaces. When found, the attributes are initialised.</li>
 * <li>When an argument element is parsed (function found), it is looked up in the 
 * the list of arguments (epc__xml_ctx->num_parameters is set).</li>
 * </ul>
 *
 * @param epc__xml_ctx_ptr  Callback data.
 * @param qname             Fully qualified name of the element (SOAP-ENV:Body).
 * @param name              Local name of the element (Body).
 * @param namespace         Namespace of the element (http://schemas.xmlsoap.org/soap/envelope/).
 * @param attrs             Attributes.
 *
 * @return XMLERR_OK
 *
 ******************************************************************************/
static sword
start_element (void *epc__xml_ctx_ptr, 
               const oratext *qname,
               const oratext *name,
               const oratext *namespace,
               /*@unused@*/ const struct xmlnodes *attrs)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *) epc__xml_ctx_ptr;
  epc__info_t *epc__info = epc__xml_ctx->epc__info;
  epc__call_t *epc__call = epc__xml_ctx->epc__call;
  char *interface_name = NULL, *function_name = NULL, *argument_name = NULL;
  size_t nr;

  dbug_enter (__FILE__, (char *) name, __LINE__, NULL);
  dbug_print (__LINE__, "input", "qname: %s; name: %s; namespace: %s",
              (char *)qname, (char *) name,
              (namespace == NULL ? "(null)" : (char *)namespace));

  if (strncmp ((const char *) name, "SOAP", 4) == 0 ||
      strncmp ((const char *) name, "soap", 4) == 0)
    {
      dbug_print (__LINE__, "info", "skipping SOAP element");
    }
  else
    {
      if (epc__call->function == NULL && strncmp((char *)qname, "SOAP-ENV", 8) != 0)
        {
          /* elements is a new method */

          /* qname: ns1:do_system_call; name: do_system_call; namespace: demo */
          const char *colon = strchr((char *)qname, ':');

          function_name = (char *) name;
          if ( colon != NULL )
            {
              /* copy part before (excluding ':') name in qname */
              (void) snprintf(epc__call->inline_namespace,
                              sizeof(epc__call->inline_namespace),
                              "%.*s", strlen((char*)qname) - strlen((char*)name) - 1,
                              (const char *)qname);
              epc__call->inline_namespace[sizeof(epc__call->inline_namespace)-1] = '\0';
            }
          else
            {
              epc__call->inline_namespace[0] = '\0';
            }

          dbug_print (__LINE__, "info", "method: %s; inline namespace: %s",
                      function_name, epc__call->inline_namespace);

          interface_name = (char *) namespace;
          lookup_interface (interface_name, epc__info, epc__call);
          lookup_function (function_name, epc__info, epc__call);

          switch (epc__call->epc__error)
            {
            case INTERFACE_UNKNOWN:
              /* construct the response */
              (void) snprintf (epc__call->msg_response,
                               MAX_MSG_RESPONSE_LEN + 1,
                               SOAP_HEADER_START "<SOAP-ENV:Fault>\
<faultcode>Client</faultcode><faultstring>interface %s unknown</faultstring>\
</SOAP-ENV:Fault>" SOAP_HEADER_END, interface_name);
              break;

            case FUNCTION_UNKNOWN:
              /* construct the response */
              (void) snprintf (epc__call->msg_response,
                               MAX_MSG_RESPONSE_LEN + 1,
                               SOAP_HEADER_START "<SOAP-ENV:Fault>\
<faultcode>Client</faultcode><faultstring>function %s unknown</faultstring>\
</SOAP-ENV:Fault>" SOAP_HEADER_END, function_name);
              break;

            case OK:
              break;

            default:
              assert (epc__call->epc__error == INTERFACE_UNKNOWN
                      || epc__call->epc__error == FUNCTION_UNKNOWN
                      || epc__call->epc__error == OK);
            }

          /* nullify all parameters */

          if (epc__call->interface != NULL &&
              epc__call->function != NULL)
            {
              for (nr = 0; nr < epc__call->function->num_parameters; nr++)
                switch (epc__call->function->parameters[nr].type)
                  {
                  case C_STRING:
                    *((char *) epc__call->function->parameters[nr].data) = '\0';
                    break;

                  case C_INT:
                    *((idl_int_t *) epc__call->function->parameters[nr].data) = 0;
                    break;

                  case C_LONG:
                    *((idl_long_t *) epc__call->function->parameters[nr].data) = 0L;
                    break;

                  case C_FLOAT:
                    *((idl_float_t *) epc__call->function->parameters[nr].data) = 0.0F;
                    break;

                  case C_DOUBLE:
                    *((idl_double_t *) epc__call->function->parameters[nr].data) = 0.0F;
                    break;

                  case C_VOID:
                    break;

                  default:
                    assert (epc__call->function->parameters[nr].type >= C_DATATYPE_MIN
                            && epc__call->function->parameters[nr].type <= C_DATATYPE_MAX);
                  }
            }
        }
      else if (epc__call->function != NULL)
        {
          /* element is an argument */
          dbug_print (__LINE__, "info", "argument: %s", (char *) name);

          argument_name = (char *) name;

          /* get next in or inout argument */
          for (nr = epc__xml_ctx->num_parameters;
               nr < epc__call->function->num_parameters; nr++)
            {
              if (epc__call->function->parameters[nr].mode != C_OUT)    /* in or in/out */
                {
                  dbug_print (__LINE__, "info",
                              "parameter[%d]: %s; argument_name: %s",
                              (int) nr,
                              epc__call->function->parameters[nr].
                              name, argument_name);
                  assert (strcmp
                          (epc__call->function->parameters[nr].name,
                           argument_name) == 0);
                  break;        /* found */
                }
            }

          assert (nr < epc__call->function->num_parameters);    /* should be found */
          if (nr >= epc__call->function->num_parameters)
            {
              epc__call->epc__error = PARAMETER_UNKNOWN;
            }
          else
            {
              epc__xml_ctx->num_parameters = nr + 1;    /* next time: search from next parameter */
            }
        }
    }

  dbug_print (__LINE__, "info", "epc__error: %d",
              (int) epc__call->epc__error);

  return XMLERR_OK;
}

/**
 * @brief End parsing an element.
 *
 * This is a callback called at the end of parsing an element.
 *
 * Used for debugging purposes only.
 *
 * @param epc__xml_ctx_ptr  Callback data.
 * @param name              Name of the element.
 *
 * @return XMLERR_OK
 *
 ******************************************************************************/
static sword
end_element (void *epc__xml_ctx_ptr, const oratext * name)
{
  dbug_leave (__LINE__, NULL);

  return XMLERR_OK;
}


/**
 * @brief Set the data of an argument.
 *
 * This is a callback called after parsing the element content.
 *
 * @param epc__xml_ctx_ptr  Callback data.
 * @param ch                Characters.
 * @param len               Length of ch.
 *
 * @return XMLERR_OK
 *
 ******************************************************************************/
static sword
element_content (void *epc__xml_ctx_ptr, const oratext * ch, size_t len)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *) epc__xml_ctx_ptr;
  epc__call_t *epc__call = epc__xml_ctx->epc__call;
  const dword_t nr = epc__xml_ctx->num_parameters - 1;

  assert (epc__call != NULL);
  assert (epc__call->function != NULL);
  assert (nr >= 0 && nr < epc__call->function->num_parameters);
  assert (epc__call->function->parameters[nr].mode != C_OUT);   /* in or in/out */

  dbug_print (__LINE__, "info", "element content: %*s", len, (char *) ch);

  switch (epc__call->function->parameters[nr].type)
    {
    case C_STRING:
      assert (len < epc__call->function->parameters[nr].size);
      if (len >= epc__call->function->parameters[nr].size)
        {
          len = epc__call->function->parameters[nr].size - 1;
        }
      (void) strncpy ((char *) epc__call->function->parameters[nr].data,
                      (char *) ch, len);
      ((char *) epc__call->function->parameters[nr].data)[len] = '\0';
      break;

    case C_INT:
      *((idl_int_t *) epc__call->function->parameters[nr].data) =
        (int) strtol (ch, NULL, 10);
      break;

    case C_LONG:
      *((idl_long_t *) epc__call->function->parameters[nr].data) =
        strtol (ch, NULL, 10);
      break;

    case C_FLOAT:
      *((idl_float_t *) epc__call->function->parameters[nr].data) =
        strtof (ch, NULL);
      break;

    case C_DOUBLE:
      *((idl_double_t *) epc__call->function->parameters[nr].data) =
        strtod (ch, NULL);
      break;

    case C_VOID:                /* impossible */
      assert (epc__call->function->parameters[nr].type != C_VOID);
      break;

    default:
      assert (epc__call->function->parameters[nr].type >= C_DATATYPE_MIN &&
              epc__call->function->parameters[nr].type <= C_DATATYPE_MAX);
    }

  return XMLERR_OK;
}

/**
 * @brief Lookup the interface in the list of interfaces.
 *
 * @param interface_name  The interface name
 * @param epc__info       The EPC run-time information
 * @param epc__call       The interface member is set if the interface is found.
 *                        If not found the epc__error member is set to
 *                        INTERFACE_UNKNOWN.
 *
 ******************************************************************************/
static void
lookup_interface (const char *interface_name, epc__info_t * epc__info,
                  epc__call_t * epc__call)
{
  int inr;
  int result;

  /* get the interface */
  for (inr = 0, epc__call->interface = NULL; inr < epc__info->num_interfaces;
       inr++)
    if ((result =
         strcmp (interface_name, epc__info->interfaces[inr]->name)) == 0)
      {
        epc__call->interface = epc__info->interfaces[inr];
        break;
      }
    else if (result < 0)        /* interfaces sorted ascending */
      {
        break;
      }

  if (epc__call->interface == NULL)
    {
      /* interface not found */
      fprintf (stderr, "ERROR: interface %s not found\n", interface_name);
      epc__call->epc__error = INTERFACE_UNKNOWN;
    }
}

/**
 * @brief Lookup the function in the list of functions of an interface.
 *
 * @param function_name   The function name
 * @param epc__info       The EPC run-time information
 * @param epc__call       The interface member is set if the interface is found.
 *                        If not found the epc__error member is set to 
 *                        FUNCTION_UNKNOWN.
 *
 ******************************************************************************/
static void
lookup_function (const char *function_name, epc__info_t * epc__info,
                 epc__call_t * epc__call)
{
  int fnr;
  int result;

  if (epc__call->interface == NULL)
    {
      epc__call->function = NULL;
    }
  else
    {
      /* get the function */
      for (fnr = 0, epc__call->function = NULL;
           fnr < epc__call->interface->num_functions; fnr++)
        if ((result =
             strcmp (function_name,
                     epc__call->interface->functions[fnr].name)) == 0)
          {
            epc__call->function = &epc__call->interface->functions[fnr];
            break;
          }
        else if (result < 0)    /* interface functions sorted ascending */
          {
            break;
          }
    }

  if (epc__call->function == NULL)
    {
      /* interface not found */
      fprintf (stderr, "ERROR: function '%s' not found\n", function_name);
      epc__call->epc__error = FUNCTION_UNKNOWN;
    }
}
