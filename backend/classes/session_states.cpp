using namespace std;

class SessionStates {
public:
	static const int START = 0;

	static const int UPLOADING = 1;
	
	static const int READY_TO_RUN = 2;

	static const int RUNNING = 3;

	static const int BROKEN = 99999;
};
