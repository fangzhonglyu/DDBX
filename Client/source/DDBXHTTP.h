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

#include <cugl/cugl.h>  
#include <cpr/cpr.h>

#define login_body(username,password) ("{\"username\":\"" + (username) + "\",\"password\":\"" + (password) + "\"}")

using namespace rtc::impl;
using namespace cugl;

class HTTP {
    
public:

    
private:
    std::string _backend_url;
    bool _isLoggedIn;
    std::string _authToken;
    std::string _uuid;
    
    std::queue<cpr::AsyncResponse> _responseQueue;
    
    cpr::AsyncWrapper<bool> login(std::string username, std::string password){
        return cpr::PostCallback(
            [this](cpr::Response r) {
                if(r.status_code == 200){
                    auto json = JsonValue::allocWithJson(r.text);
                    _authToken = json->getString("token");
                    _uuid = json->getString("uuid");
                    _isLoggedIn = true;
                    return true;
                }
                else{
                    _isLoggedIn = false;
                    return false;
                }
            }, 
            cpr::Url{_backend_url + "/login"},
            cpr::Body(login_body(username, password)),
            cpr::Header{{"Content-Type", "application/json"}});
    }
    
    cpr::AsyncWrapper<int> getBalance(){
        // Perform the request like usually:
        return cpr::GetCallback(
            [](cpr::Response r) {
                if(r.status_code == 200){
                    auto json = JsonValue::allocWithJson(r.text);
                    return json->getInt("balance");
                } else{
                    return -10000;
                }
            }, 
            cpr::Url{_backend_url},
            cpr::Bearer{_authToken}
        );
    }
    
    cpr::AsyncWrapper<bool> setBalance(int balance){
        return cpr::PostCallback(
            [](cpr::Response r) {
                return r.status_code == 200;
            },
            cpr::Url{_backend_url + "/balance"},
            cpr::Body("{\"balance\":" + std::to_string(balance) + "}"),
            cpr::Header{{"Content-Type", "application/json"}, {"Authorization", "Bearer " + _authToken}}
        );
    }

};

#endif /* __DDBH_HTTP_H__ */
