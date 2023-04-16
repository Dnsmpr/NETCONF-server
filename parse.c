#include "parse.h"

int parse_xml(char* request, xmlNodePtr* root, xmlDocPtr* document){
  *document = xmlReadMemory(request, strlen(request), NULL, NULL, 0);
  if (*document == NULL) {
    printf("Failed to parse XML request\n");
    return -1;
  }
  *root = xmlDocGetRootElement(*document);
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

void traverse_xml(xmlNodePtr node, void (*xml_operation)(xmlNodePtr))
{
  for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
    xml_operation(child);
    traverse_xml(child, xml_operation);
  }
}


