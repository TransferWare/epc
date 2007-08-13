/*@+posixlib@*/

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

#ifndef HAVE__INT64

#if defined(HAVE___INT64)

#define _int64 __int64

#elif defined(HAVE_UNSIGNED_LONG_LONG)

#define _int64 unsigned long long

#else

typedef struct {
  unsigned long m1, m2;
} _int64;

#endif

#endif

#include <oratypes.h>
#include <oraxml.h>

#define string_defined
#include <epc_xml.h>

/* include dmalloc as last one */
#ifdef WITH_DMALLOC
#include "dmalloc.h"
#endif

#define LEVEL_SOAP_ENVELOPE 1
#define LEVEL_SOAP_BODY 2
#define LEVEL_SOAP_METHOD 3
#define LEVEL_SOAP_ARGUMENT 4

/* strtof is not a standard function */
extern float strtof(const char* s, /*@null@ */ char** endptr);

/*

Method call

<?xml version="1.0"?>
<methodCall>
   <methodName>examples.getStateName</methodName>
   <params>
      <param>
         <value><i4>41</i4></value>
       </param>
   </params>
</methodCall>

Method response

<?xml version="1.0"?>
<methodResponse>
  <params>
    <param>
        <value><string>South Dakota</string></value>
    </param>
  </params>
</methodResponse>

Fault

<?xml version="1.0"?>
<methodResponse>
  <fault>
    <value>
      <struct>
        <member>
          <name>faultCode</name>
          <value><int>4</int></value>
        </member>
        <member>
          <name>faultString</name>
          <value><string>Too many parameters.</string></value>
        </member>
      </struct>
    </value>
  </fault>
</methodResponse>
*/

#define LEVEL_XMLRPC_METHOD 2
#define LEVEL_XMLRPC_ARGUMENT 5

/* This structure contains the context while parsing an XML document */
typedef struct
{
  /*@temp@ */ struct epc__info *epc__info;
  /*@temp@ *//*@null@ */ struct epc__call *epc__call;
  long num_parameters;          /* number of in or in/out parameters already parsed */
  unsigned int level; /* element level:
                         1 - SOAP envelope
                         2 - SOAP body
                         3 - method name
                         4 - argument
                         ... - inside XML argument
                      */
} epc__xml_ctx_t;

typedef struct xml_info
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
               const oratext *namespace, const struct xmlnodes *attrs);

static sword end_element (void *epc__xml_ctx_ptr, const oratext * name);

static
sword
element_content (void *epc__xml_ctx_ptr, const oratext * ch, size_t len);

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
  /*@-nullassign@ */
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
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  };
  /*@=nullassign@ */
  /*@only@ */ xml_info_t *xml_info;

  DBUG_ENTER ("epc__xml_init");

  assert (epc__info->xml_info == NULL);

  xml_info = (xml_info_t *) malloc (sizeof (xml_info_t));

  assert (xml_info != NULL);

  xml_info->epc__xml_ctx.epc__info = epc__info;
  xml_info->epc__xml_ctx.epc__call = NULL;
  xml_info->epc__xml_ctx.num_parameters = 0L;
  xml_info->epc__xml_ctx.level = 0;

  xml_info->saxcb = saxcb;

  /*@-nullpass@ */
  xml_info->xmlctx = xmlinit (&ecode,
                              NULL,
                              error_handler,
                              &xml_info->epc__xml_ctx,
                              &xml_info->saxcb,
                              &xml_info->epc__xml_ctx, NULL, NULL, NULL);
  /*@=nullpass@ */

  DBUG_PRINT ("info", ("xmlctx: %p", xml_info->xmlctx));
  DBUG_LEAVE ();

  epc__info->xml_info = xml_info;

  assert (epc__info->xml_info != NULL);

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

  assert (xml_info != NULL);

  DBUG_PRINT ("info", ("xmlctx: %p", xml_info->xmlctx));

  ecode = xmlterm (xml_info->xmlctx);   /* terminate XML package */
  xml_info->epc__xml_ctx.epc__info = NULL;
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

  assert (xml_info != NULL);
  DBUG_PRINT ("info", ("xmlctx: %p", xml_info->xmlctx));
  DBUG_PRINT ("input", ("buf: %*s", len, buf));

  xml_info->epc__xml_ctx.epc__call = epc__call;
  /*@-nullpass@ */
  ecode = xmlparsebuf (xml_info->xmlctx, (oratext *) buf, len, NULL, flags);
  /*@=nullpass@ */
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

  DBUG_ENTER ("error_handler");

  assert (epc__call != NULL);

  DBUG_PRINT ("info", ("protocol: %c", EPC__CALL_PROTOCOL(epc__call)));

  switch(EPC__CALL_PROTOCOL(epc__call))
    {
    case PROTOCOL_SOAP:
      epc__call->epc__error = PARSE_ERROR;

      if (errcode <= 99)
        {
          /* server side error */
          /* copy */
          (void) snprintf (epc__call->msg_response,
                           MAX_MSG_RESPONSE_LEN + 1,
                           SOAP_HEADER_START "<SOAP-ENV:Fault>\n\
<faultcode>Server</faultcode><faultstring>%s</faultstring>\n\
</SOAP-ENV:Fault>" SOAP_HEADER_END, (char *) msg);
        }
      else
        {
          /* client side error */
          /* copy */
          (void) snprintf (epc__call->msg_response,
                           MAX_MSG_RESPONSE_LEN + 1,
                           SOAP_HEADER_START "<SOAP-ENV:Fault>\n\
<faultcode>Client</faultcode><faultstring>%s</faultstring>\n\
</SOAP-ENV:Fault>" SOAP_HEADER_END, (char *) msg);
        }
      break;

    case PROTOCOL_XMLRPC:
      /* copy */
      (void) snprintf (epc__call->msg_response,
                       MAX_MSG_RESPONSE_LEN + 1,
                       "<methodResponse><fault><value><struct>\n\
<member><name>faultCode</name><value><int>%d</int></value></member>\n\
<member><name>faultString</name><value><string>%s</string></value></member>\n\
</struct></value></fault></methodResponse>", (int) errcode, (char *) msg);
      break;

    default:
      assert(EPC__CALL_PROTOCOL(epc__call) >= PROTOCOL_XML_MIN ||
             EPC__CALL_PROTOCOL(epc__call) <= PROTOCOL_XML_MAX);
      break;
    }

  DBUG_LEAVE ();
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
 * <li>((epc__xml_ctx_t *) epc__xml_ctx_ptr)->level</li>
 * </ul>
 *
 * @param epc__xml_ctx_ptr  Callback data.
 *
 * @return XMLERR_xxx
 *
 ******************************************************************************/
static sword
start_document (void *epc__xml_ctx_ptr)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *) epc__xml_ctx_ptr;
  epc__call_t *epc__call = epc__xml_ctx->epc__call;
  sword ecode = XMLERR_OK;

  DBUG_ENTER("start_document");

  assert (epc__call != NULL);

  epc__call->interface = NULL;
  epc__call->function = NULL;
  epc__call->epc__error = OK;
  epc__xml_ctx->num_parameters = 0;
  epc__xml_ctx->level = 0;

  DBUG_LEAVE ();

  return ecode;
}

/**
 * @brief Stop parsing a document.
 *
 * This is a callback called at the end of parsing a document. 
 * Used for debugging purposes only.
 *
 * @param epc__xml_ctx_ptr  Callback data.
 *
 * @return XMLERR_xxx
 *
 ******************************************************************************/
static sword
end_document (void *epc__xml_ctx_ptr)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *) epc__xml_ctx_ptr;
  epc__call_t *epc__call = (epc__call_t *) epc__xml_ctx->epc__call;
  dword_t nr;
  sword ecode = XMLERR_OK;

  DBUG_ENTER("end_document");

  assert (epc__call != NULL);

  DBUG_PRINT ("info", ("method: %s",
                       (epc__call->function !=
                        NULL ? epc__call->function->name : "(null)")));

  if (epc__call->function != NULL)
    {
      for (nr = 0; nr < epc__call->function->num_parameters; nr++)
        {
          DBUG_PRINT ("info", ("argument %d; name: %s",
                               nr, epc__call->function->parameters[nr].name));
        }
    }

  DBUG_LEAVE ();

  return ecode;
}

/**
 * @brief Nullify parameters
 *
 * @param epc__call  Epc call
 *
 ******************************************************************************/
static void
nullify_parameters (epc__call_t *epc__call)
{
  dword_t nr;

  if (epc__call->interface != NULL && epc__call->function != NULL)
    {
      for (nr = 0; nr < epc__call->function->num_parameters; nr++)
        switch (epc__call->function->parameters[nr].type)
          {
          case C_STRING:
          case C_XML:
          case C_DATE:
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

/**
 * @brief Set a parameter value
 *
 * @param ch         Character buffer (no terminating zero needed)
 * @param len        Number of characters in the character buffer
 * @param parameter  Epc parameter to set the value for
 *
 ******************************************************************************/
static sword
set_parameter (const char *ch, size_t len, epc__parameter_t *parameter)
{
  sword ecode = XMLERR_OK;

  switch (parameter->type)
    {
    case C_XML:
      /* append data */
      if ((dword_t) (len + strlen((char *) parameter->data)) < parameter->size)
        {
          (void) snprintf( ((char *) parameter->data) + 
                           strlen((char *) parameter->data),
                           (size_t)(parameter->size -
                                    strlen((char *) parameter->data)),
                           "%.*s", (int)len, (char *)ch);
        }
      else
        {
          ecode = XMLERR_BUFFER_OVERFLOW;
        }
      break;

    case C_STRING:
    case C_DATE:
      if ((dword_t) len < parameter->size)
        {
          (void) strncpy ((char *) parameter->data,
                          (char *) ch, len);
          ((char *) parameter->data)[len] = '\0';
        }
      else
        {
          ecode = XMLERR_BUFFER_OVERFLOW;
        }
      break;

    case C_INT:
      *((idl_int_t *) parameter->data) =
        (int) strtol ((char *) ch, NULL, 10);
      break;

    case C_LONG:
      *((idl_long_t *) parameter->data) =
        strtol ((char *) ch, NULL, 10);
      break;

    case C_FLOAT:
      *((idl_float_t *) parameter->data) =
        strtof ((char *) ch, NULL);
      break;

    case C_DOUBLE:
      *((idl_double_t *) parameter->data) =
        strtod ((char *) ch, NULL);
      break;

    case C_VOID:                /* impossible */
      assert (parameter->type != C_VOID);
      break;

    default:
      assert (parameter->type >= C_DATATYPE_MIN &&
              parameter->type <= C_DATATYPE_MAX);
    }

  return ecode;
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
 * @return XMLERR_xxx
 *
 ******************************************************************************/
static sword
start_element (void *epc__xml_ctx_ptr,
               const oratext *qname,
               const oratext *name, const oratext *namespace,
               const struct xmlnodes *attrs)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *) epc__xml_ctx_ptr;
  epc__info_t *epc__info = epc__xml_ctx->epc__info;
  epc__call_t *epc__call = epc__xml_ctx->epc__call;
  dword_t nr;
  sword ecode = XMLERR_OK;

  DBUG_ENTER ("start_element");
  DBUG_PRINT ("input", ("qname: %s; name: %s; namespace: %s; level: %u",
                        (char *) qname, (char *) name,
                        (namespace == NULL ? "(null)" : (char *) namespace),
                        epc__xml_ctx->level));

  assert (epc__call != NULL);

  switch(EPC__CALL_PROTOCOL(epc__call))
    {
    case PROTOCOL_SOAP:
      switch(++epc__xml_ctx->level)
        {
        case LEVEL_SOAP_ENVELOPE:
        case LEVEL_SOAP_BODY:
          assert((strncmp ((const char *) qname, "SOAP", 4) == 0 ||
                  strncmp ((const char *) qname, "soap", 4) == 0));
          DBUG_PRINT ("info", ("skipping SOAP element"));
          break;

        case LEVEL_SOAP_METHOD:
          {
            /* element is a new method */

            /* qname: ns1:do_system_call; name: do_system_call; namespace: demo */
            const char *colon = strchr ((char *) qname, ':');
            const char *interface_name = (char *)namespace, *function_name = (char *) name;

            assert (epc__call->function == NULL);
            assert (interface_name != NULL);

            if (colon != NULL)
              {
                /* copy part before (excluding ':') name in qname */
                (void) snprintf (epc__call->inline_namespace,
                                 sizeof (epc__call->inline_namespace),
                                 "%.*s",
                                 (int) (strlen ((char *) qname) -
                                        strlen ((char *) name) - 1),
                                 (const char *) qname);
                epc__call->
                  inline_namespace[sizeof (epc__call->inline_namespace) - 1] =
                  '\0';
              }
            else
              {
                epc__call->inline_namespace[0] = '\0';
              }

            DBUG_PRINT ("info",
                        ("method: %s; inline namespace: %s",
                         function_name, epc__call->inline_namespace));

            epc__lookup_interface (interface_name, epc__info, epc__call);
            epc__lookup_function (function_name, epc__info, epc__call);

            switch (epc__call->epc__error)
              {
              case INTERFACE_UNKNOWN:
                /* construct the response */
                /* copy */
                (void) snprintf (epc__call->msg_response,
                                 MAX_MSG_RESPONSE_LEN + 1,
                                 SOAP_HEADER_START "<SOAP-ENV:Fault>\n\
<faultcode>Client</faultcode><faultstring>interface %s unknown</faultstring>\n\
</SOAP-ENV:Fault>" SOAP_HEADER_END, interface_name);
                break;

              case FUNCTION_UNKNOWN:
                /* construct the response */
                /* copy */
                (void) snprintf (epc__call->msg_response,
                                 MAX_MSG_RESPONSE_LEN + 1,
                                 SOAP_HEADER_START "<SOAP-ENV:Fault>\n\
<faultcode>Client</faultcode><faultstring>function %s unknown</faultstring>\n\
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
            nullify_parameters(epc__call);
          }
          break;

        case LEVEL_SOAP_ARGUMENT:
          {
            const char *argument_name = (char *)name;

            assert (epc__call->function != NULL);

            /* element is an argument or part of an xml argument */
            DBUG_PRINT ("info", ("argument: %s", (char *) name));

            /* get next in or inout argument */
            for (nr = epc__xml_ctx->num_parameters;
                 nr < epc__call->function->num_parameters; nr++)
              {
                if (epc__call->function->parameters[nr].mode != C_OUT) /* in or in/out */ 
                  {
                    DBUG_PRINT ("info",
                                ("parameter[%d]: %s; argument_name: %s",
                                 (int) nr,
                                 epc__call->function->parameters[nr].
                                 name, argument_name));
                    assert(strcmp(epc__call->function->parameters[nr].name, argument_name) == 0);
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
          break;

        default:
          assert(epc__call->function != NULL);
          assert(epc__call->function->parameters != NULL);
          {
            char *data = (char *)epc__call->function->parameters[nr = (epc__xml_ctx->num_parameters - 1)].data;

            /* inside xml parameter */
            assert(nr >= 0);
            assert(epc__call->function->parameters[nr].mode != C_OUT);
            assert(epc__call->function->parameters[nr].type == C_XML);

            /* append data */
            (void) snprintf(data + strlen(data),
                            (size_t)(epc__call->function->parameters[nr].size - strlen(data)),
                            "<%s", (char *)qname);

            if (attrs != NULL)
              {
                size_t attr_nr;

                /*@-mustfreefresh@*/
                for ( attr_nr = 1; attr_nr <= numAttributes(attrs); attr_nr++ )
                  {
                    xmlnode *attr = getAttributeIndex(attrs, attr_nr);
                    const char *attr_name = (const char *)getAttrQualifiedName(attr);
                    const char *attr_value = (const char *)getAttrValue(attr);

                    /* append data */
                    (void) snprintf(data + strlen(data),
                                    (size_t)(epc__call->function->parameters[nr].size - strlen(data)),
                                    " %s=%s", 
                                    attr_name,
                                    attr_value);
                  }
                /*@=mustfreefresh@*/
              }

            /* append */
            (void) snprintf(data + strlen(data),
                            (size_t)(epc__call->function->parameters[nr].size - strlen(data)),
                            ">");
          }
          break;
        }
      break;

    case PROTOCOL_XMLRPC:
      switch (++epc__xml_ctx->level)
        {
        case LEVEL_XMLRPC_ARGUMENT:
          {
            assert (epc__call->function != NULL);

            DBUG_PRINT ("info",
                        ("arguments parsed so far: %d",
                         (int) epc__xml_ctx->num_parameters));

            /* get next in or inout argument */
            for (nr = epc__xml_ctx->num_parameters;
                 nr < epc__call->function->num_parameters; nr++)
              {
                if (epc__call->function->parameters[nr].mode != C_OUT) /* in or in/out */ 
                  {
                    DBUG_PRINT ("info",
                                ("parameter[%d]: %s",
                                 (int) nr,
                                 epc__call->function->parameters[nr].
                                 name));
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

                switch (epc__call->function->parameters[nr].type)
                  {
                  case C_XML: // TODO: no XML for XMLRPC
                    break;

                  case C_STRING:
                    if (strcmp ((char *)name, "string") != 0) {
                      epc__call->epc__error = DATATYPE_UNKNOWN;
                    }
                    break;

                  case C_DATE:
                    if (strcmp ((char *)name, "dateTime.iso8601") != 0) {
                      epc__call->epc__error = DATATYPE_UNKNOWN;
                    }
                    break;

                  case C_INT:
                  case C_LONG:
                    if (strcmp ((char *)name, "int") != 0
                        && strcmp ((char *)name, "i4") != 0) {
                      epc__call->epc__error = DATATYPE_UNKNOWN;
                    }
                    break;

                  case C_FLOAT:
                  case C_DOUBLE:
                    if (strcmp ((char *)name, "double") != 0) {
                      epc__call->epc__error = DATATYPE_UNKNOWN;
                    }
                    break;

                  case C_VOID:                /* impossible */
                    break;

                  default:
                    assert (epc__call->function->parameters[nr].type >= C_DATATYPE_MIN &&
                            epc__call->function->parameters[nr].type <= C_DATATYPE_MAX);
                  }
              }
          }
          break;
        }
      break;

    default:
      assert(EPC__CALL_PROTOCOL(epc__call) >= PROTOCOL_XML_MIN ||
             EPC__CALL_PROTOCOL(epc__call) <= PROTOCOL_XML_MAX);
      break;
    }

  DBUG_PRINT ("info", ("epc__error: %d", (int) epc__call->epc__error));

  DBUG_LEAVE ();

  return ecode;
}

/**
 * @brief End parsing an element.
 *
 * This is a callback called at the end of parsing an element.
 *
 * @param epc__xml_ctx_ptr  Callback data.
 * @param name              Name of the element.
 *
 * @return XMLERR_xxx
 *
 ******************************************************************************/
static sword
end_element (void *epc__xml_ctx_ptr, const oratext * name)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *) epc__xml_ctx_ptr;
  epc__call_t *epc__call = epc__xml_ctx->epc__call;
  sword ecode = XMLERR_OK;

  DBUG_ENTER ("end_element");
  DBUG_PRINT ("info", ("name: %s; level: %u",
                       (char *) name,
                       epc__xml_ctx->level));

  assert(epc__call != NULL);

  switch(EPC__CALL_PROTOCOL(epc__call))
    {
    case PROTOCOL_SOAP:
      switch( epc__xml_ctx->level-- )
        {
        case LEVEL_SOAP_ENVELOPE:
        case LEVEL_SOAP_BODY:
        case LEVEL_SOAP_METHOD:
        case LEVEL_SOAP_ARGUMENT:
          break;

        default:
          assert(epc__call != NULL);
          assert(epc__call->function != NULL);
          assert(epc__call->function->parameters != NULL);
          {
            const dword_t nr = epc__xml_ctx->num_parameters - 1;
            char *data = (char *)epc__call->function->parameters[nr].data;

            assert(epc__xml_ctx->level != 0);

            /* append data */
            (void) snprintf(data + strlen(data),
                            (size_t)(epc__call->function->parameters[nr].size - strlen(data)),
                            "</%s>", (char *)name);
          }
        }
      break;

    case PROTOCOL_XMLRPC:
      epc__xml_ctx->level--;
      break;

    default:
      assert(EPC__CALL_PROTOCOL(epc__call) >= PROTOCOL_XML_MIN ||
             EPC__CALL_PROTOCOL(epc__call) <= PROTOCOL_XML_MAX);
      break;
    }

  DBUG_LEAVE ();

  return ecode;
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
 * @return XMLERR_xxx
 *
 ******************************************************************************/
static sword
element_content (void *epc__xml_ctx_ptr, const oratext * ch, size_t len)
{
  epc__xml_ctx_t *epc__xml_ctx = (epc__xml_ctx_t *) epc__xml_ctx_ptr;
  epc__info_t *epc__info = epc__xml_ctx->epc__info;
  epc__call_t *epc__call = epc__xml_ctx->epc__call;
  dword_t nr = epc__xml_ctx->num_parameters - 1;
  sword ecode = XMLERR_OK;

  DBUG_ENTER ("element_content");

  DBUG_PRINT ("info", ("level: %u; element content: %*s",
                       epc__xml_ctx->level,
                       len, (char *) ch));

  assert (epc__call != NULL);

  switch (EPC__CALL_PROTOCOL(epc__call))
    {
    case PROTOCOL_SOAP:
      assert (epc__call->function != NULL);
      assert (nr >= 0 && nr < epc__call->function->num_parameters);
      assert (epc__call->function->parameters[nr].mode != C_OUT);   /* in or in/out */

      ecode = set_parameter((const char *)ch, (size_t)len, &epc__call->function->parameters[nr]);
      break;

    case PROTOCOL_XMLRPC:
      switch(epc__xml_ctx->level)
        {
        case LEVEL_XMLRPC_METHOD:
          assert (epc__call->function == NULL);

          {
            const char *dot = strchr ((char *) ch, '.');
            char interface_name[MAX_INTERFACE_NAME_LEN] = "";
            char *function_name = NULL;

            if (dot == NULL) {
              epc__call->epc__error = INTERFACE_UNKNOWN;
            } else {
              function_name = ((char *)dot) + 1;

              DBUG_PRINT ("info",
                          ("function: %s",
                           function_name));

              (void) strncpy(interface_name, (char *)ch, (size_t)(dot - (char *)ch));
              interface_name[(dot - (char *)ch)] = '\0';

              DBUG_PRINT ("info",
                          ("interface: %s",
                           interface_name));

              assert (epc__call->function == NULL);

              epc__call->inline_namespace[0] = '\0';

              epc__lookup_interface (interface_name, epc__info, epc__call);
              epc__lookup_function (function_name, epc__info, epc__call);
            }

            switch (epc__call->epc__error)
              {
              case INTERFACE_UNKNOWN:
                /* construct the response */
                /* copy */
                (void) snprintf (epc__call->msg_response,
                                 MAX_MSG_RESPONSE_LEN + 1,
                                 "<methodResponse><fault><value><struct>\n\
<member><name>faultCode</name><value><int>%d</int></value></member>\n\
<member><name>faultString</name><value><string>interface %s unknown</string></value></member>\n\
</struct></value></fault></methodResponse>", (int)epc__call->epc__error, interface_name);
                break;

              case FUNCTION_UNKNOWN:
                assert(function_name != NULL);
                /* construct the response */
                /* copy */
                (void) snprintf (epc__call->msg_response,
                                 MAX_MSG_RESPONSE_LEN + 1,
                                 "<methodResponse><fault><value><struct>\n\
<member><name>faultCode</name><value><int>%d</int></value></member>\n\
<member><name>faultString</name><value><string>function %s unknown</string></value></member>\n\
</struct></value></fault></methodResponse>", (int) epc__call->epc__error, function_name);
                break;

              case OK:
                break;

              default:
                assert (epc__call->epc__error == INTERFACE_UNKNOWN
                        || epc__call->epc__error == FUNCTION_UNKNOWN
                        || epc__call->epc__error == OK);
              }

            /* nullify all parameters */
            nullify_parameters(epc__call);
          }
          break;

        case LEVEL_XMLRPC_ARGUMENT:
          {
            assert (epc__call->function != NULL);
            assert (nr >= 0 && nr < epc__call->function->num_parameters);
            assert (epc__call->function->parameters[nr].mode != C_OUT);   /* in or in/out */

            ecode = set_parameter((const char *)ch, (size_t)len, &epc__call->function->parameters[nr]);
          }
          break;

        default:
          DBUG_PRINT ("info",
                      ("level does not denote a method or argument"));
          break;
        }
      break;

    default:
      assert(EPC__CALL_PROTOCOL(epc__call) >= PROTOCOL_XML_MIN ||
             EPC__CALL_PROTOCOL(epc__call) <= PROTOCOL_XML_MAX);
      break;
    }

  DBUG_LEAVE ();

  return ecode;
}

