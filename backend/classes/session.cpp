#pragma once
#include <unordered_set>
#include <mutex>
#include <chrono>
#include <vector>

using namespace std;
using namespace json11;

class Session {

private:

	crow::websocket::connection* socket;

public:

	Session(crow::websocket::connection* socket): socket(socket) {

	}

	void take_message(const string& data) {
		string err = "NOERR";
        Json request = Json::parse(data, err);
        if (err != "NOERR") {
            CROW_LOG_INFO << err;
            return;
        }
        int cnt = request["cnt"].int_value();
        vector<Json> payload;
        for (int i = 0; i < cnt; ++i)
        	payload.push_back(Json(i));
        Json response = Json(payload);
        socket->send_text(response.dump());
	}

	~Session() {

	}

};
