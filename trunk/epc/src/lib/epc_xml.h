#ifndef EPC_XML_H
#define EPC_XML_H 1

#include <epc_defs.h>

extern
unsigned int
epc_xml_init( epc_info_t *epc_info );

extern
unsigned int
epc_xml_done( epc_info_t *epc_info );

extern
unsigned int
epc_xml_parse( epc_info_t *epc_info, epc_call_t *epc_call, const char *buf, const size_t len );

#endif
