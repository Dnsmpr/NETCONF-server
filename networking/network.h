#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "mbedtls/platform.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"

#if defined(MBEDTLS_SSL_CACHE_C)
#include "mbedtls/ssl_cache.h"
#endif

#define DEBUG_LEVEL 0
#define RESET -1
#define EXIT -2
#define SERVER_PORT "6513"
#define SERVER_CERTIFICATE_FILE "/home/kris/certs/server.crt"
#define SERVER_KEY_FILE "/home/kris/certs/server.key"
#define CA_CERTIFICATE_FILE "/home/kris/certs/ca.crt"

#define NETCONF_HELLO \
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"\
"<hello xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"\
 "<capabilities>"\
  "<capability>urn:ietf:params:netconf:base:1.0</capability>"\
  "<capability>urn:ietf:params:netconf:capability:writable-running:1.0</capability>"\
  "<capability>urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring</capability>"\
  "<capability>urn:hms:abcc?module=abcc&amp;revision=2002-01-01</capability>"\
 "</capabilities>"\
 "<session-id>4</session-id>"\
"</hello>]]>]]>"

#define NETCONF_RESPONSE_1 \
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"\
"<rpc-reply xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\" message-id=\"1\">"\
 "<data xmlns=\"urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring\">"\
   "module abcc {"\
       "yang-version 1.1;"\
       "namespace \"urn:hms:abcc\";"\
       "prefix \"abcc\";"\
       "revision 2002-01-01;"\
       "container attributes {"\
           "description \"Attributes of the abcc\";"\
           "config true;"\
           "leaf IP_ADDRESS {"\
               "mandatory true;"\
               "type string;"\
           "}"\
       "}"\
   "}"\
 "</data>"\
"</rpc-reply>]]>]]>"


#define NETCONF_RESPONSE_2 \
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"\
"<rpc-reply xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\" message-id=\"2\">"\
 "<data>"\
  "<modules-state xmlns=\"urn:ietf:params:xml:ns:yang:ietf-yang-library\">"\
   "<module>"\
    "<name>abcc</name>"\
    "<revision>2002-01-01</revision>"\
    "<namespace>urn:hms:abcc</namespace>"\
    "<conformance-type>implement</conformance-type>"\
    "<submodule></submodule>"\
    "<feature>IP_ADDRESS</feature>"\
    "<deviation></deviation>"\
   "</module>"\
  "</modules-state>"\
 "</data>"\
"</rpc-reply>]]>]]>"


#define NETCONF_RESPONSE_3 \
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"\
"<rpc-reply xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\" message-id=\"3\">"\
 "<data>"\
  "<netconf-state xmlns=\"urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring\">"\
   "<schemas>"\
    "<schema>"\
     "<identifier>abcc</identifier>"\
     "<version>2002-01-01</version>"\
     "<format>yang</format>"\
     "<namespace>urn:hms:abcc</namespace>"\
     /*"<location>NETCONF</location>"*/\
    "</schema>"\
   "</schemas>"\
  "</netconf-state>"\
 "</data>"\
"</rpc-reply>]]>]]>"

#define NETCONF_RESPONSE_4 \
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"\
"<rpc-reply message-id=\"4\" xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"\
  "<data>"\
    "<abcc xmlns=\"urn:hms:abcc\">"\
      "<attributes>"\
        "<IP_ADDRESS>192.168.1.150</IP_ADDRESS>"\
      "</attributes>"\
    "</abcc>"\
  "</data>"\
"</rpc-reply>]]>]]>"


#define NETCONF_RESPONSE_5 \
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"\
"<rpc-reply message-id=\"5\" xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"\
  "<data>"\
    "<abcc xmlns=\"urn:hms:abcc\">"\
      "<attributes>"\
        "<name>Abcc</name>"\
        "<age>30</age>"\
      "</attributes>"\
    "</abcc>"\
  "</data>"\
"</rpc-reply>]]>]]>"\

#define NETCONF_RESPONSE_6 \
"<rpc-reply message-id=\"6\" xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"\
"<ok/>"\
"</rpc-reply>]]>]]>"

int write_to_client(mbedtls_ssl_context* ssl, const char* hello_string);
const char* get_message(int msg_num);
int mbed_ssl_server();

#endif // NETWORK_H
