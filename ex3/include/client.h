//
// Created by gouy_e on 8/18/19.
//

#ifndef HLSPROXY_CLIENT_H
# define HLSPROXY_CLIENT_H

# include <cstring>
# include <chrono>
# include <iostream>
# include <zconf.h>
# include <netdb.h>
# include <netinet/in.h>
# include "url.h"
# include "proxy_config.h"

# define HTTP_HOST_HEADER       "\r\nHost: "
# define HTTP_HOST_HEADER_SIZE  (sizeof(HTTP_HOST_HEADER) - 1)
# define MANIFEST_EXTENSION     ".m3u8"
# define SEGMENT_EXTENSION      ".ts"

class Client {
private:
    const Url                   &_cdn_url;
    int                         _fd;
    int                         _cdn_fd;
    char                        _buffer[PROXY_BUFFER_SIZE];

    std::chrono::time_point<std::chrono::system_clock>  _request_start;
    std::string                                         _request_protocol;
    std::string                                         _request_uri;
public:
    Client(const Url &cdn_url, int fd);
    ~Client();

    int                         get_fd() const;
    int                         get_cdn_fd() const;
    std::string                 get_request_extension() const;
    const std::string           &get_request_uri() const;
    char                        *get_buffer();

    bool                        open_cdn();
    size_t                      update_host(size_t size);
    bool                        parse_in_request(size_t size);
    void                        print_in_request() const;
    void                        print_out_request() const;
};

#endif //HLSPROXY_CLIENT_H
