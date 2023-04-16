#include "parse.h"

int main()
{
    // Initialize the library
    xmlInitParser();

    // Parse the XML request
    xmlNodePtr root = NULL;
    xmlDocPtr doc = NULL;
    int result = parse_xml(REQUEST, &root, &doc);
    if (result == -1){
        printf("ERROR PARSING");
        return -1;
    }
    // Print the type of request
    printf("Request type: %s\n", root->name);

    // Traverse the XML tree and print the value of the field
    traverse_xml(root, &print_element);

    // Clean up
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}
