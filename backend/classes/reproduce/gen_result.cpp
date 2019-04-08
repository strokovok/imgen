#pragma once
#include <algorithm>
#include <functional>

using namespace std;


template <class T, class C>
class GenResult {

private:

	static const int MAX_PARENTS = 2;
	double parents_cnt_prob[MAX_PARENTS + 1] = {0.1, 0.2, 0.7};

public:

	C *context;

	double score;

	vector<T*> parents;

	GenResult(C *context, vector<T*> &from_population):
		context(context) {

		vector<pair<int, double>> space;
		for (int i = 0; i <= min(MAX_PARENTS, (int)from_population.size()); ++i)
			space.push_back(make_pair(i, parents_cnt_prob[i]));
		int parents_cnt = context->rnd.random_choice(space);

		for (int i = 0; i < parents_cnt; ++i) {
			int index = context->rnd.random_int(0, from_population.size() - 1);
			parents.push_back(from_population[index]);
		}
	}

	GenResult(T *from) {
		context = from->context;
		score = from->score;
	}

protected:

	const double FULL_MUTATION_PROB = 0.02;
	const double PART_MUTATION_PROB = 0.04;

	template <class P>
	void gen_property_vector(function<vector<P>*(T*)> get_pv) {
		T* self = static_cast<T*>(this);
		vector<P> *mv = get_pv(self);

		vector<vector<P>*> pv;
		for (auto &p: parents)
			pv.push_back(get_pv(p));

		int cnt = P::get_cnt(context);
		for (int i = 0; i < cnt; ++i) {
			if (pv.size() == 0 || context->rnd.random_double(0, 1) < FULL_MUTATION_PROB) {
				mv->push_back(P(context));
				continue;
			}
			int index = context->rnd.random_int(0, pv.size() - 1);
			P el = pv[index]->at(i);
			if (context->rnd.random_double(0, 1) < PART_MUTATION_PROB)
				el.mutate(context);
			mv->push_back(el);
		}
	}

};
