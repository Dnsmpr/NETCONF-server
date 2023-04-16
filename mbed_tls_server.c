/*
 *  network.c : netconf daemon network layer.
 *  Copyright (C) 2003 - Madynes Research Team - LORIA, FR
 *  Author : Benjamin Zores  <benjamin.zores@loria.fr>
 *  
 *   This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *   This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *   You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* 
 * Provides a threaded OpenSSL server with non-blocking connection handling
 * Creating new keys :
 *   - Private Key
 *     openssl genrsa -out server.key 2048
 *   - Certificate
 *     openssl req -new -x509 -key server.key -out server.cert -days 365
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>


/*** MBEDTLS */
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

#define SERVER_PORT "6513" // Default port according to RFC 7589
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
  "<capability>urn:hms:abcc?module=abcc&amp;revision=2000-01-01</capability>"\
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
       "revision 2000-01-01;"\
       "container attributes {"\
           "description \"Attributes of the abcc\";"\
           "config true;"\
           "leaf name {"\
               "mandatory true;"\
               "type string;"\
           "}"\
           "leaf age {"\
               "type uint8;"\
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
    "<revision>2000-01-01</revision>"\
    "<namespace>urn:hms:abcc</namespace>"\
    "<conformance-type>implement</conformance-type>"\
    "<submodule></submodule>"\
    "<feature></feature>"\
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
     "<version>2000-01-01</version>"\
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
        "<name>Abcc</name>"\
        "<age>30</age>"\
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
"</rpc-reply>"


static void my_debug(void *ctx, int level,
                     const char *file, int line,
                     const char *str)
{
    ((void) level);

    mbedtls_fprintf((FILE *) ctx, "%s:%04d: %s", file, line, str);
    fflush((FILE *) ctx);
}


#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"

#define RESET -1
#define EXIT -2

int write_to_client(mbedtls_ssl_context* ssl, const char* hello_string) {
    mbedtls_printf("  > Write to client:");
    fflush(stdout);

    unsigned char buf[1024];
    int len = sprintf((char *)buf, hello_string,
                      mbedtls_ssl_get_ciphersuite(ssl));

    int ret;
    while ((ret = mbedtls_ssl_write(ssl, buf, len)) <= 0) {
        if (ret == MBEDTLS_ERR_NET_CONN_RESET) {
            mbedtls_printf(" failed\n  ! peer closed the connection\n\n");
            return RESET;
        }

        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
            return EXIT;
        }
    }

    len = ret;
    mbedtls_printf(" %d bytes written\n\n%s\n", len, (char *)buf);

    return 0;
}

const char* get_message(int msg_num) {
    switch (msg_num) {
        case 0:
            return NETCONF_HELLO;

        case 1:
            return NETCONF_RESPONSE_1;
        
        case 2:
            return NETCONF_RESPONSE_2;

        case 3:
            return NETCONF_RESPONSE_3;

        case 4:
            return NETCONF_RESPONSE_4;

        case 5:
            return NETCONF_RESPONSE_5;

        case 6:
            return NETCONF_RESPONSE_6;

        default:
            mbedtls_printf("Invalid msg_num: %d\n", msg_num);
            return "";
    }
}



int mbed_ssl_server() {
  int ret, len;
    mbedtls_net_context listen_fd, client_fd;
    unsigned char buf[1024];
    const char *pers = "ssl_server";

    mbedtls_entropy_context entropy; // entropy source
    mbedtls_ctr_drbg_context ctr_drbg; // random generator
    mbedtls_ssl_context ssl;    // ssl context
    mbedtls_ssl_config conf;    // ssl config
    mbedtls_x509_crt srvcert;   // server certificate
    mbedtls_x509_crt cacert;
    mbedtls_pk_context pkey;    // private key
#if defined(MBEDTLS_SSL_CACHE_C) 
    mbedtls_ssl_cache_context cache;
#endif

    mbedtls_net_init(&listen_fd);
    mbedtls_net_init(&client_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_cache_init(&cache);
#endif
    mbedtls_x509_crt_init(&srvcert);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_pk_init(&pkey);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif

    /** 1. Seed the RNG */
    mbedtls_printf("  . Seeding the random number generator...");
    fflush(stdout);

    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                     (const unsigned char *) pers,
                                     strlen(pers))) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
        goto exit;
    }

    mbedtls_printf(" ok\n");

    /** 2. Load the certificates and private RSA key */
    mbedtls_printf("\n  . Loading the server cert. and key...");
    fflush(stdout);

    char *server_cert = SERVER_CERTIFICATE_FILE;
    char *server_key = SERVER_KEY_FILE;
    char *ca_cert = CA_CERTIFICATE_FILE;
    /*
     * This demonstration program uses embedded test certificates.
     * Instead, you may want to use mbedtls_x509_crt_parse_file() to read the
     * server and CA certificates, as well as mbedtls_pk_parse_keyfile().
     */

    ret = mbedtls_x509_crt_parse_file(&cacert, ca_cert);
    if (ret != 0) {
        mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse ca_cert returned %d\n\n", ret);
        goto exit;
    }

    ret = mbedtls_x509_crt_parse_file(&srvcert, server_cert);
    if (ret != 0) {
        mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse server_cert returned %d\n\n", ret);
        goto exit;
    }

    ret =  mbedtls_pk_parse_keyfile(&pkey, server_key, NULL);
    if (ret != 0) {
        mbedtls_printf(" failed\n  !  mbedtls_pk_parse_key server_key returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_printf(" ok\n");

    /** 3. Setup the listening TCP socket */

    mbedtls_printf("  . Bind on https://localhost:/6513 ...");
    fflush(stdout);

    if ((ret = mbedtls_net_bind(&listen_fd, NULL, SERVER_PORT, MBEDTLS_NET_PROTO_TCP)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_net_bind returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_printf(" ok\n");

    /*
     * 4. Setup stuff
     */
    mbedtls_printf("  . Setting up the SSL data....");
    fflush(stdout);

    if ((ret = mbedtls_ssl_config_defaults(&conf,
                                           MBEDTLS_SSL_IS_SERVER,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_conf_session_cache(&conf, &cache,
                                   mbedtls_ssl_cache_get,
                                   mbedtls_ssl_cache_set);
#endif
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL); // Set the CA certificate chain
    if ((ret = mbedtls_ssl_conf_own_cert(&conf, &srvcert, &pkey)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret);
        goto exit;
    }

    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_printf(" ok\n");

reset:
#ifdef MBEDTLS_ERROR_C
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, 100);
        mbedtls_printf("Last error was: %d - %s\n\n", ret, error_buf);
    }
#endif

    mbedtls_net_free(&client_fd);

    mbedtls_ssl_session_reset(&ssl);

    /** 5. Wait until a client connects */
    mbedtls_printf("  . Waiting for a remote connection ...");
    fflush(stdout);

    if ((ret = mbedtls_net_accept(&listen_fd, &client_fd,
                                  NULL, 0, NULL)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_net_accept returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl, &client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    mbedtls_printf(" ok\n");

    /** 6. Handshake */
    mbedtls_printf("  . Performing the SSL/TLS handshake...");
    fflush(stdout);

    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned %d\n\n", ret);
            goto reset;
        }
    }

    mbedtls_printf(" ok\n");

    /** Send the NETCONF response immediately after handshake 
    ret = write_to_client(&ssl, NETCONF_HELLO);
    if (ret == RESET) {
        goto reset;
    } else if (ret == EXIT) {
        goto exit;
    }*/

    /** 7. Read the HTTP Request */
/** Continuously read from the client */
int msg_num = 0;
while (1) {
    mbedtls_printf("\n\n  < Read from client:");
    fflush(stdout);

    len = sizeof(buf) - 1; // Leave space for null terminator
    memset(buf, 0, sizeof(buf)); // Clear the buffer
    ret = mbedtls_ssl_read(&ssl, buf, len); // Read from the client

    if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        continue;
    }

    if (ret <= 0) {
        switch (ret) {
            case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                mbedtls_printf(" connection was closed gracefully\n");
                break;

            case MBEDTLS_ERR_NET_CONN_RESET:
                mbedtls_printf(" connection was reset by peer\n");
                break;

            default:
                mbedtls_printf(" mbedtls_ssl_read returned -0x%x\n", (unsigned int)-ret);
                break;
        }

        break;
    }

    len = ret; // The number of bytes read
    mbedtls_printf(" %d bytes read\n\n%s", len, (char *)buf); // Print the response
    
    if (msg_num < 7) {
        ret = write_to_client(&ssl, get_message(msg_num));
        msg_num++;
        if (ret == RESET) {
            goto reset;
        } else if (ret == EXIT) {
            goto exit;
        }
    }
}


    mbedtls_printf("  . Closing the connection...");

    while ((ret = mbedtls_ssl_close_notify(&ssl)) < 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
            ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_close_notify returned %d\n\n", ret);
            goto reset;
        }
    }

    mbedtls_printf(" ok\n");

    ret = 0;
    goto reset;

exit:

#ifdef MBEDTLS_ERROR_C
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, 100);
        mbedtls_printf("Last error was: %d - %s\n\n", ret, error_buf);
    }
#endif

    mbedtls_net_free(&client_fd);
    mbedtls_net_free(&listen_fd);

    mbedtls_x509_crt_free(&srvcert);
    mbedtls_pk_free(&pkey);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_cache_free(&cache);
#endif
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

    mbedtls_exit(ret);
}




/* start a new server
 * int ssl_support : boolean to determine whether to use SSL encryption or not
 * int port : port to bind server
 */
int main () {
    int r;
    r = mbed_ssl_server();
    return r;
}
