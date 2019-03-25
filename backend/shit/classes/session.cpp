#pragma once
#include <unordered_set>
#include <mutex>
#include <chrono>

class Session {

private:

	int id;
	unordered_set<crow::websocket::connection*> sockets;
	chrono::time_point<chrono::steady_clock> last_used;
	mutex mtx;

public:

	Session(int id, crow::websocket::connection* socket): id(id) {
		add_socket(socket);
	}

	int get_id() {
		lock_guard<mutex> _(mtx);
		return id;
	}

	void add_socket(crow::websocket::connection* socket) {
		lock_guard<mutex> _(mtx);
		sockets.insert(socket);
	}

	void remove_socket(crow::websocket::connection* socket) {
		lock_guard<mutex> _(mtx);
		sockets.erase(socket);
		if (sockets.size() == 0)
			last_used = chrono::steady_clock::now();
	}

	void awake() {
		if (mtx.try_lock()) {
			last_used = chrono::steady_clock::now();
			mtx.unlock();
		}
	}

	bool is_dead() {
		if (mtx.try_lock()) {
			if (sockets.size() > 0) {
				mtx.unlock();
				return false;
			}
			auto now = chrono::steady_clock::now();
			auto delta = chrono::duration_cast<chrono::minutes>(now - last_used).count();
			if (delta < 5) {
				mtx.unlock();
				return true;
			}
			mtx.unlock();
			return false;
		} else
			return false;
	}

};
