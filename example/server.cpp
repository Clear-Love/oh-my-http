#include "net/io_context.hpp"
#include "net/http_server.hpp"

void server() {
    io_context ctx;
    auto server = http_server::make();
    server->get_router().route("/", [](http_server::http_request &request) {
        std::string response;
        response += "url: " + request.url + "\r\n";
        response += "body: " + request.body + "\n";
        request.write_response(200, request.body);
    });
    server->do_start("127.0.0.1", "8080");

    ctx.join();
}

int main() {
    server();
    return 0;
}