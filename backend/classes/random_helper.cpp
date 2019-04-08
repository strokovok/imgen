#pragma once
#include <random>

using namespace std;

class RandomHelper {

private:

	random_device rand_device;
	mt19937 rand_gen;

public:

	RandomHelper() {
		rand_gen = mt19937(rand_device());
	}

	int random_int(int l, int r) {
		uniform_int_distribution<> dis(l, r);
		return dis(rand_gen);
	}

	double random_double(double l, double r) {
		uniform_real_distribution<> dis(l, r);
		return dis(rand_gen);
	}

	template <typename T>
	T random_choice(vector<pair<T, double>> space) {
		for (int i = 1; i < space.size(); ++i)
			space[i].second += space[i - 1].second;
		double x = random_double(0, space.back().second);
		int i = 0;
		for (; !(space[i].second > x) && i + 1 < space.size(); ++i);
		return space[i].first;
	}

};
