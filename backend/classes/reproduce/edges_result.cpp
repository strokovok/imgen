#include "gen_result.cpp"
#include "gen_edges_context.cpp"
#include "edge_segment.cpp"


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
