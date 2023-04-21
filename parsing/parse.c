#include "parse.h"
#include "network.h"

int parse_xml(char* request, xmlNodePtr* root, xmlDocPtr* document){
    char *request_copy = strdup(request);
    if (request_copy == NULL) {
        printf("Failed to allocate memory for request_copy\n");
        return -1;
    }

    char *delimiter = strstr(request_copy, "]]>]]>");
    if (delimiter != NULL) {
        *delimiter = '\0';
    }

    *document = xmlReadMemory(request_copy, strlen(request_copy), NULL, NULL, 0);
    if (*document == NULL) {
        printf("Failed to parse XML request\n");
        free(request_copy);
        return -1;
    }
    *root = xmlDocGetRootElement(*document);

    free(request_copy);
    return 1;
}

void print_element(xmlNodePtr node)
{
    if (node->type == XML_ELEMENT_NODE) {
        printf("Element: %s\n", node->name);
        if (node->children != NULL && node->children->type == XML_TEXT_NODE) {
            printf("Value: %s\n", node->children->content);
        }
    }
}

int process_xml(KeyValuePairArray* array, xmlNodePtr node) {
  if(!node || !array){
    return -1;
  }

  const char *netconf_request = (const char*)(node->name); 
  if( !strcmp(netconf_request, HELLO) && !strcmp(netconf_request, RPC) ) {
    return -1;
  }
  
  set_request_type((const char*)(node->name), array);
  traverse_xml(node, array, &extract_xml);
  return 0;

}

char* create_xml_reply(KeyValuePairArray *array) {
  if(!strcmp(array->request_type, HELLO)) {
    return NETCONF_HELLO;
  }

  if(!strcmp(array->request_type, RPC)) {
  return NETCONF_RESPONSE_1;
  }


  return "";

}

void extract_xml(xmlNodePtr node, KeyValuePairArray* array)
{
    if (node->type == XML_ELEMENT_NODE) {
        if (node->children != NULL && node->children->type == XML_TEXT_NODE) {
            KeyValuePair kv;
            init_key_value_pair(&kv, (const char*)node->name, (const void*)node->children->content, xmlStrlen(node->children->content) + 1);
            add_key_value(array, &kv);
        }
    }
}

void traverse_xml(xmlNodePtr node, KeyValuePairArray* array, void (*xml_operation)(xmlNodePtr, KeyValuePairArray* array))
{
  for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
    xml_operation(child, array);
    traverse_xml(child, array, xml_operation);
  }
}


