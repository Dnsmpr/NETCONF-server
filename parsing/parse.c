#include "parse.h"
#include "network.h"
#include "abcc.h"

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

char* create_xml_reply(KeyValuePairArray *array, abcc* device, char *message_id) {
  if(!strcmp(array->request_type, HELLO)) {
    return NETCONF_HELLO;
  }

  if(!strcmp(array->request_type, RPC)) {
    if(!get_key(array, GET)) {
      if(!get_key(array, NETCONF_STATE)) {
        return NETCONF_RESPONSE_2;
      }
      if(!get_key(array, MODULE_STATE)) {
          return NETCONF_RESPONSE_1;
      }
      return create_netconf_response_4(device->IP_ADDRESS, message_id);
    }

    if(!get_key(array, GET_SCHEMA)) {
      return NETCONF_RESPONSE_1;
    }

  if(!get_key(array, EDIT_CONFIG)) {
    if(get_key(array, RUNNING)) {
      return create_netconf_response_4(device->IP_ADDRESS, message_id);
    }
    void* value = NULL;
    get_value(array, IP, &value);
    set_IP_ADDRESS(device, (char*) value);
    return NETCONF_RESPONSE_6;

  }

    if(!get_key(array, GET_CONFIG)) {
      return NETCONF_RESPONSE_3;
    } 
      

  }


  return "";

}

char *create_netconf_response_4(char* IP_ADR, char *msg_num) {
    xmlDocPtr doc = NULL;
    xmlNodePtr root_element = NULL;

    // Create a new XML document
    doc = xmlNewDoc(BAD_CAST "1.0");

    // Create the root element 'rpc-reply' and add it to the document
    root_element = xmlNewNode(NULL, BAD_CAST "rpc-reply");
    xmlNewProp(root_element, BAD_CAST "xmlns", BAD_CAST "urn:ietf:params:xml:ns:netconf:base:1.0");
    xmlNewProp(root_element, BAD_CAST "message-id", BAD_CAST msg_num);
    xmlDocSetRootElement(doc, root_element);

    // Create 'data' element and add it to the root element
    xmlNodePtr data_element = xmlNewChild(root_element, NULL, BAD_CAST "data", NULL);

    // Create 'abcc' element and add it to the 'data' element
    xmlNodePtr abcc_element = xmlNewChild(data_element, NULL, BAD_CAST "abcc", NULL);
    xmlNewProp(abcc_element, BAD_CAST "xmlns", BAD_CAST "urn:hms:abcc");

    // Create 'attributes' element and add it to the 'abcc' element
    xmlNodePtr attributes_element = xmlNewChild(abcc_element, NULL, BAD_CAST "attributes", NULL);

    // Create 'IP_ADDRESS' element and add it to the 'attributes' element
    xmlNewChild(attributes_element, NULL, BAD_CAST "IP_ADDRESS", BAD_CAST IP_ADR);

    // Serialize the XML document to a string
    xmlChar *xml_buff;
    int buffersize;
    xmlDocDumpFormatMemory(doc, &xml_buff, &buffersize, 0);

    // Allocate memory for the new string, including the end of message pattern and the null terminator
    char *output = (char *) malloc(buffersize + 1 + 6);
    strncpy(output, (char *) xml_buff, buffersize);
    strcpy(output + buffersize, "]]>]]>");
    output[buffersize + 6] = '\0';

    // Clean up memory allocations
    xmlFree(xml_buff);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return output;
}


char *int_to_str(int num) {
    int len = snprintf(NULL, 0, "%d", num);
    char *str = (char *) malloc(len + 1);
    snprintf(str, len + 1, "%d", num);
    return str;
}

void extract_xml(xmlNodePtr node, KeyValuePairArray* array)
{
  void *value = NULL; // Initialize value to NULL
    if (node->type == XML_ELEMENT_NODE) {
        if (node->children != NULL && node->children->type == XML_TEXT_NODE) {
          value = (void*) node->children->content;
        }
          KeyValuePair kv;
          init_key_value_pair(&kv, (const char*)node->name, value, value ? xmlStrlen(node->children->content) + 1 : 0); // Handle NULL value case
          add_key_value(array, &kv);
    }
}

void traverse_xml(xmlNodePtr node, KeyValuePairArray* array, void (*xml_operation)(xmlNodePtr, KeyValuePairArray* array))
{
  for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
    xml_operation(child, array);
    traverse_xml(child, array, xml_operation);
  }
}


