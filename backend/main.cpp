#include <unordered_set>
#include <mutex>

#include "include/crow.h"
#include "include/json11/json11.cpp"

#include "classes/session.cpp"

using namespace std;

unordered_map <crow::websocket::connection*, Session*> socket_session;
mutex mtx;

void start_session(crow::websocket::connection* socket) {
    lock_guard<mutex> _(mtx);
    socket_session[socket] = new Session(socket);
}

Session* get_session(crow::websocket::connection* socket) {
    lock_guard<mutex> _(mtx);
    return socket_session[socket];
}

void end_session(crow::websocket::connection* socket) {
    Session* session = get_session(socket);
    delete session;
    lock_guard<mutex> _(mtx);
    socket_session.erase(socket);
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/ws")
        .websocket()
        .onopen([&](crow::websocket::connection& socket) {
            CROW_LOG_INFO << "new websocket connection";
            start_session(&socket);
        })
        .onclose([&](crow::websocket::connection& socket, const string& reason) {
            CROW_LOG_INFO << "websocket connection closed: " << reason;
            end_session(&socket);
        })
        .onmessage([&](crow::websocket::connection& socket, const string& data, bool is_binary) {
            Session* session = get_session(&socket);
            session->take_message(data);
        });

    app.port(40080)
        .multithreaded()
        .run();

    return 0;
}