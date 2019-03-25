#include <unordered_set>
#include <mutex>

#include "include/crow.h"
#include "include/json11/json11.cpp"

#include "classes/Session.cpp"

using namespace std;
using namespace json11;

int sessions_count = 0;
unordered_map<int, Session*> sessions;
unordered_map<crow::websocket::connection*, int> socket_session;
mutex mtx;

Session* get_socket_session(crow::websocket::connection* socket) {
    lock_guard<mutex> _(mtx);
    if (socket_session.count(socket)):
        return sessions[socket_session[socket]];
    return nullptr;
}

void forget_socket(crow::websocket::connection* socket) {
    Session* session = get_socket_session(socket);
    if (session)
        session->remove_socket(socket);
    lock_guard<mutex> _(mtx);
    socket_session.erase(socket);
}

int start_session(crow::websocket::connection* socket) {
    forget_socket(socket);
    lock_guard<mutex> _(mtx);
    socket_session[socket] = ++sessions_count;
    sessions[sessions_count] = new Session(sessions_count, socket);
    return sessions_count;
}

bool join_session(crow::websocket::connection* socket, int id) {
    forget_socket(socket);
    Session* session;
    {
        lock_guard<mutex> _(mtx);
        if (sessions.count(id) == 0)
            return false;
        session = sessions[id];
        session->awake();
        socket_session[socket] = id;
    }
    session->add_socket(socket);
    return true;
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/ws")
        .websocket()
        .onopen([&](crow::websocket::connection& socket) {
            CROW_LOG_INFO << "new websocket connection";
        })
        .onclose([&](crow::websocket::connection& socket, const string& reason) {
            CROW_LOG_INFO << "websocket connection closed: " << reason;
            forget_socket(&socket);
        })
        .onmessage([&](crow::websocket::connection& socket, const string& data, bool is_binary) {
            string err = "NOERR";
            Json request = Json::parse(data, err);
            if (err != "NOERR") {
                CROW_LOG_INFO << err;
                return;
            }
            if (request["op"].string_value() == "START_SESSION") {
                Json response = Json::object {
                    {"op", "SESSION_STARTED"},
                    {"session_id", start_session(&socket)},
                };
                u->send_text(response.dump());
            }
            if (request["op"].string_value() == "JOIN_SESSION") {
                int id = request["session_id"].int_value();
                Json response;
                if (join_session(&socket, id)) {
                    response = Json::object {
                        {"op", "SESSION_JOINED"},
                        {"session_id", id},
                    };  
                } else {
                    response = Json::object {
                        {"op", "NO_SUCH_SESSION"}
                    };
                }
                u->send_text(response.dump());
            }
        });

    app.port(40080)
        .multithreaded()
        .run();

    return 0;
}
