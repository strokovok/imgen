#include "../random_helper.cpp"


class GenEdgesContext {

public:

	Image *image;
	RandomHelper rnd;
	int segments_cnt;
	double best_possible_score;

	GenEdgesContext(Image *image, int segments_cnt):
		image(image), segments_cnt(segments_cnt) {

		best_possible_score = image->width() * image->height();
	}
};
