

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
