#ifndef EPC_XML_H
#define EPC_XML_H 1

#include <epc_defs.h>

extern
unsigned int
epc__xml_init( epc__info_t *epc__info );

extern
unsigned int
epc__xml_done( epc__info_t *epc__info );

extern
unsigned int
epc__xml_parse( epc__info_t *epc__info, epc__call_t *epc__call, const char *buf, const size_t len );

#endif
