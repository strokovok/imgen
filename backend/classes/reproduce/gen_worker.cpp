#include <thread>

using namespace std;
using namespace json11;


template <class T, class C>
class GenWorker {

private:

	const int POPULATION_SIZE = 50;
	const int SPAWN_CNT = 100;

	const double LUCKY_CHANCE = 0.02;
	const double UNLUCKY_CHANCE = 0.02;

	C *context;

	vector<T*> population;
	T *realtime_best;
	long long iteration = 0;

	bool stopped = false;
	T *shared_best;
	long long shared_iteration = 0;
	mutex mtx;

	T *extracted_best;

	thread task_thread;

	void born_n(int cnt, vector<T*> &from_population) {
		for (int i = 0; i < cnt; ++i)
			population.push_back(new T(context, from_population));
	}

	T* get_best() {
		T* best = nullptr;
		for (auto &dude: population)
			best = (!best || dude->score > best->score) ? dude : best;
		return best;
	}

	void do_selection() {
		vector<pair<double, T*>> spawn;
		for (auto &dude: population) {
			double rank = dude->score;
			if (context->rnd.random_double(0, 1) < LUCKY_CHANCE)
				rank = 1e18;
			if (context->rnd.random_double(0, 1) < UNLUCKY_CHANCE)
				rank = -1e18;
			spawn.push_back(make_pair(-rank, dude));
		}
		population.clear();
		sort(spawn.begin(), spawn.end());
		for (int i = 0; i < spawn.size(); ++i)
			if (i < POPULATION_SIZE)
				population.push_back(spawn[i].second);
			else
				delete spawn[i].second;
	}

	void do_iteration() {
		vector<T*> old_population = population;
		born_n(SPAWN_CNT, old_population);
		do_selection();
		++iteration;
	}

	void work() {
		for (bool keep_working = true; keep_working;) {
			do_iteration();

			T *best = get_best();
			if (best->score > realtime_best->score) {
				delete realtime_best;
				realtime_best = new T(best);
			}
			T *put_best = new T(realtime_best);

			mtx.lock();
			swap(shared_best, put_best);
			keep_working &= !stopped;
			shared_iteration = iteration;
			mtx.unlock();

			delete put_best;
		}
	}

	void extract_best() {
		lock_guard<mutex> _(mtx);
		if (shared_best->score > extracted_best->score)
			swap(shared_best, extracted_best);
	}

public:

	GenWorker(C *context): context(context) {

		vector<T*> empty_population;
		born_n(POPULATION_SIZE, empty_population);
		T *best = get_best();
		realtime_best = new T(best);
		shared_best = new T(best);
		extracted_best = new T(best);

		task_thread = thread(&GenWorker::work, this);
	}

	Json get_result() {
		extract_best();
		return extracted_best->to_json();
	}

	long long get_iteration() {
		lock_guard<mutex> _(mtx);
		return shared_iteration;
	}

	double get_accuracy() {
		extract_best();
		return extracted_best->score / context->best_possible_score;
	}

	~GenWorker() {
		mtx.lock();
		stopped = true;
		mtx.unlock();

		task_thread.join();

		for (auto &dude: population)
			delete dude;

		delete realtime_best;
		delete shared_best;
		delete extracted_best;
		delete context;
	}

};
