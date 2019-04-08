#pragma once
#include <unordered_set>
#include <mutex>
#include <ctime>
#include <fstream>
#include <string>

#include "front_ops.cpp"
#include "back_ops.cpp"
#include "session_states.cpp"
#include "edge_detector.cpp"
#include "base64_file_saver.cpp"
#include "reproduce/gen_worker.cpp"
#include "reproduce/edges_result.cpp"

using namespace std;
using namespace json11;
using namespace cimg_library;

class Session {

private:

	int id, state;
	crow::websocket::connection* socket;

	string user_image_base64;
	string user_image_extension;

	Image *user_image, *edges_image;

	GenEdgesContext *edges_context;
	GenWorker<EdgesResult, GenEdgesContext> *edges_worker;

	//const int MAX_SIZE = 1024;
	const int MAX_SIZE = 256;

	const int MAX_FILE_SIZE = 10 * 1024 * 1024;

	const int MAX_SEGMENTS = 512;

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
        if (err != "NOERR")
            return void(broke("Unable to parse json: " + err));

        string op = request["op"].string_value();

        if (op == FrontOps::START_UPLOADING) return void(on_start_uploading(request));

        if (op == FrontOps::UPLOAD_PIECE) return void(on_upload_piece(request));

        if (op == FrontOps::UPLOAD_DONE) return void(on_upload_done(request));

    	if (op == FrontOps::START) return void(on_start(request));

        if (op == FrontOps::GET_RESULT) return void(on_get_result(request));

        broke("Unknown operation: " + op);
	}

private:

	void send(Json &response) {
		socket->send_text(response.dump());
	}

	void on_start_uploading(Json &request) {
		if (state != SessionStates::START)
			return void(broke("Upload already started"));

		string ext = request["extension"].string_value();
		if (ext != "png" && ext != "jpg" && ext != "jpeg")
			return void(broke("Unknown image format: " + ext));
		user_image_extension = ext;

		if (request["length"].is_number()) {
			int length = request["length"].int_value();
			if (length > MAX_FILE_SIZE)
				return void(broke("Max size of file is: " + to_string(MAX_FILE_SIZE) + " bytes"));
			user_image_base64.reserve(length);
		}

		state = SessionStates::UPLOADING;
		Json response = Json::object {
			{"op", BackOps::GIVE_ME_PIECE}
		};
		send(response);
	}

	void on_upload_piece(Json &request) {
		if (state != SessionStates::UPLOADING)
			return void(broke("Uploading is not in progress"));

		if (!request["piece"].is_string())
			return void(broke("Piece must be string"));

		string piece = request["piece"].string_value();
		if (piece.length() + user_image_base64.length() > MAX_FILE_SIZE)
			return void(broke("Max size of file is: " + to_string(MAX_FILE_SIZE) + " bytes"));

		user_image_base64 += piece;

		Json response = Json::object {
			{"op", BackOps::GIVE_ME_PIECE}
		};
		send(response);
	}

	void on_upload_done(Json &request) {
		if (state != SessionStates::UPLOADING)
			return void(broke("Uploading is not in progress"));

		string stamp = to_string(time(NULL)) + "_" + to_string(id);
		string user_image_path = "sessions/" + stamp + '.' + user_image_extension;

		try {
			if (!Base64FileSaver().save(user_image_base64, user_image_path))
				return void(broke("Unable to parse base64"));
			clear_base64_cache();
			process_file(user_image_path);
		} catch (...) {
			return void(broke("Unable to process file"));
		}

		state = SessionStates::READY_TO_RUN;
		Json response = Json::object {
			{"op", BackOps::READY_TO_RUN}
		};
		send(response);
	}

	void on_start(Json &request) {
		if (state != SessionStates::READY_TO_RUN)
			return void(broke("Not ready to start"));

		int segments_cnt = 0;
		if (request["segments_cnt"].is_number())
			segments_cnt = request["segments_cnt"].int_value();

		if (segments_cnt < 0 || segments_cnt > MAX_SEGMENTS)
			return void(broke("Incorrect segments_cnt"));

		edges_context = new GenEdgesContext(edges_image, segments_cnt);

		edges_worker = new GenWorker<EdgesResult, GenEdgesContext>(edges_context);

		state = SessionStates::RUNNING;
	}

	void on_get_result(Json &request) {
		if (state != SessionStates::RUNNING)
			return void(broke("Not running to get result"));

		Json response = Json::object {
			{"op", BackOps::RESULT},
			{"edges", edges_worker->get_result()}
		};
		send(response);
	}

	void process_file(string user_image_path) {
		user_image = new Image(user_image_path.c_str());

		int w = user_image->width(), h = user_image->height();
		float mul = (w > h) ? MAX_SIZE / float(w) : MAX_SIZE / float(h);
		w *= mul, h *= mul;
		user_image->resize(w, h);

		edges_image = EdgeDetector().process(user_image);
	}

	void clear_base64_cache() {
		user_image_base64 = "";
		user_image_base64.shrink_to_fit();
	}

	void clear() {
		if (state == SessionStates::BROKEN)
			return;

		if (state == SessionStates::UPLOADING)
			clear_base64_cache();

		if (state == SessionStates::RUNNING) {
			delete edges_worker;
			delete edges_context;
		}

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
