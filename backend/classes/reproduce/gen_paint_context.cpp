#include "../random_helper.cpp"


class GenPaintContext {

public:

	Image *image;
	RandomHelper rnd;
	int triangles_cnt;
	int circles_cnt;
	int axis_div;
	double opacity;
	double best_possible_score;

	GenPaintContext(
		Image *image,
		int triangles_cnt,
		int circles_cnt,
		int axis_div,
		double opacity):
			image(image),
			triangles_cnt(triangles_cnt),
			circles_cnt(circles_cnt),
			axis_div(axis_div),
			opacity(opacity) {

		best_possible_score = double(255 * 255) * 3 * image->width() * image->height();
	}
};
