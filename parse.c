#include "parse.h"

void traverse_xml(xmlNodePtr node, void (*xml_operation)(xmlNodePtr))
{
  for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
    xml_operation(child);
    traverse_xml(child, xml_operation);
  }
}


