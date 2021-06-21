//? #version 430

// https://www.shadertoy.com/view/Ms2cRK
/////////////////////////////////////////////////////////////////////////////////////////
const float Pi = 3.14159265359;
const float RcpPi = 1.0 / (1.0 * Pi);
const float RcpPi2 = 1.0 / (2.0 * Pi);
const float RcpPi4 = 1.0 / (4.0 * Pi);

float HapkeBRDF(float mu, float mu0, float a_ss)
{
	float s = sqrt(1.0 - a_ss);

	//vec3 H  = (1.0 + 2.0 * mu ) / (1.0 + 2.0 * mu  * s);
	//vec3 H0 = (1.0 + 2.0 * mu0) / (1.0 + 2.0 * mu0 * s);

	float HH0 = ((1.0 + 2.0 * mu) * (1.0 + 2.0 * mu0)) /
		((1.0 + 2.0 * mu * s) * (1.0 + 2.0 * mu0 * s));

	float R = a_ss * HH0 / (mu + mu0) * RcpPi4;

	return R;
}

float HapkeBRDF(float mu, float mu0, float a_ss, float phFunc)
{
	float s = sqrt(1.0 - a_ss);

	float HH0 = ((1.0 + 2.0 * mu) * (1.0 + 2.0 * mu0)) /
		((1.0 + 2.0 * mu * s) * (1.0 + 2.0 * mu0 * s));

	float R = a_ss * (HH0 - 1.0 + phFunc) / (mu + mu0) * RcpPi4;

	return R;
}

vec3 HapkeBRDF(float mu, float mu0, vec3 a_ss)
{
	return vec3(HapkeBRDF(mu, mu0, a_ss.r),
		HapkeBRDF(mu, mu0, a_ss.g),
		HapkeBRDF(mu, mu0, a_ss.b));
}

vec3 HapkeBRDF(float mu, float mu0, vec3 a_ss, float phFunc)
{
	return vec3(HapkeBRDF(mu, mu0, a_ss.r, phFunc),
		HapkeBRDF(mu, mu0, a_ss.g, phFunc),
		HapkeBRDF(mu, mu0, a_ss.b, phFunc));
}

vec3 HapkeBRDF(float mu, float mu0, vec3 a_ss, vec3 phFunc)
{
	return vec3(HapkeBRDF(mu, mu0, a_ss.r, phFunc.r),
		HapkeBRDF(mu, mu0, a_ss.g, phFunc.g),
		HapkeBRDF(mu, mu0, a_ss.b, phFunc.b));
}

float Pow2(float x) { return x * x; }

float PowL_Exp(float x, float s)
{
	return exp(s - s * x) * Pow2(exp(s * x) - 1.0) / Pow2(exp(s) - 1.0);
}

const float HapkeMaxAlbedo = (11.0 + 28.0 * log(2.0)) / 30.0;// 1.0136 (> 1 due to error in H approx)

float Albedo_to_SSAlbedo(float a)
{
	return 1.0 - PowL_Exp(a * (-1.0 / HapkeMaxAlbedo) + 1.0, 4.776);
}

vec3 Albedo_to_SSAlbedo(vec3 a)
{
	return vec3(Albedo_to_SSAlbedo(a.r),
		Albedo_to_SSAlbedo(a.g),
		Albedo_to_SSAlbedo(a.b));
}

/////////////////////////////////////////////////////////////////////////////////////////