

class PaintCircle {

private:

	void random_jump(int &x, int jump, int l, int r, GenPaintContext *context) {
		x = max(min(x + context->rnd.random_int(-jump, +jump), r), l);
	}

	void evaulate_color(GenPaintContext *context) {
		int w = context->image->width();
		int h = context->image->height();

		int xmin = max(x - radius, 0);
		int xmax = min(x + radius, w - 1);
		int ymin = max(y - radius, 0);
		int ymax = min(y + radius, h - 1);

		r = g = b = 0;
		int cnt = 0;

		Image *image = context->image;

		for (int i = xmin; i <= xmax; ++i)
			for (int j = ymin; j <= ymax; ++j)
				if ((i - x) * (i - x) + (j - y) * (j - y) <= radius * radius) {
					r += (*image)(i, j, 0, 0);
					g += (*image)(i, j, 0, 1);
					b += (*image)(i, j, 0, 2);
					++cnt;
				}
		r /= cnt;
		g /= cnt;
		b /= cnt;
	}

public:

	static int get_cnt(GenPaintContext *context) {
		return context->circles_cnt;
	}

	int x, y, radius, r, g, b;

	PaintCircle(GenPaintContext *context, int num) {
		int w = context->image->width();
		int h = context->image->height();
		int avg = (w + h) / 2;

		x = context->rnd.random_int(0, w - 1);
		y = context->rnd.random_int(0, h - 1);

		int max_radius = avg / context->axis_div;
		if (num < context->triangles_cnt / 10)
			max_radius = avg / context->axis_div * 4;

		radius = context->rnd.random_int(1, max_radius);

		evaulate_color(context);
	}

	void mutate(GenPaintContext *context, int num) {
		int w = context->image->width();
		int h = context->image->height();
		int avg = (w + h) / 2;

		if (context->rnd.random_double(0, 1) < 0.6) {
			random_jump(x, avg / 20, 0, w - 1, context);
			random_jump(y, avg / 20, 0, h - 1, context);
		}
		
		int max_radius = avg / context->axis_div;
		if (num < context->triangles_cnt / 10)
			max_radius = avg / context->axis_div * 4;

		if (context->rnd.random_double(0, 1) < 0.6)
			random_jump(radius, avg / 10, 1, max_radius, context);

		evaulate_color(context);
	}

	Json to_json() const {
		return Json::array { x, y, radius, r, g, b };
	}
};
