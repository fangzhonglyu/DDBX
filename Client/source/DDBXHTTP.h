//
//  DDBXHTTP.h
//  DDBX Client
//
//  HTTP request handler for the DDBX Client application.
//
//  Author: Barry Lyu
//  Version: 3/8/24
//

#ifndef __DDBX_HTTP_H__

#define __DDBX_HTTP_H__

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <iostream>
#include <cstring>

using namespace 


class HTTP {

public:
	static bool initOpenSSL() {
		SSL_load_error_strings();
		OpenSSL_add_ssl_algorithms();
	}

};

#endif