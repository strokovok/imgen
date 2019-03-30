
using namespace std;
using namespace cimg_library;

typedef CImg<unsigned char> Image;

class EdgeDetector {

private:

	const int STRONG_THRESHOLD = 90;
	const int G_THRESHOLD = 170;

	int *maskx = new int[9] {-1, -2, -1, 0, 0, 0, 1, 2, 1};
	int *masky = new int[9] {-1, 0, 1, -2, 0, 2, -1, 0, 1};
	int *maskxy = new int[9] {0, 1, 2, -1, 0, 1, -2, -1, 0};
	int *maskyx = new int[9] {2, 1, 0, 1, 0, -1, 0, -1, -2};

	int apply_mask(Image *input, int i, int j, int *mask) {
		int g = 0, mc = 0;
		for (int y = -1; y <= 1; ++y)
			for (int x = -1; x <= 1; ++x)
				g += (*input)(i + x, j + y, 0, 0) * mask[mc++];
		return g;
	}

	double count_g(Image *input, int i, int j) {
		int gx = abs(apply_mask(input, i, j, maskx));
		int gy = abs(apply_mask(input, i, j, masky));
		int gxy = abs(apply_mask(input, i, j, maskxy));
		int gyx = abs(apply_mask(input, i, j, maskyx));
	 	double g = max(max(gx, gy), max(gxy, gyx));
	 	return g;
	}

	void filter_weak(Image *input) {
		for (int i = 1; i + 1 < input->width(); ++i)
			for (int j = 1; j + 1 < input->height(); ++j) {
				bool strong = false;
				for (int x = -1; x <= 1; ++x)
					for (int y = -1; y <= 1; ++y)
						strong |= (*input)(i + x, j + y, 0, 0) > STRONG_THRESHOLD;
				if (!strong)
					for (int c = 0; c < 3; ++c)
						(*input)(i, j, 0, c) = 0;
			}
	}

	void make_edges(Image *input) {
		Image *source = new Image(*input);
		input->fill(0);
		for (int i = 1; i + 1 < input->width(); ++i)
			for (int j = 1; j + 1 < input->height(); ++j) {
				double g = count_g(source, i, j);
				unsigned char color = min(int(g / G_THRESHOLD * 255), 255);
				for (int c = 0; c < 3; ++c)
					(*input)(i, j, 0, c) = color;
			}
		filter_weak(input);
		delete source;
	}

	void make_grayscale(Image *input) {
		cimg_forXY(*input, x, y) {
			int sum = 0;
			for (int c = 0; c < 3; ++c)
				sum += (*input)(x, y, 0, c);
			int gs = sum / 3;
			for (int c = 0; c < 3; ++c)
				(*input)(x, y, 0, c) = gs;
		}
	}

public:

	Image* process(Image *input) {
		Image *result = new Image(*input);
		make_grayscale(result);
		float sigma = 1;
		result->blur(sigma, sigma, sigma, true, true);
		make_edges(result);
		return result;
	}

};
