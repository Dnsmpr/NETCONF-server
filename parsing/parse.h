#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include "KeyValuePairArray.h"
#include "abcc.h"

#define get_operaton "get"
#define get_config_operaton "get-config"
#define lock_operaton "lock"
#define HELLO "hello"
#define RPC "rpc"
#define GET "get"
#define NETCONF_STATE "netconf-state"
#define MODULE_STATE "modules-state"
#define GET_SCHEMA "get-schema"
#define EDIT_CONFIG "edit-config"
#define GET_CONFIG "get-config"
#define RUNNING "running"
#define IP "IP_ADDRESS"

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
char* create_xml_reply(KeyValuePairArray *array, abcc *device, char *message_id);
char *create_netconf_response_4(char* IP_ADR, char *msg_num);
char *int_to_str(int num);