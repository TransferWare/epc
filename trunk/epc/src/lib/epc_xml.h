#ifndef EPC_XML_H
#define EPC_XML_H 1

#include <epc_defs.h>

#define SOAP_HEADER_START "<?xml version='1.0' encoding='iso-8859-1'?>\
<SOAP-ENV:Envelope xmlns:SOAP-ENV='http://schemas.xmlsoap.org/soap/envelope/' \
xmlns:SOAP-ENC='http://schemas.xmlsoap.org/soap/encoding/' \
xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' \
xmlns:xsd='http://www.w3.org/2001/XMLSchema'>\
<SOAP-ENV:Body>"

#define SOAP_HEADER_END "</SOAP-ENV:Body></SOAP-ENV:Envelope>"

extern
unsigned int
epc__xml_init( /*@temp@*/ epc__info_t *epc__info );

extern
unsigned int
epc__xml_done( epc__info_t *epc__info );

extern
unsigned int
epc__xml_parse( epc__info_t *epc__info, epc__call_t *epc__call, const char *buf, const size_t len );

#endif
