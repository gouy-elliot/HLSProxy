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

# define BUFFER_SIZE            (20 * 1024)
# define HTTP_HOST_HEADER       "\r\nHost: "
# define HTTP_HOST_HEADER_SIZE  (sizeof(HTTP_HOST_HEADER) - 1)

class Client {
private:
    const Url                   &_cdn_url;
    int                         _fd;
    int                         _cdn_fd;
    char                        _buffer[BUFFER_SIZE];

    std::chrono::time_point<std::chrono::system_clock>  _request_start;
    std::string                                         _request_protocol;
    std::string                                         _request_uri;

    bool                        print_in_request(size_t size);
public:
    Client(const Url &cdn_url, int fd);
    ~Client();

    int                         get_fd() const;
    int                         get_cdn_fd() const;
    char                        *get_buffer();

    bool                        open_cdn();
    size_t                      update_host(size_t size);
    void                        print_out_request() const;
};

#endif //HLSPROXY_CLIENT_H
