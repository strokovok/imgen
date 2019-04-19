#include "gen_result.cpp"
#include "gen_paint_context.cpp"
#include "paint_triangle.cpp"
#include "paint_circle.cpp"

using namespace std;
using namespace json11;
using namespace cimg_library;


class PaintResult: public GenResult<PaintResult, GenPaintContext> {

public:

	vector<PaintTriangle> triangles;
	vector<PaintCircle> circles;

	vector<PaintTriangle>* get_triangles() {
		return &triangles;
	}

	vector<PaintCircle>* get_circles() {
		return &circles;
	}

	PaintResult(GenPaintContext *context, vector<PaintResult*> &from_population)
		: GenResult(context, from_population) {

		gen_property_vector<PaintTriangle>(&PaintResult::get_triangles);

		gen_property_vector<PaintCircle>(&PaintResult::get_circles);

		evaluate_score();

		parents.clear();
	}

	PaintResult(PaintResult *from) : GenResult(from) {
		triangles = from->triangles;
		circles = from->circles;
	}

	Json to_json() {
		return Json::object {
			{ "triangles", Json(triangles) },
			{ "circles", Json(circles) },
		};
	}

	Image* draw() {
		Image *image = context->image;
		Image *res = new Image(image->width(), image->height(), 1, 3);
		float opacity = context->opacity;
		res->fill(0);

		int ti = 0, ci = 0;
		for (bool done = false; !done;) {
			double tp = triangles.size() ? double(ti) / triangles.size() : 1e18;
			double cp = circles.size() ? double(ci) / circles.size() : 1e18;

			if (tp < cp)
				draw_triangle(triangles[ti++], res, opacity);
			else
				draw_circle(circles[ci++], res, opacity);

			done = true;
			done &= (ti == triangles.size());
			done &= (ci == circles.size());
		}

		return res;
	}

private:

	void draw_triangle(PaintTriangle &t, Image *res, float opacity) {
		unsigned char color[] = {(unsigned char)t.r, (unsigned char)t.g, (unsigned char)t.b};
		res->draw_triangle(t.x1, t.y1, t.x2, t.y2, t.x3, t.y3, color, opacity);
	}

	void draw_circle(PaintCircle &c, Image *res, float opacity) {
		unsigned char color[] = {(unsigned char)c.r, (unsigned char)c.g, (unsigned char)c.b};
		res->draw_circle(c.x, c.y, c.radius, color, opacity);
	}

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
