//
// Created by gouy_e on 8/16/19.
//

#include "url.h"

Url::Url(std::string protocol, std::string host, std::string port, std::string path) :
        _protocol(std::move(protocol)), _host(std::move(host)),
        _port(std::move(port)), _path(std::move(path)) {

}

/**
 * Parse a string into a Url object
 * @param url 
 * @return the newly created Url object
 */
Url Url::parse(const std::string &url) {
    std::string protocol, host, port, path;
    std::string tmp_url = url;

    // Find protocol
    size_t found = tmp_url.find("://");
    if (found != std::string::npos) {
        protocol = url.substr(0, found);
        tmp_url = tmp_url.substr(found + 3);
    } else {
        protocol = "http";
    }

    // Find path
    found = tmp_url.find_first_of('/');
    if (found != std::string::npos) {
        path = tmp_url.substr(found);
        tmp_url = tmp_url.substr(0, found);
    } else {
        path = "/";
    }

    // Find host and port
    found = tmp_url.find_first_of(':');
    if (found != std::string::npos) {
        host = tmp_url.substr(0, found);
        port = tmp_url.substr(found + 1);
    } else {
        host = tmp_url;
        port = "80";
    }
    return Url(protocol, host, port, path);
}

/**
 * Check if the url has valid components for proxy
 * @return true or false
 */
bool Url::is_valid() const {
    if (_protocol != "http") {
        return false;
    }

    try {
        int port = std::stoi(_port);
        if (port <= 0 || port > 65535) {
            return false;
        }
    } catch (...) {
        return false;
    }

    return true;
}

const std::string &Url::get_protocol() const {
    return _protocol;
}

const std::string &Url::get_host() const {
    return _host;
}

const std::string &Url::get_port() const {
    return _port;
}

const std::string &Url::get_path() const {
    return _path;
}
