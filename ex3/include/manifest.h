//
// Created by gouy_e on 8/18/19.
//

#ifndef HLSPROXY_MANIFEST_H
# define HLSPROXY_MANIFEST_H

# include <list>
# include <cstring>
# include <iostream>
# include "proxy_config.h"
# include "url.h"

# define MANIFEST_STREAM_TAG    "#EXT-X-STREAM-INF:"
# define MANIFEST_SEGMENT_TAG   "#EXTINF:"
# define MANIFEST_MEDIA_TAG     "#EXT-X-MEDIA:"

class Manifest {
private:
    std::string             _left_over;
    std::list<std::string>  _streams;
    std::string             _current_stream;
    const std::string       _manifest_uri;
    int                     _client_fd;
    bool                    _is_previous_line_stream;
    bool                    _is_left_over_sent;
    const Url               &_cdn_url;

    bool                    parse_manifest_line(const std::string &line);
    std::string             get_stream_uri(const std::string &stream_path) const;
    size_t                  rewrite_line(const std::string &line, char *buffer, int size, int begin_of_line);
    size_t                  rewrite_media_line(const std::string &line, char *buffer, int size, int begin_of_line);
public:
     Manifest(const Url &cdn_url, const std::string &request_uri, int client_fd);
    ~Manifest()             = default;

    const std::string       &get_manifest_uri() const;
    int                     get_client_fd() const;
    size_t                  parse_manifest(char *buffer, size_t size);
    bool                    contains_stream(const std::string &stream);
};

#endif //HLSPROXY_MANIFEST_H
