#pragma once
#include <unordered_set>
#include <mutex>
#include <chrono>
#include <vector>
#include <fstream>
#include <string>

#include "front_ops.cpp"
#include "back_ops.cpp"
#include "session_states.cpp"

using namespace std;
using namespace json11;

class Session {

private:

	int id, state;
	crow::websocket::connection* socket;
	ofstream user_image;

public:

	Session(int id, crow::websocket::connection* socket): id(id), socket(socket) {
		state = SessionStates::START;
	}

	void send(Json &response) {
		socket->send_text(response.dump());
	}

	void take_message(const string& data) {
		string err = "NOERR";
        Json request = Json::parse(data, err);
        if (err != "NOERR") {
            CROW_LOG_INFO << err;
            return;
        }

        string op = request["op"].string_value();

        if (op == FrontOps::START_UPLOADING) on_start_uploading(request);

        if (op == FrontOps::UPLOAD_PIECE) on_upload_piece(request);

        if (op == FrontOps::UPLOAD_DONE) on_upload_done(request);
	}

	void on_start_uploading(Json &request) {
		state = SessionStates::UPLOADING;
		string file_name = "sessions/" + to_string(id) + "." + request["extension"].string_value();
		user_image.open(file_name, ios::binary);
		Json response = Json::object {
			{"op", BackOps::GIVE_ME_PIECE}
		};
		send(response);
	}

	void on_upload_piece(Json &request) {
		auto bytes = request["piece"].array_items();
		for (auto &item: bytes) {
			unsigned char byte = item.int_value();
			user_image << byte;
		}
		Json response = Json::object {
			{"op", BackOps::GIVE_ME_PIECE}
		};
		send(response);
	}

	void on_upload_done(Json &request) {
		user_image.close();
		state = SessionStates::PROCESSING_FILE;
	}

	~Session() {

	}

};
