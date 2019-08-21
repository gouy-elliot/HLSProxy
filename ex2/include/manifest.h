//
// Created by gouy_e on 8/18/19.
//

#ifndef HLSPROXY_MANIFEST_H
# define HLSPROXY_MANIFEST_H

# include <list>
# include <cstring>
# include <iostream>

# define MANIFEST_STREAM_TAG "#EXT-X-STREAM-INF"

class Manifest {
private:
    std::string             _left_over;
    std::list<std::string>  _streams;
    std::string             _current_stream;
    const std::string       _manifest_uri;
    int                     _client_fd;
    bool                    _is_previous_line_stream;

    void                    parse_manifest_line(const std::string &line);
    std::string             get_stream_uri(const std::string &stream_path) const;
public:
     Manifest(const std::string &request_uri, int client_fd);
    ~Manifest()             = default;

    const std::string       &get_manifest_uri() const;
    int                     get_client_fd() const;
    void                    parse_manifest(char *buffer, size_t size);
    bool                    contains_stream(const std::string &stream);
};

#endif //HLSPROXY_MANIFEST_H
