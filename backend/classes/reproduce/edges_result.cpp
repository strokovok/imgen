#include "gen_result.cpp"
#include "../random_helper.cpp"

using namespace std;
using namespace json11;
using namespace cimg_library;


class GenEdgesContext {

public:

	Image *image;
	RandomHelper rnd;
	int segments_cnt;

	GenEdgesContext(Image *image, int segments_cnt):
		image(image), segments_cnt(segments_cnt) {}
};


class EdgeSegment {

private:

	int x, y, len;
	double x_part, y_part;

	void recount(GenEdgesContext *context) {
		if (abs(x_part) < 0.01 && abs(y_part) < 0.01)
			x_part = y_part = 1;
		double mul = len / (sqrt(x_part * x_part + y_part * y_part) * 2);
		x_part *= mul, y_part *= mul;

		int w = context->image->width();
		int h = context->image->height();

		x1 = max(min(x + (int)x_part, w - 1), 0);
		x2 = max(min(x - (int)x_part, w - 1), 0);
		y1 = max(min(y + (int)y_part, h - 1), 0);
		y2 = max(min(y - (int)y_part, h - 1), 0);
	}

public:

	static int get_cnt(GenEdgesContext *context) {
		return context->segments_cnt;
	}

	int x1, y1, x2, y2;

	EdgeSegment(GenEdgesContext *context, int num) {
		int w = context->image->width();
		int h = context->image->height();
		int avg = (w + h) / 2;

		x = context->rnd.random_int(0, w - 1);
		y = context->rnd.random_int(0, h - 1);
		len = context->rnd.random_int(avg / 40, avg / 14);
		x_part = context->rnd.random_double(-1, 1);
		y_part = context->rnd.random_double(-1, 1);
		recount(context);
	}

	void mutate(GenEdgesContext *context, int num) {
		int w = context->image->width();
		int h = context->image->height();
		int avg = (w + h) / 2;
		int r = 5;

		x = max(min(x + context->rnd.random_int(-r, r), w - 1), 0);
		y = max(min(y + context->rnd.random_int(-r, r), h - 1), 0);
		len = context->rnd.random_int(avg / 40, avg / 14);
		x_part = context->rnd.random_double(-1, 1);
		y_part = context->rnd.random_double(-1, 1);
		recount(context);
	}

	Json to_json() const {
		return Json::array { x1, y1, x2, y2 };
	}
};


class EdgesResult: public GenResult<EdgesResult, GenEdgesContext> {

public:

	vector<EdgeSegment> segments;

	vector<EdgeSegment>* get_segments() {
		return &segments;
	}

	EdgesResult(GenEdgesContext *context, vector<EdgesResult*> &from_population)
		: GenResult(context, from_population) {

		gen_property_vector<EdgeSegment>(&EdgesResult::get_segments);

		evaluate_score();

		parents.clear();
	}

	EdgesResult(EdgesResult *from) : GenResult(from) {
		segments = from->segments;
	}

	Json to_json() {
		return Json::object {
			{ "segments", Json(segments) }
		};
	}

	Image* draw() {
		Image *image = context->image;
		Image *res = new Image(image->width(), image->height(), 1, 1);
		res->fill(0);
		unsigned char color[] = {255};
		for (auto &s: segments)
			res->draw_line(s.x1, s.y1, s.x2, s.y2, color);
		return res;
	}

private:

	void evaluate_score() {
		Image *image = context->image;
		Image *res = draw();

		score = 0;
		for (int i = 0; i < image->width(); ++i)
			for (int j = 0; j < image->height(); ++j) {
				int c1 = (*image)(i, j, 0, 0);
				int c2 = (*res)(i, j, 0, 0);
				score += 1 - abs(c1 - c2) / double(255);
			}

		delete res;
	}

};
