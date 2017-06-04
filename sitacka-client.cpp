#include <iostream>
#include <bits/unique_ptr.h>
#include "siktacka/client/client.h"

int main() {
    std::unique_ptr<sikclient::Client> client = std::make_unique<sikclient::Client>(
            "janusz", "127.0.0.1", 12345u, "127.0.0.1", 12346u);
    try {
        client->run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}