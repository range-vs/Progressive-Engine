#include "Common.h"

namespace gmath
{
	const float GeneralMath::PI(3.14159265358979323846f);
	const float GeneralMath::_2PI(6.2831853071796f);
	const float GeneralMath::_4PI(12.566370614359f);
	const float GeneralMath::PI_2(1.5707963267949f);
	const float GeneralMath::PI_4(0.78539816339745f);

	float GeneralMath::degreesToRadians(const float & d)
	{
		return d * (GeneralMath::PI / 180.f);
	}

	float GeneralMath::radiansToDigrees(const float & r)
	{
		return r * (180.f / GeneralMath::PI);
	}

	float GeneralMath::clamp(float value, float min, float max)
	{
		return fmax(min, fmin(value, max));
	}

	Color Colors::black(0, 0, 0, 1);
	Color Colors::blue(0, 0, 1, 1);
	Color Colors::green(0, 1, 0, 1);
	Color Colors::red(1, 0, 0, 1);
	Color Colors::white(1, 1, 1, 1);

}