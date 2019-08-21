//
// Created by gouy_e on 8/16/19.
//

#ifndef HLSPROXY_URL_H
# define HLSPROXY_URL_H

# include <string>
# include <utility>

class Url {
private:
    const std::string   _protocol;
    const std::string   _host;
    const std::string   _port;
    const std::string   _path;

    Url(std::string protocol,
        std::string host,
        std::string port,
        std::string path);
public:
    ~Url()              = default;
    bool                is_valid() const;
    const std::string   &get_protocol() const;
    const std::string   &get_host() const;
    const std::string   &get_port() const;
    const std::string   &get_path() const;

    static Url          parse(const std::string &url);
};

#endif //HLSPROXY_URL_H
