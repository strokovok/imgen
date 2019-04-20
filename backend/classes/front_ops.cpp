using namespace std;

class FrontOps {
public:
	static const string START_UPLOADING;

	static const string UPLOAD_PIECE;

	static const string UPLOAD_DONE;

	static const string START;

	static const string STOP;
	
	static const string GET_RESULT;
};

const string FrontOps::START_UPLOADING = "START_UPLOADING";

const string FrontOps::UPLOAD_PIECE = "UPLOAD_PIECE";

const string FrontOps::UPLOAD_DONE = "UPLOAD_DONE";

const string FrontOps::START = "START";

const string FrontOps::STOP = "STOP";

const string FrontOps::GET_RESULT = "GET_RESULT";
