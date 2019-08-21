//
// Created by gouy_e on 8/16/19.
//

#include "proxy.h"

/**
 * Launch the HLS proxy
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char const *argv[]) {

    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " cdn_url" << std::endl;
        return EXIT_FAILURE;
    }

    const Url cdn_url = Url::parse(argv[1]);
    if (!cdn_url.is_valid()) {
        std::cerr << "Invalid CDN url..." << std::endl;
        return EXIT_FAILURE;
    }

    Proxy proxy(cdn_url);
    if (proxy.open_server()) {
        proxy.launch();
    } else {
        std::cerr << "Could not open the server..." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
