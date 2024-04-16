////
////  DDBXHTTP.cpp
////  DDBX Client
////
////  HTTP request handler for the DDBX Client application.
////
////  Author: Barry Lyu
////  Version: 3/8/24
////
//
//#include "DDBXHTTP.h"
//
//bool HTTP::initOpenSSL(){
//    SSL_load_error_strings();
//    OpenSSL_add_ssl_algorithms();
//    
//    _method = TLS_client_method();
//    SSL_CTX* ctx = SSL_CTX_new(_method);
//    if (!ctx) {
//        CULog("Cannot create SSL context");
//        _method = nullptr;
//        return false;
//    }
//
//        // Create a connection
//    BIO* bio = BIO_new_ssl_connect(ctx);
//    SSL* ssl;
//    BIO_get_ssl(bio, &ssl);
//    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
//    return true;
//}
//
//void HTTP::stateCallback(Transport::State state){
//    if(state == Transport::State::Connected || state == Transport::State::Completed){
//        _valid = true;
//    }
//    else{
//        _valid = false;
//    }
//}
//
//void HTTP::receiveCallback(rtc::message_ptr message){
//    if(isHttpRequest(message->data(),message->size())){
//        CULog("%s", reinterpret_cast<const char*>(message->data()));
//        
//    } else{
//        CULog("Received NON-HTTP message.");
//    }
//}
//
//bool HTTP::initHTTPTransport(std::string host, std::string service){
//    _host = host;
//    _service = service;
//    
//    Transport::state_callback stateCb = [=](Transport::State state) { this->stateCallback(state); };
//    
//    rtc::message_callback recvCb = [=](rtc::message_ptr message) { this->receiveCallback(message); };
//
//    _tcp = std::make_shared<rtc::impl::TcpTransport>(host,service,nullptr);
//    
//    _tcp->start();
//    
//    
//    
//    _tcp->onRecv(recvCb);
//    
//    while(_tcp->state() != rtc::impl::Transport::State::Connected){
//    }
//    
////
//    
//    _http = std::make_shared<rtc::impl::HttpProxyTransport>(_tcp, host,service, stateCb);
//    _http->onRecv(recvCb);
//    _http->start();
//    
//    
// 
//    return true;
//}
//
//void HTTP::sendHTTPRequest(rtc::message_ptr message){
//    _http->send(message);
//}
//
//
// 
//// POST /test_player_info/login HTTP/1.1
//// Content-Type: application/json
//// User-Agent: PostmanRuntime/7.37.0
//// Accept: */*
//// Postman-Token: f60a1a8d-fb51-4725-9950-4a7f1c5de7e6
//// Host: us-east4-ddbx-soxehli.cloudfunctions.net
//// Accept-Encoding: gzip, deflate, br
//// Connection: keep-alive
//// Content-Length: 61
////  
//// {
//// "username":"soxehli",
//// "password":"barry030707"
//// }
// 
//
//rtc::message_ptr HTTP::makeRequest(Method method, Auth auth, std::string endPoint, std::string body){
//    std::string request;
//    switch(method){
//        case Method::GET:
//            request = "GET";
//            break;
//        case Method::POST:
//            request = "POST";
//            break;
//        case Method::PUT:
//            request = "PUT";
//            break;
//        case Method::DELETE:
//            request = "DELETE";
//            break;
//        default:
//            CUAssertLog(false, "Unsupported HTTP method");
//    }
//    
//    request += " " + endPoint;
//    request += " HTTP/1.1\r\n";
//    request += "Content-Type: application/json\r\n";
//    if(auth.authType == Auth::Bearer){
//        request += "Authorization: Bearer " + auth.token + "\r\n";
//    }
//    request += "User-Agent: DDBXClient r\n";
//    request += "Accept: */*\r\n";
//    request += "Host: " + _host + "\r\n";
//    request += "Accept-Encoding: gzip, deflate, br\r\n";
//    request += "Connection: keep-alive\r\n";
//    request += "Content-Length: " + std::to_string(body.size()) + "\r\n";
//    request += "\r\n";
//    request += body;
//    	
//    return rtc::make_message(request);
//}
//
//void HTTP::sendLogin(std::string username, std::string password){
//    std::string body = "{\n\"username\":\"" + username + "\",\n\"password\":\"" + password + "\"\n}";
//    rtc::message_ptr message = makeRequest(Method::POST, Auth(), "/test_player_info/login", body);
//    sendHTTPRequest(message);
//}
