//
// Created by gouy_e on 8/18/19.
//

#include "client.h"

Client::Client(const Url &cdn_url, int fd) : _cdn_url(cdn_url), _fd(fd), _cdn_fd(-1), _buffer() {

}

Client::~Client() {
    if (_cdn_fd != -1) {
        ::close(_cdn_fd);
    }

    ::close(_fd);
}

int Client::get_fd() const {
    return _fd;
}

int Client::get_cdn_fd() const {
    return _cdn_fd;
}

/**
 * Get the extension based on the location found in the HTTP header
 * @return the file extension or an empty string
 */
std::string Client::get_request_extension() const {
    size_t found = _request_uri.find_last_of('.');
    if (found != std::string::npos) {
        return _request_uri.substr(found);
    }
    return "";
}

const std::string &Client::get_request_uri() const {
    return _request_uri;
}

char *Client::get_buffer() {
    return _buffer;
}

/**
 * Open the socket to the outgoing server
 * @return false if an error occurred, true otherwise
 */
bool Client::open_cdn() {
    int err;
    struct addrinfo hints = {}, *addrs;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    err = getaddrinfo(_cdn_url.get_host().c_str(), _cdn_url.get_port().c_str(), &hints, &addrs);
    if (err != 0) {
        return false;
    }

    for (struct addrinfo *addr = addrs; addr != nullptr; addr = addr->ai_next) {
        _cdn_fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (_cdn_fd == -1) {
            break;
        }

        if (connect(_cdn_fd, addr->ai_addr, addr->ai_addrlen) == 0) {
            break;
        }

        ::close(_cdn_fd);
        _cdn_fd = -1;
    }

    freeaddrinfo(addrs);
    return _cdn_fd != -1;
}

/**
 * Update the Host HTTP headers in the client buffer to the proxy host
 * Use CRLF to find the header
 * @param size
 * @return the new size of bytes of the buffer
 */
size_t Client::update_host(size_t size) {
    unsigned long i = 0;
    while (i < size) {
        if (strncmp(_buffer + i, HTTP_HOST_HEADER, HTTP_HOST_HEADER_SIZE) == 0) {

            int start_host = i + HTTP_HOST_HEADER_SIZE;
            int end_host = start_host;
            while (end_host + 1 < size) {
                if (_buffer[end_host] == '\r' && _buffer[end_host + 1] == '\n') {
                    int new_host_size = _cdn_url.get_host().length();
                    memcpy(_buffer + start_host + new_host_size, _buffer + end_host, size - end_host);
                    memcpy(_buffer + start_host, _cdn_url.get_host().c_str(), new_host_size);
                    return size + new_host_size - (end_host - start_host);
                }
                end_host += 1;
            }
        }
        ++i;
    }

    return size;
}

/**
 * Parse the incoming request
 * Use CLRF to find the beginning of the packet
 * @param size
 * @return true if request has been found, false otherwise
 */
bool Client::parse_in_request(size_t size) {
    int i = 0;
    int begin_of_line = 0;
    while (++i < size) {
        if (_buffer[i - 1] == '\r' && _buffer[i] == '\n') {

            size_t line_size = i - begin_of_line - 1; // Remove the \r
            std::string line = std::string(_buffer + begin_of_line, line_size);

            // First line of packet
            size_t found = line.find_first_of(' ');
            if (found != std::string::npos) {
                line = line.substr(found + 1);
                found = line.find_first_of(' ');
                if (found != std::string::npos) {
                    _request_start = std::chrono::system_clock::now();
                    _request_protocol = line.substr(found + 1);
                    _request_uri = _cdn_url.get_protocol() + "://" + _cdn_url.get_host() + line.substr(0, found);
                    return true;
                }
            }
        }
    }
    _request_uri = "";
    _request_protocol = "";
    return false;
}

/**
 * Print the incoming request
 * @return
 */
void Client::print_in_request() const {
    std::string extension = get_request_extension();
    if (extension == MANIFEST_EXTENSION) {
        std::cout << "[IN][MANIFEST] " << _request_uri << std::endl;
    } else if (extension == SEGMENT_EXTENSION) {
        std::cout << "[IN][SEGMENT] " << _request_uri << std::endl;
    } else {
        std::cout << "[IN] " << _request_uri << std::endl;
    }
}

/**
 * Print the outgoing request
 * @return
 */
void Client::print_out_request() const {
    if (strncmp(_buffer, _request_protocol.c_str(), _request_protocol.length()) == 0) {
        int elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - _request_start).count();

        std::string extension = get_request_extension();
        if (extension == MANIFEST_EXTENSION) {
            std::cout << "[OUT][MANIFEST] " << _request_uri << " (" << elapsed_milliseconds << "ms)" << std::endl;
        } else if (extension == SEGMENT_EXTENSION) {
            std::cout << "[OUT][SEGMENT] " << _request_uri << " (" << elapsed_milliseconds << "ms)" << std::endl;
        } else {
            std::cout << "[OUT] " << _request_uri << " (" << elapsed_milliseconds << "ms)" << std::endl;
        }
    }
}
