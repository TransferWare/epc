#ifndef EPC_XML_H
#define EPC_XML_H 1

#include <oratypes.h>
#include <oraxml.h>

/* string defined in both oratypes.h and in idl_defs.h */
#define string_defined 1
#include "epc_defs.h"

typedef struct {
  epc_info_t *epc_info;
  epc_call_t *epc_call;
} epc_xml_ctx_t;

extern
unsigned int
epc_xml_init( struct xmlctx **xmlctx, void *ctx );

extern
unsigned int
epc_xml_done( struct xmlctx **xmlctx );

extern
unsigned int
epc_xml_parse( struct xmlctx *xmlctx, const char *buf, const size_t len );

#endif
