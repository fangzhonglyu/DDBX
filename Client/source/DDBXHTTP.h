////
////  DDBXHTTP.h
////  DDBX Client
////
////  HTTP request handler for the DDBX Client application.
////
////  Author: Barry Lyu
////  Version: 3/8/24
////
//
//#ifndef __DDBX_HTTP_H__
//#define __DDBX_HTTP_H__
//
//#include <openssl/ssl.h>
//#include <openssl/err.h>
//#include <openssl/bio.h>
//#include <iostream>
//#include <cstring>
//#include <tcptransport.hpp>
//#include <httpproxytransport.hpp>
//#include <http.hpp>
//#include <plog/Log.h>
//#include <cugl/cugl.h>   
//
//using namespace rtc::impl;
//using namespace cugl;
//
//class HTTP {
//    
//public:
//    enum Method {
//        GET,
//        POST,
//        PUT,
//        DELETE
//    };
//    
//    class Auth {
//    public:
//        enum AuthType{
//            NoAuth,
//            Bearer
//        };
//    
//        AuthType authType;
//        std::string token;
//    };
//
//    
//private:
//    std::string _host, _service;
//    Transport::state_callback _callback;
//    const SSL_METHOD* _method;
//    std::shared_ptr<TcpTransport> _tcp;
//    std::shared_ptr<HttpProxyTransport> _http;
//    bool _valid;
//    
//    void stateCallback(Transport::State state);
//    
//    void receiveCallback(rtc::message_ptr msg);
//
//public:
//    bool initOpenSSL();
//    
//    bool initHTTPTransport(std::string host, std::string service);
//    
//    void sendHTTPRequest(rtc::message_ptr message);
//    
//    rtc::message_ptr makeRequest(Method method, Auth auth, std::string endPoint, std::string body);
//    
//    rtc::message_ptr makeRequest(Method method, Auth auth, std::string endPoint, std::shared_ptr<JsonValue> body){
//        return makeRequest(method, auth, endPoint, body->toString());
//    }
//    
//    void sendLogin(std::string username, std::string password);
//
//};
//
//#endif /* __DDBH_HTTP_H__ */
