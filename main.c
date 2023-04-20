#include "parse.h"
#include "network.h"
#include "parsing/DataStructures/KeyValuePairArray.h"



int main()
{
    KeyValuePairArray array;
    init_key_value_array(&array, 7);
    KeyValuePair kv;
    char* ip = "IP_ADDRESS";
    char* ip_value = "192.168.1.1";
    init_key_value_pair(&kv, ip, ip_value, strlen(ip_value) + 1);
    add_key_value(&array, &kv);
    print_all_nodes(&array);
    printf("\n");

    

    free_key_value_pair_array(&array);
    // Initialize the library
    xmlInitParser();

    // Parse the XML request
    xmlNodePtr root = NULL;
    xmlDocPtr doc = NULL;
    int result = parse_xml(NETCONF_HELLO, &root, &doc);
    if (result == -1){
        printf("ERROR PARSING");
        return -1;
    }
    // Print the type of request
    // printf("Request type: %s\n", root->name);

    // Traverse the XML tree and print the value of the field
    init_key_value_array(&array, 7);
    process_xml(&array, root);
    //set_request_type((const char*)"rpc", &array);
    print_all_nodes(&array);
    free_key_value_pair_array(&array);


    // Clean up
    xmlFreeDoc(doc);
    xmlCleanupParser();

    
    mbed_ssl_server();

    return 0;
}
