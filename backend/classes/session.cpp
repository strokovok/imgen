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
#include "reproduce/paint_result.cpp"

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

	GenWorker<EdgesResult, GenEdgesContext> *edges_worker;
	GenWorker<PaintResult, GenPaintContext> *paint_worker;

	const int MAX_FILE_SIZE = 10 * 1024 * 1024;

	const int EDGES_CANVAS_SIZE = 256;

	const int PAINT_CANVAS_SIZE = 128;

	const int MAX_SEGMENTS = 1024;

	const int MAX_TRIANGLES = 1024;

	const int MAX_CIRCLES = 1024;

	const int MAX_AXIS_DIV = 15;

public:

	Session(int id, crow::websocket::connection* socket): id(id), socket(socket) {
		state = SessionStates::START;
		Json msg = Json::object {
			{"op", BackOps::CONSTS},
			{"edges_canvas_size", EDGES_CANVAS_SIZE},
			{"paint_canvas_size", PAINT_CANVAS_SIZE},
			{"max_segments", MAX_SEGMENTS},
			{"max_triangles", MAX_TRIANGLES},
			{"max_circles", MAX_CIRCLES},
			{"max_axis_div", MAX_AXIS_DIV},
		};
		send(msg);
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

	bool extract_int_param(Json &request, string name, int &x, int mn, int mx) {
		if (request[name].is_number())
			x = request[name].int_value();
		if (x < mn || x > mx) {
			broke("Incorrect " + name);
			return false;
		}
		return true;
	}

	bool extract_double_param(Json &request, string name, double &x, double mn, double mx) {
		if (request[name].is_number())
			x = request[name].number_value();
		if (x < mn || x > mx) {
			broke("Incorrect " + name);
			return false;
		}
		return true;
	}

	void on_start(Json &request) {
		if (state != SessionStates::READY_TO_RUN)
			return void(broke("Not ready to start"));

		int segments_cnt = 0;
		if (!extract_int_param(request, "segments_cnt", segments_cnt, 0, MAX_SEGMENTS))
			return;

		int triangles_cnt = 0;
		if (!extract_int_param(request, "triangles_cnt", triangles_cnt, 0, MAX_TRIANGLES))
			return;

		int circles_cnt = 0;
		if (!extract_int_param(request, "circles_cnt", circles_cnt, 0, MAX_CIRCLES))
			return;

		int axis_div = 10;
		if (!extract_int_param(request, "axis_div", axis_div, 2, MAX_AXIS_DIV))
			return;

		double paint_opacity = 0.75;
		if (!extract_double_param(request, "paint_opacity", paint_opacity, 0, 1))
			return;

		edges_worker = new GenWorker<EdgesResult, GenEdgesContext>(
			new GenEdgesContext(edges_image, segments_cnt)
		);

		paint_worker = new GenWorker<PaintResult, GenPaintContext>(
			new GenPaintContext(
				user_image,
				triangles_cnt,
				circles_cnt,
				axis_div,
				paint_opacity
			)
		);

		state = SessionStates::RUNNING;
	}

	void on_get_result(Json &request) {
		if (state != SessionStates::RUNNING)
			return void(broke("Not running to get result"));

		double iteration =
			(edges_worker->get_iteration() + paint_worker->get_iteration()) / 2;

		double accuracy =
			(edges_worker->get_accuracy() + paint_worker->get_accuracy()) / 2;

		Json response = Json::object {
			{"op", BackOps::RESULT},
			{"edges", edges_worker->get_result()},
			{"paint", paint_worker->get_result()},
			{"iteration", iteration},
			{"accuracy", accuracy},
		};
		send(response);
	}

	Image* open_and_resize(string path, int max_size) {
		Image *image = new Image(path.c_str());

		int w = image->width(), h = image->height();
		float mul = (w > h) ? max_size / float(w) : max_size / float(h);
		w *= mul, h *= mul;
		image->resize(w, h);

		return image;
	}

	void process_file(string path) {
		user_image = open_and_resize(path, PAINT_CANVAS_SIZE);

		Image *image_for_edges = open_and_resize(path, EDGES_CANVAS_SIZE);
		edges_image = EdgeDetector().process(image_for_edges);
		delete image_for_edges;
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
			delete paint_worker;
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
