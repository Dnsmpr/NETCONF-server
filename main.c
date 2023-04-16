#include "parse.h"

int main()
{
    // Initialize the library
    xmlInitParser();

    // Parse the XML request
    xmlDocPtr doc = xmlReadMemory(REQUEST, sizeof(REQUEST), NULL, NULL, 0);
    if (doc == NULL) {
        printf("Failed to parse XML request\n");
        return 1;
    }

    // Get the root element of the request
    xmlNodePtr root = xmlDocGetRootElement(doc);

    // Print the type of request
    printf("Request type: %s\n", root->name);

    // Traverse the XML tree and print the value of the field
    traverse_xml(root, &print_element);

    // Clean up
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}
