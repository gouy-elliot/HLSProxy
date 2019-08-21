//
// Created by gouy_e on 8/16/19.
//

#ifndef HLSPROXY_PROXY_H
# define HLSPROXY_PROXY_H

# include <list>
# include <cstring>
# include <csignal>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <zconf.h>
# include "url.h"
# include "client.h"

# define PROXY_PORT         8080

class Proxy {
private:
    static volatile bool    _stop_signal;
    const Url               &_cdn_url;
    int                     _server_fd;
    std::list<Client *>     _clients;

    int                     init_read_set(fd_set *read_set) const;
    bool                    init_signal() const;
    static void             handle_signal(int signal);
    bool                    accept_client();
public:
    explicit Proxy(const Url &cdn_url);
    ~Proxy();

    bool                    open_server();
    void                    launch();
};

#endif //HLSPROXY_PROXY_H
