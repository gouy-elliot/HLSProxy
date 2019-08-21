//
// Created by gouy_e on 8/16/19.
//

#include "proxy.h"

volatile bool Proxy::_stop_signal = false;

Proxy::Proxy(const Url &cdn_url) : _cdn_url(cdn_url), _server_fd(-1) {

}

Proxy::~Proxy() {
    auto it = _clients.begin();
    while (it != _clients.end()) {
        delete (*it);
        _clients.erase(it++);
    }

    if (_server_fd != -1) {
        ::close(_server_fd);
    }
}

/**
 * Initialize the read fd_set using the opened sockets (server + clients)
 * @param read_set
 * @return the highest file descriptor set in the fd_set
 */
int Proxy::init_read_set(fd_set *read_set) const {
    std::list<Client *>::const_iterator it;
    int max_fd = _server_fd;

    FD_ZERO(read_set);
    FD_SET(_server_fd, read_set);
    for (it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->get_fd() > max_fd) {
            max_fd = (*it)->get_fd();
        }

        if ((*it)->get_cdn_fd() > max_fd) {
            max_fd = (*it)->get_cdn_fd();
        }

        FD_SET((*it)->get_fd(), read_set);
        FD_SET((*it)->get_cdn_fd(), read_set);
    }

    return max_fd;
}

/**
 * Setup signal handler
 * @return false if an error occurred, true otherwise
 */
bool Proxy::init_signal() const {
    struct sigaction sig_int_handler;

    sig_int_handler.sa_handler = Proxy::handle_signal;
    sigemptyset(&sig_int_handler.sa_mask);
    sig_int_handler.sa_flags = 0;

    bool sign_int = sigaction(SIGINT, &sig_int_handler, nullptr) >= 0;
    return sigaction(SIGTERM, &sig_int_handler, nullptr) >= 0 && sign_int;
}

/**
 * Called when a signal is received
 * @param signal
 */
void Proxy::handle_signal(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        Proxy::_stop_signal = true;
    }
}

/**
 * Accept a new client from the proxy socket and add it in the client list
 * @return false if an error occurred when adding the new client, true otherwise
 */
bool Proxy::accept_client() {
    int client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((client_fd = accept(_server_fd, (struct sockaddr *) &address, (socklen_t*) &addrlen)) < 0) {
        return false;
    }

    auto *new_client = new Client(_cdn_url, client_fd);
    if (new_client->open_cdn()) {
        _clients.push_back(new_client);
        return true;
    } else {
        delete new_client;
        return false;
    }
}

/**
 * Open the proxy server socket and listen for incoming connection
 * @return false if an error occurred, false otherwise
 */
bool Proxy::open_server() {
    if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        return false;
    }

    int opt = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        return false;
    }

    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PROXY_PORT);
    if (bind(_server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        return false;
    }

    return listen(_server_fd, 1) >= 0;
}

/**
 * Launch the proxy server main loop
 */
void Proxy::launch() {
    fd_set read_set;
    bool loop = true;

    init_signal();
    while (loop && !Proxy::_stop_signal) {
        int max_fd = init_read_set(&read_set);
        int rec_val = select(max_fd + 1, &read_set, nullptr, nullptr, nullptr);

        loop = rec_val > 0;
        if (loop) {
            if (FD_ISSET(_server_fd, &read_set)) {
                accept_client();
            }

            std::list<Client *>::const_iterator it;
            for (it = _clients.begin(); it != _clients.end(); ++it) {

                if (FD_ISSET((*it)->get_fd(), &read_set)) {
                    int size = ::read((*it)->get_fd() , (*it)->get_buffer(), BUFFER_SIZE);
                    if (size > 0) {
                        bool is_request = (*it)->parse_in_request(size);
                        _manifest_parser.check_loaded_manifest(*it);
                        (*it)->print_in_request();
                        if (is_request) {
                            size = (*it)->update_host(size);
                        }
                        if (::write((*it)->get_cdn_fd(), (*it)->get_buffer(), size) <= 0) {
                            // We are a proxy should not happen
                            std::cerr << "Error when writing data..." << std::endl;
                        }
                    } else if (size == 0) {
                        _manifest_parser.clear(*it);
                        delete (*it);
                        _clients.erase(it++);
                        continue;
                    }
                }

                if (FD_ISSET((*it)->get_cdn_fd(), &read_set)) {
                    int size = ::read((*it)->get_cdn_fd() , (*it)->get_buffer(), BUFFER_SIZE);
                    if (size > 0) {
                        (*it)->print_out_request();
                        _manifest_parser.parse_manifest(*it, (*it)->get_buffer(), size);
                        if (::write((*it)->get_fd(), (*it)->get_buffer(), size) <= 0) {
                            // We are a proxy should not happen
                            std::cerr << "Error when writing data..." << std::endl;
                        }
                    } else if (size == 0) {
                        delete (*it);
                        _clients.erase(it++);
                    }
                }
            }
        }
    }
}
