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
#include "Vector3.h"

namespace Math
{
	class RAYENGINE_API Ray
	{
	public:
		Ray(const Vector3& origin = Vector3(), const Vector3& direction = Vector3());
		Ray(const Ray& other);

		Vector3 GetPoint(float rayLength) const;
		bool Equals(const Ray& other) const;

		std::string ToString() const;

		Ray& operator=(const Ray& other);
		bool operator==(const Ray& other) const;
		bool operator!=(const Ray& other) const;

		void Origin(const Math::Vector3& origin);
		Vector3& Origin();
		const Vector3& Origin() const;
		void Direction(const Math::Vector3& direction);
		Vector3& Direction();
		const Vector3& Direction() const;

	private:
		Vector3 m_Origin;
		Vector3 m_Direction;
	};
}