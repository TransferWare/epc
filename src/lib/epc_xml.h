#ifndef EPC_XML_H
#define EPC_XML_H 1

/* This structure contains the context while parsing an XML document */
typedef struct {
  struct epc_info *epc_info;
  struct epc_call *epc_call;
  long num_parameters; /* number of in or in/out parameters parsed yet */
} epc_xml_ctx_t;

struct xmlctx;

extern
unsigned int
epc_xml_init( struct xmlctx **xmlctx, epc_xml_ctx_t *epc_xml_ctx );

extern
unsigned int
epc_xml_done( struct xmlctx **xmlctx );

extern
unsigned int
epc_xml_parse( struct xmlctx *xmlctx, const char *buf, const size_t len );

#endif
