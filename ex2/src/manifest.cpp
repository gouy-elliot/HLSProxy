//
// Created by gouy_e on 8/18/19.
//

#include "manifest.h"

Manifest::Manifest(const std::string &request_uri, int client_fd) :
_manifest_uri(request_uri),
_is_previous_line_stream(false),
_client_fd(client_fd) {

}

/**
 * Parse and save the stream line in the manifest
 * @param line
 */
void Manifest::parse_manifest_line(const std::string &line) {
    if (_is_previous_line_stream) {
        _streams.push_back(line);
        _is_previous_line_stream = false;
    } else if (line[0] == '#') {
        size_t found = line.find_first_of(':');
        if (found != std::string::npos) {
            std::string tag = line.substr(0, found);
            if (tag == MANIFEST_STREAM_TAG) {
                _is_previous_line_stream = true;
            }
        }
    }
}

/**
 * Compute the url of a stream using the request url
 * @param stream_path
 * @return the stream url
 */
std::string Manifest::get_stream_uri(const std::string &stream_path) const {
    std::string stream_uri = _manifest_uri;
    size_t found = stream_uri.find_last_of('/');

    if (found != std::string::npos) {
        stream_uri = stream_uri.substr(0, found + 1) + stream_path;
        return stream_uri;
    }
    return stream_path;
}

const std::string &Manifest::get_manifest_uri() const {
    return _manifest_uri;
}

int Manifest::get_client_fd() const {
    return _client_fd;
}

/**
 * Rebuild the manifest line by line from the buffer
 * @param buffer
 * @param size
 */
void Manifest::parse_manifest(char *buffer, size_t size) {
    int i = 0;
    int begin_of_line = 0;
    while (i < size) {
        if (buffer[i] == '\n') {
            int line_size = i - begin_of_line;
            std::string line = std::string(buffer + begin_of_line, line_size);

            if (begin_of_line == 0 && !_left_over.empty()) {
                _left_over.append(line);
                line = _left_over;
                _left_over.clear();
            }

            parse_manifest_line(line);
            begin_of_line = i + 1;
        }
        ++i;
    }

    if (begin_of_line < i) {
        _left_over = std::string(buffer + begin_of_line, i - begin_of_line);
    }
}

/**
 * Check if the manifest contains a reference to the given uri
 * Print the track switch if a new stream is loaded
 * @param request_uri
 * @return true if a new stream has been loaded, false otherwise
 */
bool Manifest::contains_stream(const std::string &request_uri) {
    for (auto const &it : _streams) {
        std::string stream_uri = get_stream_uri(it);
        if (request_uri == stream_uri) {
            if (!_current_stream.empty() && _current_stream != stream_uri) {
                std::cout << "[TRACK SWITCH]" << std::endl;
            }
            _current_stream = stream_uri;
            return true;
        }
    }
    return false;
}
