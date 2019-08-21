//
// Created by gouy_e on 8/18/19.
//

#include "manifest.h"

Manifest::Manifest(const Url &cdn_url, const std::string &request_uri, int client_fd) :
        _manifest_uri(request_uri),
        _is_previous_line_stream(false),
        _is_left_over_sent(true),
        _cdn_url(cdn_url),
        _client_fd(client_fd) {

}

/**
 * Parse and save the stream line in the manifest
 * @param line
 * @return true if the current line need to be updated, false otherwise
 */
bool Manifest::parse_manifest_line(const std::string &line) {
    if (_is_previous_line_stream) {
        _streams.push_back(line);
        _is_previous_line_stream = false;
        return true;
    } else if (line[0] == '#') {
        if (line.rfind(MANIFEST_STREAM_TAG, 0) == 0 || line.rfind(MANIFEST_SEGMENT_TAG, 0) == 0) {
            _is_previous_line_stream = true;
        } else if (line.rfind(MANIFEST_MEDIA_TAG, 0) == 0) {
            return true;
        }
    }
    return false;
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

/**
 * Update the line in the buffer to change Url in the manifest to pass by our proxy
 * Update only if the url host match our proxy host (we only proxy one server)
 * @param line
 * @param buffer
 * @param size
 * @param begin_of_line
 * @return the size difference between the new and old line
 */
size_t Manifest::rewrite_line(const std::string &line, char *buffer, int size, int begin_of_line) {
    if (line.rfind("http://", 0) == 0 || line.rfind("https://", 0) == 0) {
        Url url = Url::parse(line);
        if (url.get_host() == _cdn_url.get_host()) {
            std::string new_line = PROXY_HOST + url.get_path();

            memcpy(buffer + begin_of_line + new_line.size(), buffer + begin_of_line + line.size(), size - (begin_of_line + line.size()));
            memcpy(buffer + begin_of_line, new_line.c_str(), new_line.size());
            return new_line.size() - line.size();
        }
    } else if (line.rfind(MANIFEST_MEDIA_TAG, 0) == 0) {
        return rewrite_media_line(line, buffer, size, begin_of_line);
    }
    return 0;
}

/**
 * Rewrite a line containing a media definition (#EXT-X-MEDIA)
 * @param line
 * @param buffer
 * @param size
 * @param begin_of_line
 * @return the size difference between the new and old line
 */
size_t Manifest::rewrite_media_line(const std::string &line, char *buffer, int size, int begin_of_line) {
    int i = line.find_first_of(':');
    if (i == std::string::npos) {
        i = -1;
    }

    std::string result = MANIFEST_MEDIA_TAG;
    result += ":";
    int begin_of_param = i + 1;
    while (++i <= line.size()) {
        if (line[i] == ',' || line[i] == 0) {
            std::string param = line.substr(begin_of_param, i - begin_of_param);
            if (param.rfind("URI=\"", 0) == 0) {
                std::string uri_value = std::string(buffer + begin_of_line + begin_of_param + 5, param.size() - 6);
                return rewrite_line(uri_value, buffer, size, begin_of_line + begin_of_param + 5);
            }
            begin_of_param = i + 1;
        }
    }
    return 0;
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
  * @return the new size of the buffer
  */
size_t Manifest::parse_manifest(char *buffer, size_t size) {

    // copy first piece on line received in a previous read
    if (!_left_over.empty() && !_is_left_over_sent) {
        memcpy(buffer + _left_over.size(), buffer, size);
        memcpy(buffer, _left_over.c_str(), _left_over.size());
        size += _left_over.size();
        _left_over.clear();
        _is_left_over_sent = true;
    }

    int i = 0;
    int begin_of_line = 0;
    while (i < size) {
        if (buffer[i] == '\n') {
            int line_size = i - begin_of_line;
            std::string line = std::string(buffer + begin_of_line, line_size);

            bool is_from_left_over = false;
            if (begin_of_line == 0 && !_left_over.empty()) {
                _left_over.append(line);
                line = _left_over;
                _left_over.clear();
                is_from_left_over= true;
            }

            bool need_rewrite = parse_manifest_line(line);
            if (need_rewrite && !is_from_left_over) {
                size_t size_diff = rewrite_line(line, buffer, size, begin_of_line);
                size += size_diff;
                i += size_diff;
            }
            begin_of_line = i + 1;
        }
        ++i;
    }

    if (begin_of_line < i) {
        _left_over = std::string(buffer + begin_of_line, i - begin_of_line);
        if (_is_previous_line_stream) {
            // last line might be trunked but needs to be updated, do not send it back to VLC now
            size -= _left_over.size();
            _is_left_over_sent = false;
        }
        return size;
    }
    return size;
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
