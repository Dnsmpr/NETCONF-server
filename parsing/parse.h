#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include "KeyValuePairArray.h"

#define get_operaton "get"
#define get_config_operaton "get-config"
#define lock_operaton "lock"
#define HELLO "hello"

#define REQUEST "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<rpc message-id=\"101\" xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">\
  <get>\
    <filter type=\"subtree\">\
      <system-information xmlns=\"http://xml.juniper.net/xnm/1.1/xnm\">\
        <hardware-model>SRX240H</hardware-model>\
        <os-name>Junos</os-name>\
        <os-version>12.1X47-D20.7</os-version>\
      </system-information>\
    </filter>\
  </get>\
</rpc>"

void extract_xml(xmlNodePtr node, KeyValuePairArray* array);
int parse_xml(char* request, xmlNodePtr* root, xmlDocPtr* document);
void traverse_xml(xmlNodePtr node, KeyValuePairArray* array, void (*xml_operation)(xmlNodePtr, KeyValuePairArray* array));
void print_element(xmlNodePtr node);
int process_xml(KeyValuePairArray* array, xmlNodePtr node);