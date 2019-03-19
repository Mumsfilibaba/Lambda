/*////////////////////////////////////////////////////////////

Copyright 2018 Alexander Dahlin

Licensed under the Apache License, Version 2.0 (the
"License"); you may not use this file except in
compliance with the License. You may obtain a copy of
the License at

http ://www.apache.org/licenses/LICENSE-2.0

THIS SOFTWARE IS PROVIDED "AS IS". MEANING NO WARRANTY
OR SUPPORT IS PROVIDED OF ANY KIND.

In event of any damages, direct or indirect that can
be traced back to the use of this software, shall no
contributor be held liable. This includes computer
failure and or malfunction of any kind.

////////////////////////////////////////////////////////////*/

#pragma once
#include "RayEngine.h"

#define PI 3.1415926535898
#define E 2.7182818284590
#define PI_F 3.141592f
#define E_f 2.718281f
#define PIx2 2.0 * PI
#define PIx2_F 2.0f * PI_F

namespace Math
{
	double ToDegrees(double degrees);

	double ToRadians(double radians);

	float ToDegreesF(float degrees);
	
	float ToRadiansF(float radians);

	double Max(double d1, double d2);
	
	float MaxF(float f1, float f2);
	
	double Min(double d1, double d2);
	
	float MinF(float f1, float f2);
}