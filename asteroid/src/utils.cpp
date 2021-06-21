#include <random>

inline float biasedDistribution(float bias) {
	std::random_device rd{};
	std::mt19937 gen{ rd() };

	std::uniform_real_distribution<float> s{ 0.0f, 1.0f };
	float r = s(gen);

	float k = pow(1 - bias, 3);
	return (r * k) / (r * k - r + 1);
}