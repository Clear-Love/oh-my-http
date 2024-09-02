#include "net/http_server.hpp"
#include "net/io_context.hpp"

void server() {
    io_context ctx;
    auto server = http_server::make();
    server->get_router().route("/", [](http_server::http_request &request) {
        std::string response;
        if (request.body.empty()) {
            response = "你好，你的请求正文为空哦";
        } else {
            response = "你好，你的请求是: [" + request.body + "]，共 " +
                       std::to_string(request.body.size()) + " 字节";
        }
        request.write_response(200, response);
    });
    server->do_start("127.0.0.1", "8080");

    ctx.join();
}

int main() {
    server();
    return 0;
}