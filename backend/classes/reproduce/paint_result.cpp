#include "gen_result.cpp"
#include "gen_paint_context.cpp"
#include "paint_triangle.cpp"

using namespace std;
using namespace json11;
using namespace cimg_library;


class PaintResult: public GenResult<PaintResult, GenPaintContext> {

public:

	vector<PaintTriangle> triangles;

	vector<PaintTriangle>* get_triangles() {
		return &triangles;
	}

	PaintResult(GenPaintContext *context, vector<PaintResult*> &from_population)
		: GenResult(context, from_population) {

		gen_property_vector<PaintTriangle>(&PaintResult::get_triangles);

		evaluate_score();

		parents.clear();
	}

	PaintResult(PaintResult *from) : GenResult(from) {
		triangles = from->triangles;
	}

	Json to_json() {
		return Json::object {
			{ "triangles", Json(triangles) }
		};
	}

	Image* draw() {
		Image *image = context->image;
		Image *res = new Image(image->width(), image->height(), 1, 3);
		res->fill(0);
		for (auto &t: triangles) {
			unsigned char color[] = {(unsigned char)t.r, (unsigned char)t.g, (unsigned char)t.b};
			float opacity = context->opacity;
			res->draw_triangle(t.x1, t.y1, t.x2, t.y2, t.x3, t.y3, color, opacity);
		}
		return res;
	}

private:

	void evaluate_score() {
		Image *image = context->image;
		Image *res = draw();

		score = 0;
		double worst = (255 * 255) * 3;
		for (int i = 0; i < image->width(); ++i)
			for (int j = 0; j < image->height(); ++j) {
				double sum = 0;
				for (int c = 0; c < 3; ++c) {
					int c1 = (*image)(i, j, 0, c);
					int c2 = (*res)(i, j, 0, c);
					sum += (c1 - c2) * (c1 - c2);
				}
				score += worst - sum;
			}

		delete res;
	}

};
