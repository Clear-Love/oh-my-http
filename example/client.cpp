#include "net/http_client.hpp"
#include "net/io_context.hpp"
#include <iostream>

#define URL "http://baidu.com"

void client() {
    io_context ctx;
    auto client = http_client::make();

    client->do_request(
        {"GET", URL}, [client](expected<int> ret,
                               http_client::http_response const &response) {
            ret.expect(URL);

            io_context::get().set_timeout(std::chrono::seconds(1), [client] {
                client->do_request(
                    {"GET", URL},
                    [client](expected<int> ret,
                             http_client::http_response const &response) {
                        ret.expect(URL);
                    });
            });
            std::cout << response.body << std::endl;
        });

    ctx.join();
}

int main() {
    client();
    return 0;
}