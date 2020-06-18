#pragma once

#include <cmath>
#include <random>

const double PI = acos(-1);
const double eps = 1e-6;
const double INF = 1e20;

int gamma_trans(double x)
{
    return int(0.5 + 255 * pow(x < 0 ? 0. : x > 1 ? 1. : x, 1. / 2.2));
}

double sqr(double x) { return x * x; }

namespace utils
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0., 1.);
} // namespace utils

double rand01() { return utils::dis(utils::gen); }
