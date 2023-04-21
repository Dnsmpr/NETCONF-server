#include "network.h"
#include "KeyValuePairArray.h"
#include "parse.h"


static void my_debug(void *ctx, int level,
                     const char *file, int line,
                     const char *str)
{
    ((void) level);

    mbedtls_fprintf((FILE *) ctx, "%s:%04d: %s", file, line, str);
    fflush((FILE *) ctx);
}

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
    mbedtls_ssl_cache_context cache;

    mbedtls_net_init(&listen_fd);
    mbedtls_net_init(&client_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ssl_cache_init(&cache);
    mbedtls_x509_crt_init(&srvcert);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_pk_init(&pkey);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_debug_set_threshold(DEBUG_LEVEL);

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
    mbedtls_printf(" %d bytes read\n\n%s\n\n", len, (char *)buf); // Print the response

    KeyValuePairArray array;


    xmlInitParser();
    xmlNodePtr root = NULL;
    xmlDocPtr doc = NULL;
    int result = parse_xml((char *)buf, &root, &doc);
    if (result == -1){
        printf("ERROR PARSING");
        return -1;
    }
    // Print the type of request
    // printf("Request type: %s\n", root->name);

    // Traverse the XML tree and print the value of the field
    init_key_value_array(&array, 10);
    process_xml(&array, root);
    //set_request_type((const char*)"rpc", &array);
    print_all_nodes(&array);
    free_key_value_pair_array(&array);


    // Clean up
    xmlFreeDoc(doc);
    xmlCleanupParser();
/*
    
    if (msg_num < 7) {
        ret = write_to_client(&ssl, get_message(msg_num));
        msg_num++;
        if (ret == RESET) {
            goto reset;
        } else if (ret == EXIT) {
            goto exit;
        }
    }
    */
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
/*
int main () {
    int r;
    r = mbed_ssl_server();
    return r;
}
*/

