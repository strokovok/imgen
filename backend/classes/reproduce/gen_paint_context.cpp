#include "../random_helper.cpp"


class GenPaintContext {

public:

	Image *image;
	RandomHelper rnd;
	int triangles_cnt;
	int circles_cnt;
	int axis_div;
	double opacity;

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
			opacity(opacity)
		{}
};
