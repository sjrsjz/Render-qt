#pragma once
#include <math.h>
class SmoothMove {
private:
	double v0{};
	double v1{};
	double x0{};
	double x1{};
	double dx{};
	double startTime{};
	double TotalDuration{};
public:
	void NewEndPositon(double x, double time) {
		x0 = x1;
		v0 = v1;
		dx = x - x0;
		startTime = time;
	}
	void SetStartPosition(double x, double time) {
		x0 = x;
		x1 = x;
		v0 = 0;
		dx = 0;
		startTime = time;
	}
	void SetTotalDuration(double time) {
		TotalDuration = time;
	}
	void Update(double time) {
		double t = time - startTime;
		if (t >= TotalDuration) {
			x1 = x0 + dx;
			v1 = 0;
			return;
		}
		const double _1_div_exp_2 = 1.0 / exp(2.0);
		x1 = x0 + t * (v0 + (v0 - dx / TotalDuration) / (1 - _1_div_exp_2) * (exp(-2 * t / TotalDuration) - 1));
		v1 = ((TotalDuration - 2 * t) * (v0 * TotalDuration - dx) * exp(-2 * t / TotalDuration) + TotalDuration * dx - v0 * TotalDuration * TotalDuration * _1_div_exp_2) / (TotalDuration * TotalDuration * (1 - _1_div_exp_2));
	}
	double X() {
		return x1;
	}
};