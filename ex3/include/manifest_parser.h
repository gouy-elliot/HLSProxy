//
// Created by gouy_e on 8/19/19.
//

#ifndef HLSPROXY_MANIFEST_PARSER_H
# define HLSPROXY_MANIFEST_PARSER_H

# include <list>
# include <cstring>
# include "client.h"
# include "manifest.h"
# include "url.h"

class ManifestParser {
private:
    std::list<Manifest *>               _manifests;
    const Url                           &_cdn_url;

    Manifest                            *get_request_manifest(const std::string &request_uri, int client_fd);
public:
    explicit ManifestParser(const Url &cdn_url);
    ~ManifestParser();

    size_t                              parse_manifest(const Client *client, char *buffer, size_t size);
    void                                check_loaded_manifest(const Client *client) const;
    void                                clear(const Client *client);
};

#endif //HLSPROXY_MANIFEST_PARSER_H
