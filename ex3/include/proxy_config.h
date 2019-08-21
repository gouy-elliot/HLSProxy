//
// Created by gouy_e on 8/21/19.
//

#ifndef HLSPROXY_PROXY_CONFIG_H
# define HLSPROXY_PROXY_CONFIG_H

# define PROXY_PORT         8080
# define PROXY_HOST         "http://127.0.0.1:8080"
# define PROXY_BUFFER_SIZE  (20 * 1024)
# define PROXY_READ_SIZE    (PROXY_BUFFER_SIZE - 1024 * 3) // We leave space at the end of buffer to rewrite URLs

#endif //HLSPROXY_PROXY_CONFIG_H
