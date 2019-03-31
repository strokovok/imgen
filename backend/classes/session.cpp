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
#include "edge_detector.cpp"

using namespace std;
using namespace json11;
using namespace cimg_library;

class Session {

private:

	int id, state;
	crow::websocket::connection* socket;

	string user_image_path;

	int cur_file_size = 0;
	ofstream user_image_file;
	Image *user_image, *edges_image;

	const int MAX_SIZE = 1024;

	const int MAX_FILE_SIZE = 10 * 1024 * 1024;

public:

	Session(int id, crow::websocket::connection* socket): id(id), socket(socket) {
		state = SessionStates::START;
	}

	~Session() {
		clear();
	}

	void take_message(const string& data) {
		if (state == SessionStates::BROKEN)
			return;

		string err = "NOERR";
        Json request = Json::parse(data, err);
        if (err != "NOERR") {
            CROW_LOG_INFO << err;
            return;
        }

        string op = request["op"].string_value();

        if (op == FrontOps::START_UPLOADING) return void(on_start_uploading(request));

        if (op == FrontOps::UPLOAD_PIECE) return void(on_upload_piece(request));

        if (op == FrontOps::UPLOAD_DONE) return void(on_upload_done(request));

        send_invalid("Unknown operation: " + op);
	}

private:

	void send(Json &response) {
		socket->send_text(response.dump());
	}

	void send_invalid(string reason) {
		Json response = Json::object {
			{"op", BackOps::INVALID_OPERATION},
			{"reason", reason},
		};
		send(response);
	}

	void on_start_uploading(Json &request) {
		if (state != SessionStates::START)
			return void(send_invalid("Upload already started"));

		string extension = request["extension"].string_value();
		if (extension != "png" && extension != "jpg" && extension != "jpeg")
			return void(send_invalid("Unknown image format: " + extension));

		state = SessionStates::UPLOADING;
		user_image_path = "sessions/" + to_string(id) + "." + extension;
		user_image_file.open(user_image_path, ios::binary);
		Json response = Json::object {
			{"op", BackOps::GIVE_ME_PIECE}
		};
		send(response);
	}

	void on_upload_piece(Json &request) {
		if (state != SessionStates::UPLOADING)
			return void(send_invalid("Uploading is not in progress"));

		if (!request["piece"].is_string())
			return void(send_invalid("Piece must be string"));

		string piece = request["piece"].string_value();
		if (piece.length() % 2 != 0)
			return void(send_invalid("Length of piece must be even"));

		if (piece.length() / 2 + cur_file_size > MAX_FILE_SIZE)
			return void(send_invalid("Max size of file is: " + to_string(MAX_FILE_SIZE) + " bytes"));

		for (int i = 0; i < piece.length(); i += 2) {
			char a = piece[i], b = piece[i + 1];
			if (a < 'a' || b < 'a' || a >= ('a' + 16) || b >= ('a' + 16))
				return void(send_invalid("Piece chars must be in range ['a', 'a' + 16)"));
			unsigned char byte = (a - 'a') * 16 + (b - 'a');
			user_image_file << byte;
			++cur_file_size;
		}
		Json response = Json::object {
			{"op", BackOps::GIVE_ME_PIECE}
		};
		send(response);
	}

	void on_upload_done(Json &request) {
		user_image_file.close();
		try {
			process_file();
		} catch (...) {
			broke("Unable to process file");
			return;
		}
		state = SessionStates::READY_TO_RUN;
		Json response = Json::object {
			{"op", BackOps::READY_TO_RUN}
		};
		send(response);
	}

	void process_file() {
		user_image = new Image(user_image_path.c_str());

		int w = user_image->width(), h = user_image->height();
		float mul = (w > h) ? MAX_SIZE / float(w) : MAX_SIZE / float(h);
		w *= mul, h *= mul;
		user_image->resize(w, h);

		edges_image = EdgeDetector().process(user_image);
	}

	void clear() {
		if (state == SessionStates::BROKEN)
			return;

		if (state == SessionStates::UPLOADING)
			user_image_file.close();

		if (state != SessionStates::START && state != SessionStates::UPLOADING) {
			delete user_image;
			delete edges_image;
		}
	}

	void broke(string reason) {
		clear();
		state = SessionStates::BROKEN;
		Json response = Json::object {
			{"op", BackOps::BROKEN},
			{"reason", reason},
		};
		send(response);
	}

};
