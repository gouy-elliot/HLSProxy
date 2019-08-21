//
// Created by gouy_e on 8/19/19.
//

#include "manifest_parser.h"

ManifestParser::ManifestParser(const Url &cdn_url) : _cdn_url(cdn_url) {

}

ManifestParser::~ManifestParser() {
    auto it = _manifests.begin();
    while (it != _manifests.end()) {
        delete (*it);
        _manifests.erase(it++);
    }
}

/**
 * Get the corresponding manifest object from the request uri
 * Create a new manifest if it doesn't exists
 * @param request_uri
 * @param client_fd
 * @return the manifest object
 */
Manifest *ManifestParser::get_request_manifest(const std::string &request_uri, int client_fd) {
    std::list<Manifest *>::iterator it;
    for (it = _manifests.begin(); it != _manifests.end(); ++it) {
        if ((*it)->get_manifest_uri() == request_uri) {
            return *it;
        }
    }

    auto *new_manifest = new Manifest(_cdn_url, request_uri, client_fd);
    _manifests.push_back(new_manifest);
    return new_manifest;
}

/**
 * Remove HTTP header from the buffer before parsing the manifest
 * @param client
 * @param buffer
 * @param size
 * @return the new size of the buffer
 */
size_t ManifestParser::parse_manifest(const Client *client, char *buffer, size_t size) {

    std::string extension = client->get_request_extension();
    if (extension != MANIFEST_EXTENSION) {
        return size;
    }

    Manifest *current_manifest = get_request_manifest(client->get_request_uri(), client->get_fd());
    int i = 0;

    while (size > 3 && i < size - 3) {
        if (buffer[i] == '\r' &&
            buffer[i + 1] == '\n' &&
            buffer[i + 2] == '\r' &&
            buffer[i + 3] == '\n') {

            int body_start = i + 4;
            return current_manifest->parse_manifest(buffer + body_start, size - body_start) + body_start;
        }
        ++i;
    }
    return current_manifest->parse_manifest(buffer, size);
}

/**
 * Check if any loaded manifest contains a reference to the request made by the client
 * @param client
 */
void ManifestParser::check_loaded_manifest(const Client *client) const {

    std::string extension = client->get_request_extension();
    if (extension != MANIFEST_EXTENSION) {
        return;
    }

    const std::string &request_uri = client->get_request_uri();
    for (auto const &it : _manifests) {
        if (it->contains_stream(request_uri)) {
        }
    }
}

/**
 * Clear all manifest loaded for a client
 * @param client
 */
void ManifestParser::clear(const Client *client) {
    auto it = _manifests.begin();
    while (it != _manifests.end()) {
        if ((*it)->get_client_fd() == client->get_fd()) {
            delete (*it);
            _manifests.erase(it++);
        } else {
            ++it;
        }
    }
}
