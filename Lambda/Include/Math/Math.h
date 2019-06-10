#pragma once
#include "Float2.h"
#include "Float3.h"
#include "Float4.h"

#include "Int2.h"
#include "Int3.h"
#include "Int4.h"

#include "Float2x2.h"
#include "Float3x3.h"
#include "Float4x4.h"

#include "Color.h"

#include "RandomGenerator.h"
#include "Rectangle.h"
#include "MathUtilities.h"

//LOG_SYSTEM_PRINT("--TESTING FLOAT2--");
//Float2 f2 = Float2(2.0f);
//Float2 f22 = Float2(3.0f, 4.0f);
//LOG_SYSTEM_PRINT("Expect: 2.0f, 2.0f - %s\n", ToString(f2).c_str());
//LOG_SYSTEM_PRINT("Expect: 3.0f, 4.0f - %s\n", ToString(f22).c_str());
//f2.SetX(5.0f);
//f2.SetY(6.0f);
//LOG_SYSTEM_PRINT("Expect: 5.0f, 6.0f - %s\n", ToString(f2).c_str());
//LOG_SYSTEM_PRINT("Expect: 5.0f, 6.0f - (%.6f, %.6f)\n", f2.GetX(), f2.GetY());
//
//LOG_SYSTEM_PRINT("Expect: 8.0f, 10.0f - %s\n", ToString(f2.Add(f22)).c_str());
//LOG_SYSTEM_PRINT("Expect: 9.0f, 11.0f - %s\n", ToString(f2.Add(1.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect: 4.0f, 6.0f - %s\n", ToString(f2.Subtract(Float2(5.0f, 5.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect: 3.0f, 5.0f - %s\n", ToString(f2.Subtract(1.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect: 6.0f, 15.0f - %s\n", ToString(f2.Multiply(Float2(2.0f, 3.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect: 12.0f, 30.0f - %s\n", ToString(f2.Multiply(2.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect: 6.0f, 15.0f - %s\n", ToString(f2.Divide(2.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect: 6.0f, 5.0f - %s\n", ToString(f2.Divide(Float2(1.0f, 3.0f))).c_str());
//
//LOG_SYSTEM_PRINT("Expect: false - ");
//if (f2.IsUnitVector())
//{
//	LOG_SYSTEM_PRINT("true\n");
//}
//else
//{
//	LOG_SYSTEM_PRINT("false\n");
//}
//
//LOG_SYSTEM_PRINT("Expect: false - ");
//if (f2.Equals(f22))
//{
//	LOG_SYSTEM_PRINT("true\n");
//}
//else
//{
//	LOG_SYSTEM_PRINT("false\n");
//}
//
//LOG_SYSTEM_PRINT("Expect: 38.0f - %.6f\n", f2.Dot(f22));
//LOG_SYSTEM_PRINT("Expect: 61.0f - %.6f\n", f2.LengthSqrd());
//LOG_SYSTEM_PRINT("Expect: 7.81024f - %.6f\n", f2.Length());
//
//LOG_SYSTEM_PRINT("Expect: 0.7682f, 0.6402f - %s\n", ToString(f2.GetNormalized()).c_str());
//
//LOG_SYSTEM_PRINT("Expect: -5.0f, 6.0f - %s\n", ToString(f2.Normal()).c_str());
//LOG_SYSTEM_PRINT("Expect: 3.7377f, 3.1147f - %s\n", ToString(f2.Project(f22)).c_str());
//LOG_SYSTEM_PRINT("Expect: 6.0f, -5.0f - %s\n", ToString(f2.Reflect(Float2(0.0f, 1.0f))).c_str());
//
//LOG_SYSTEM_PRINT("Expect: 2.5f, 4.5f - %s\n", ToString(Clamp(Float2(2.0f, 5.0f), Float2(2.5f), Float2(4.5f))).c_str());
//LOG_SYSTEM_PRINT("Expect: 1.625f, 1.625f - %s\n", ToString(Lerp(0.25f, Float2(1.0f), Float2(3.5f))).c_str());
//
//LOG_SYSTEM_PRINT("\n--TESTING FLOAT3--\n");
//Float3 f3 = Float3(2.0f);
//Float3 f32 = Float3(3.0f, 4.0f, 5.0f);
//LOG_SYSTEM_PRINT("Expect: 2.0f, 2.0f, 2.0f - %s\n", ToString(f3).c_str());
//LOG_SYSTEM_PRINT("Expect: 3.0f, 4.0f, 5.0f - %s\n", ToString(f32).c_str());
//f3.SetX(5.0f);
//f3.SetY(6.0f);
//f3.SetZ(7.0f);
//LOG_SYSTEM_PRINT("Expect: 5.0f, 6.0f, 7.0f - %s\n", ToString(f3).c_str());
//LOG_SYSTEM_PRINT("Expect: 5.0f, 6.0f, 7.0f - (%.6f, %.6f, %.6f)\n", f3.GetX(), f3.GetY(), f3.GetZ());
//
//LOG_SYSTEM_PRINT("Expect: 8.0f, 10.0f, 12.0f - %s\n", ToString(f3.Add(f32)).c_str());
//LOG_SYSTEM_PRINT("Expect: 9.0f, 11.0f, 13.0f - %s\n", ToString(f3.Add(1.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect: 4.0f, 6.0f, 8.0f- %s\n", ToString(f3.Subtract(Float3(5.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect: 3.0f, 5.0f, 7.0f - %s\n", ToString(f3.Subtract(1.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect: 6.0f, 15.0f, -14.0f - %s\n", ToString(f3.Multiply(Float3(2.0f, 3.0f, -2.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect: 12.0f, 30.0f, -28.0f - %s\n", ToString(f3.Multiply(2.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect: 6.0f, 15.0f, -14.0f - %s\n", ToString(f3.Divide(2.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect: 6.0f, 5.0f, 7.0f - %s\n", ToString(f3.Divide(Float3(1.0f, 3.0f, -2.0f))).c_str());
//
//LOG_SYSTEM_PRINT("Expect: false - ");
//if (f3.IsUnitVector())
//{
//	LOG_SYSTEM_PRINT("true\n");
//}
//else
//{
//	LOG_SYSTEM_PRINT("false\n");
//}
//
//LOG_SYSTEM_PRINT("Expect: false - ");
//if (f3.Equals(f32))
//{
//	LOG_SYSTEM_PRINT("true\n");
//}
//else
//{
//	LOG_SYSTEM_PRINT("false\n");
//}
//
//LOG_SYSTEM_PRINT("Expect: 73.0f - %.6f\n", f3.Dot(f32));
//LOG_SYSTEM_PRINT("Expect: 110.0f - %.6f\n", f3.LengthSqrd());
//LOG_SYSTEM_PRINT("Expect: 10.4880f - %.6f\n", f3.Length());
//
//LOG_SYSTEM_PRINT("Expect: 0.5720f, 0.4767f, 0.6674f - %s\n", ToString(f3.GetNormalized()).c_str());
//
//LOG_SYSTEM_PRINT("Expect: -3.0f, -9.0f, 9.0f - %s\n", ToString(f3.Cross(f32)).c_str());
//LOG_SYSTEM_PRINT("Expect: 3.9818f, 3.3181f, 4.6454f - %s\n", ToString(f3.Project(f32)).c_str());
//LOG_SYSTEM_PRINT("Expect: 6.0f, -5.0f, 7.0f - %s\n", ToString(f3.Reflect(Float3(0.0f, 1.0f, 0.0f))).c_str());
//
//LOG_SYSTEM_PRINT("Expect: 2.5f, 4.5f, 2.5f - %s\n", ToString(Clamp(Float3(2.0f, 5.0f, -5.0f), Float3(2.5f), Float3(4.5f))).c_str());
//LOG_SYSTEM_PRINT("Expect: 1.625f, 1.625f, 1.625f - %s\n", ToString(Lerp(0.25f, Float3(1.0f), Float3(3.5f))).c_str());
//
//LOG_SYSTEM_PRINT("\n--TESTING FLOAT4--\n");
//Float4 f4 = Float4(2.0f);
//Float4 f42 = Float4(3.0f, 4.0f, 5.0f, 6.0f);
//LOG_SYSTEM_PRINT("Expect: 2.0f, 2.0f, 2.0f, 2.0f - %s\n", ToString(f4).c_str());
//LOG_SYSTEM_PRINT("Expect: 3.0f, 4.0f, 5.0f, 6.0f - %s\n", ToString(f42).c_str());
//f4.SetX(5.0f);
//f4.SetY(6.0f);
//f4.SetZ(7.0f);
//f4.SetW(8.0f);
//LOG_SYSTEM_PRINT("Expect: 5.0f, 6.0f, 7.0f, 8.0f - %s\n", ToString(f4).c_str());
//LOG_SYSTEM_PRINT("Expect: 5.0f, 6.0f, 7.0f, 8.0f - (%.6f, %.6f, %.6f, %.6f)\n", f4.GetX(), f4.GetY(), f4.GetZ(), f4.GetW());
//
//LOG_SYSTEM_PRINT("Expect: 8.0f, 10.0f, 12.0f, 14.0f - %s\n", ToString(f4.Add(f42)).c_str());
//LOG_SYSTEM_PRINT("Expect: 9.0f, 11.0f, 13.0f, 15.0f - %s\n", ToString(f4.Add(1.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect: 4.0f, 6.0f, 8.0f, 10.0f- %s\n", ToString(f4.Subtract(Float4(5.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect: 3.0f, 5.0f, 7.0f, 9.0f- %s\n", ToString(f4.Subtract(1.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect: 6.0f, 15.0f, -14.0f, 4.5f - %s\n", ToString(f4.Multiply(Float4(2.0f, 3.0f, -2.0f, 0.5f))).c_str());
//LOG_SYSTEM_PRINT("Expect: 12.0f, 30.0f, -28.0f, 9.0f - %s\n", ToString(f4.Multiply(2.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect: 6.0f, 15.0f, -14.0f, 4.5f - %s\n", ToString(f4.Divide(2.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect: 6.0f, 5.0f, 7.0f, 2.25f - %s\n", ToString(f4.Divide(Float4(1.0f, 3.0f, -2.0f, 2.0f))).c_str());
//
//LOG_SYSTEM_PRINT("Expect: false - ");
//if (f4.IsUnitVector())
//{
//	LOG_SYSTEM_PRINT("true\n");
//}
//else
//{
//	LOG_SYSTEM_PRINT("false\n");
//}
//
//LOG_SYSTEM_PRINT("Expect: false - ");
//if (f4.Equals(f42))
//{
//	LOG_SYSTEM_PRINT("true\n");
//}
//else
//{
//	LOG_SYSTEM_PRINT("false\n");
//}
//
//LOG_SYSTEM_PRINT("Expect: 86.5f - %.6f\n", f4.Dot(f42));
//LOG_SYSTEM_PRINT("Expect: 115.0625f - %.6f\n", f4.LengthSqrd());
//LOG_SYSTEM_PRINT("Expect: 10.7267f - %.6f\n", f4.Length());
//
//LOG_SYSTEM_PRINT("Expect: 0.5593f, 0.4661f, 0.6525f, 0.2097f - %s\n", ToString(f4.GetNormalized()).c_str());
//
//LOG_SYSTEM_PRINT("Expect: 4.5105f, 3.7588f, 5.2623f, 1.6914f - %s\n", ToString(f4.Project(f42)).c_str());
//LOG_SYSTEM_PRINT("Expect: 6.0f, -5.0f, 7.0f, 2.25f - %s\n", ToString(f4.Reflect(Float4(0.0f, 1.0f, 0.0f, 0.0f))).c_str());
//
//LOG_SYSTEM_PRINT("Expect: 2.5f, 4.5f, 2.5f, 4.0f - %s\n", ToString(Clamp(Float4(2.0f, 5.0f, -5.0, 4.0f), Float4(2.5f), Float4(4.5f))).c_str());
//LOG_SYSTEM_PRINT("Expect: 1.625f, 1.625f, 1.625f, 1.625f - %s\n", ToString(Lerp(0.25f, Float4(1.0f), Float4(3.5f))).c_str());
//
//LOG_SYSTEM_PRINT("\n--TESTING INT2--\n");
//Int2 i2 = Int2(2);
//Int2 i22 = Int2(3, 4);
//LOG_SYSTEM_PRINT("Expect: 2, 2 - %s\n", ToString(i2).c_str());
//LOG_SYSTEM_PRINT("Expect: 3, 4 - %s\n", ToString(i22).c_str());
//i2.SetX(5);
//i2.SetY(6);
//LOG_SYSTEM_PRINT("Expect: 5, 6 - %s\n", ToString(i2).c_str());
//LOG_SYSTEM_PRINT("Expect: 5, 6 - (%d, %d)\n", i2.GetX(), i2.GetY());
//
//LOG_SYSTEM_PRINT("Expect: 8, 10 - %s\n", ToString(i2.Add(i22)).c_str());
//LOG_SYSTEM_PRINT("Expect: 9, 11 - %s\n", ToString(i2.Add(1)).c_str());
//LOG_SYSTEM_PRINT("Expect: 4, 6 - %s\n", ToString(i2.Subtract(Int2(5, 5))).c_str());
//LOG_SYSTEM_PRINT("Expect: 3, 5 - %s\n", ToString(i2.Subtract(1)).c_str());
//
//LOG_SYSTEM_PRINT("Expect: false - ");
//if (i2.Equals(i22))
//{
//	LOG_SYSTEM_PRINT("true\n");
//}
//else
//{
//	LOG_SYSTEM_PRINT("false\n");
//}
//
//LOG_SYSTEM_PRINT("\n--TESTING INT3--\n");
//Int3 i3 = Int3(2);
//Int3 i32 = Int3(3, 4, 5);
//LOG_SYSTEM_PRINT("Expect: 2, 2, 2 - %s\n", ToString(i3).c_str());
//LOG_SYSTEM_PRINT("Expect: 3, 4, 5 - %s\n", ToString(i32).c_str());
//i3.SetX(5);
//i3.SetY(6);
//i3.SetZ(7);
//LOG_SYSTEM_PRINT("Expect: 5, 6, 7 - %s\n", ToString(i3).c_str());
//LOG_SYSTEM_PRINT("Expect: 5, 6, 7 - (%d, %d, %d)\n", i3.GetX(), i3.GetY(), i3.GetZ());
//
//LOG_SYSTEM_PRINT("Expect: 8, 10, 12 - %s\n", ToString(i3.Add(i32)).c_str());
//LOG_SYSTEM_PRINT("Expect: 9, 11, 13- %s\n", ToString(i3.Add(1)).c_str());
//LOG_SYSTEM_PRINT("Expect: 4, 9, 9 - %s\n", ToString(i3.Subtract(Int3(5, 2, 4))).c_str());
//LOG_SYSTEM_PRINT("Expect: 3, 8, 8 - %s\n", ToString(i3.Subtract(1)).c_str());
//
//LOG_SYSTEM_PRINT("Expect: false - ");
//if (i3.Equals(i32))
//{
//	LOG_SYSTEM_PRINT("true\n");
//}
//else
//{
//	LOG_SYSTEM_PRINT("false\n");
//}
//
//LOG_SYSTEM_PRINT("\n--TESTING INT4--\n");
//Int4 i4 = Int4(2);
//Int4 i42 = Int4(3, 4, 5, 6);
//LOG_SYSTEM_PRINT("Expect: 2, 2, 2, 2 - %s\n", ToString(i4).c_str());
//LOG_SYSTEM_PRINT("Expect: 3, 4, 5, 6 - %s\n", ToString(i42).c_str());
//i4.SetX(5);
//i4.SetY(6);
//i4.SetZ(7);
//i4.SetW(8);
//LOG_SYSTEM_PRINT("Expect: 5, 6, 7, 8 - %s\n", ToString(i4).c_str());
//LOG_SYSTEM_PRINT("Expect: 5, 6, 7, 8 - (%d, %d, %d, %d)\n", i4.GetX(), i4.GetY(), i4.GetZ(), i4.GetW());
//
//LOG_SYSTEM_PRINT("Expect: 8, 10, 12, 14 - %s\n", ToString(i4.Add(i42)).c_str());
//LOG_SYSTEM_PRINT("Expect: 9, 11, 13, 15- %s\n", ToString(i4.Add(1)).c_str());
//LOG_SYSTEM_PRINT("Expect: 4, 9, 9, 18 - %s\n", ToString(i4.Subtract(Int4(5, 2, 4, -3))).c_str());
//LOG_SYSTEM_PRINT("Expect: 3, 8, 8, 17 - %s\n", ToString(i4.Subtract(1)).c_str());
//
//LOG_SYSTEM_PRINT("Expect: false - ");
//if (i4.Equals(i42))
//{
//	LOG_SYSTEM_PRINT("true\n");
//}
//else
//{
//	LOG_SYSTEM_PRINT("false\n");
//}
//
//LOG_SYSTEM_PRINT("\n--TESTING FLOAT2X2--\n");
//Float2 f2 = Float2(2.0f, 5.0);
//Float2x2 f2x2 = Float2x2(2.0f);
//Float2x2 f2x22 = Float2x2(Float2(1.0f, 2.0f), Float2(3.0f, 4.0f));
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 0.0f]\n[0.0f, 2.0f]\n%s", ToString(f2x2).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 2.0f]\n[3.0f, 4.0f]\n%s", ToString(f2x22).c_str());
//LOG_SYSTEM_PRINT("Expect: 17.0f, 24.0f - %s\n", ToString(f2x22.Multiply(f2)).c_str());
//
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 4.0f]\n[6.0f, 8.0f]\n%s", ToString(f2x22.Multiply(f2x2)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[4.0f, 8.0f]\n[12.0f, 16.0f]\n%s", ToString(f2x22.Multiply(2.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[6.0f, 8.0f]\n[12.0f, 18.0f]\n%s", ToString(f2x22.Add(f2x2)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[11.0f, 13.0f]\n[17.0f, 23.0f]\n%s", ToString(f2x22.Add(5.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[9.0f, 13.0f]\n[17.0f, 21.0f]\n%s", ToString(f2x22.Subtract(f2x2)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 6.0f]\n[10.0f, 14.0f]\n%s", ToString(f2x22.Subtract(7.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 3.0f]\n[5.0f, 7.0f]\n%s", ToString(f2x22.Divide(2.0f)).c_str());
//
//LOG_SYSTEM_PRINT("Expect: false - ");
//if (f2x22.Equals(f2x2))
//{
//	LOG_SYSTEM_PRINT("true\n");
//}
//else
//{
//	LOG_SYSTEM_PRINT("false\n");
//}
//
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 5.0f]\n[3.0f, 7.0f]\n%s", ToString(f2x22.GetTranspose()).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[-0.875f, 0.375f]\n[0.625f, -0.125f]\n%s", ToString(f2x22.GetInverse()).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[7.0f, -5.0f]\n[-3.0f, 1.0f]\n%s", ToString(f2x22.Cofactor()).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[7.0f, -3.0f]\n[-5.0f, 1.0f]\n%s", ToString(f2x22.Adjugate()).c_str());
//LOG_SYSTEM_PRINT("Expect: -8.0f - %.6f\n", f2x22.Determinant());
//
//f2x22.SetRow(0, 2.0f, 4.0f);
//f2x22.SetRow(1, 5.0f, 3.0f);
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 4.0f]\n[5.0f, 3.0f]\n%s\n%s\n", ToString(f2x22.GetRow(0)).c_str(), ToString(f2x22.GetRow(1)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 4.0f]\n[5.0f, 3.0f]\n%.6f, %.6f\n%.6f, %.6f\n", f2x22.GetElement(0, 0), f2x22.GetElement(0, 1), f2x22.GetElement(1, 0), f2x22.GetElement(1, 1));
//
//LOG_SYSTEM_PRINT("Expect:\n[0.5f, 0.0f]\n[0.0f, 0.75f]\n%s", ToString(Float2x2::Scale(Vec2f(0.5f, 0.75f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 0.0f]\n[0.0f, 2.0f]\n%s", ToString(Float2x2::Scale(2.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f]\n[0.0f, 1.0f]\n%s", ToString(Float2x2::Identity()).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.8660f, 0.5f]\n[-0.5f, 0.8660f]\n%s", ToString(Float2x2::Rotation(ToRadiansF(30.0f))).c_str());

//LOG_SYSTEM_PRINT("\n--TESTING FLOAT3X3--\n");
//Float3 f3 = Float3(6.0f, 5.0f, 7.0f);
//Float3x3 f3x3 = Float3x3(2.0f);
//Float3x3 f3x32 = Float3x3(Float3(1.0f, 2.0f, 3.0f), Float3(5.0f, 6.0f, 7.0f), Float3(9.0f, 10.0f, 11.0f));
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 0.0f, 0.0f]\n[0.0f, 2.0f, 0.0f]\n[0.0f, 0.0f, 2.0f]\n%s", ToString(f3x3).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 2.0f, 3.0f]\n[5.0f, 6.0f, 7.0f]\n[9.0f, 10.0f, 11.0f]\n%s", ToString(f3x32).c_str());
//LOG_SYSTEM_PRINT("Expect: 94.0f, 112.0f, 130.0f - %s\n", ToString(f3x32.Multiply(f3)).c_str());
//
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 4.0f, 6.0f]\n[10.0f, 12.0f, 14.0f]\n[18.0f, 20.0f, 22.0f]\n%s", ToString(f3x32.Multiply(f3x3)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[4.0f, 8.0f, 12.0f]\n[20.0f, 24.0f, 28.0f]\n[36.0f, 40.0f, 44.0f]\n%s", ToString(f3x32.Multiply(2.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[6.0f, 8.0f, 12.0f]\n[20.0f, 26.0f, 28.0f]\n[36.0f, 40.0f, 46.0f]\n%s", ToString(f3x32.Add(f3x3)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[11.0f, 13.0f, 17.0f]\n[25.0f, 31.0f, 33.0f]\n[41.0f, 45.0f, 51.0f]\n%s", ToString(f3x32.Add(5.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[9.0f, 13.0f, 17.0f]\n[25.0f, 29.0f, 33.0f]\n[41.0f, 45.0f, 49.0f]\n%s", ToString(f3x32.Subtract(f3x3)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 6.0f, 10.0f]\n[18.0f, 22.0f, 26.0f]\n[34.0f, 38.0f, 42.0f]\n%s", ToString(f3x32.Subtract(7.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 3.0f, 5.0f]\n[9.0f, 11.0f, 13.0f]\n[17.0f, 19.0f, 21.0f]\n%s", ToString(f3x32.Divide(2.0f)).c_str());
//
//LOG_SYSTEM_PRINT("Expect: false - ");
//if (f3x32.Equals(f3x3))
//{
//	LOG_SYSTEM_PRINT("true\n");
//}
//else
//{
//	LOG_SYSTEM_PRINT("false\n");
//}
//
//Float3x3 mat = Float3x3(Float3(1.0f, 3.0f, 5.0f), Float3(9.0f, 11.0f, 13.0f), Float3(17.0f, 19.5f, 21.0f));
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 9.0f, 17.0f]\n[3.0f, 11.0f, 19.5f]\n[5.0f, 13.0f, 21.0f]\n%s", ToString(mat.GetTranspose()).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[-1.40625f, 2.15625f, -1.0f]\n[2.0f, -4.0f, 2.0f]\n[-0.71875f, 1.96875f, -1.0f]\n%s", ToString(mat.GetInverse()).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[-22.5f, 32.0f, -11.5f]\n[34.5f, -64.0f, 31.5f]\n[-16.0f, 32.0f, -16.0f]\n%s", ToString(mat.Cofactor()).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[-22.5f, 34.5f, -16.0f]\n[32.0f, -64.0f, 32.0f]\n[-11.5f, 31.5f, -16.0f]\n%s", ToString(mat.Adjugate()).c_str());
//LOG_SYSTEM_PRINT("Expect: 16.0f - %.6f\n", f3x32.Determinant());
//
//f3x32.SetRow(0, 2.0f, 4.0f, 6.0f);
//f3x32.SetRow(1, 5.0f, 3.0f, 1.0f);
//f3x32.SetRow(2, -2.0f, 0.0f, 2.0f);
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 4.0f, 6.0f]\n[5.0f, 3.0f, 1.0f]\n[-2.0f, 0.0f, 2.0f]\n%s\n%s\n%s\n", ToString(f3x32.GetRow(0)).c_str(), ToString(f3x32.GetRow(1)).c_str(), ToString(f3x32.GetRow(2)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 4.0f, 6.0f]\n[5.0f, 3.0f, 1.0f]\n[-2.0f, 0.0f, 2.0f]\n%.6f, %.6f, %.6f\n%.6f, %.6f, %.6f\n%.6f, %.6f, %.6f\n", f3x32.GetElement(0, 0), f3x32.GetElement(0, 1), f3x32.GetElement(0, 2),
//	f3x32.GetElement(1, 0), f3x32.GetElement(1, 1), f3x32.GetElement(1, 2), f3x32.GetElement(2, 0), f3x32.GetElement(2, 1), f3x32.GetElement(2, 2));
//
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f]\n[0.0f, 1.0f, 0.0f]\n[0.0f, 0.0f, 1.0f]\n%s", ToString(Float3x3::Identity()).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f]\n[0.0f, 1.0f, 0.0f]\n[-1.0f, 5.0f, 1.0f]\n%s", ToString(Float3x3::Translation(Vec2f(-1.0f, 5.0f))).c_str());
//
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f]\n[0.0f, 0.6427f, 0.7660f]\n[0.0f, -0.7660f, 0.6427f]\n%s", ToString(Float3x3::RotationX(ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.0f, -0.7660f]\n[0.0f, 1.0f, 0.0f]\n[0.7660f, 0.0f, 0.6427f]\n%s", ToString(Float3x3::RotationY(ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.7660f, 0.0f]\n[-0.7660f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 1.0f]\n%s", ToString(Float3x3::RotationZ(ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f]\n[0.0f, 0.6427f, 0.7660f]\n[0.0f, -0.7660f, 0.6427f]\n%s", ToString(Float3x3::Rotation(Vec3f(1.0f, 0.0f, 0.0f), ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.0f, -0.7660f]\n[0.0f, 1.0f, 0.0f]\n[0.7660f, 0.0f, 0.6427f]\n%s", ToString(Float3x3::Rotation(Vec3f(0.0f, 1.0f, 0.0f), ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.7660f, 0.0f]\n[-0.7660f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 1.0f]\n%s", ToString(Float3x3::Rotation(Vec3f(0.0f, 0.0f, 1.0f), ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f]\n[0.0f, 0.6427f, 0.7660f]\n[0.0f, -0.7660f, 0.6427f]\n%s", ToString(Float3x3::RotationRollPitchYaw(0.0f, ToRadiansF(50.0f), 0.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.0f, -0.7660f]\n[0.0f, 1.0f, 0.0f]\n[0.7660f, 0.0f, 0.6427f]\n%s", ToString(Float3x3::RotationRollPitchYaw(0.0f, 0.0f, ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.7660f, 0.0f]\n[-0.7660f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 1.0f]\n%s", ToString(Float3x3::RotationRollPitchYaw(ToRadiansF(50.0f), 0.0f, 0.0f)).c_str());
//
//LOG_SYSTEM_PRINT("Expect:\n[0.6683f, -0.3329f, -0.6652f]\n[0.4860f, 0.8724f, 0.0516f]\n[0.5631f, -0.3578f, 0.7448f]\n%s", ToString(Float3x3::Rotation(Vec3f(0.5f, 1.5f, -1.0f), ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.5f, 0.0f, 0.0f]\n[0.0f, 0.75f, 0.0f]\n[0.0f, 0.0f, 2.0f]\n%s", ToString(Float3x3::Scale(Vec3f(0.5f, 0.75f, 2.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 0.0f, 0.0f]\n[0.0f, 2.0f, 0.0f]\n[0.0f, 0.0f, 2.0f]\n%s", ToString(Float3x3::Scale(2.0f)).c_str());

//LOG_SYSTEM_PRINT("\n--TESTING FLOAT4X4--\n");
//Float4 f4 = Float4(6.0f, 5.0f, 7.0f, 1.0f);
//Float4x4 f4x4 = Float4x4(2.0f);
//Float4x4 f4x42 = Float4x4(Float4(1.0f, 2.0f, 3.0f, 4.0f), Float4(5.0f, 6.0f, 7.0f, 8.0f), Float4(9.0f, 10.0f, 11.0f, 12.0f), Float4(13.0f, 14.0f, 15.0f, 16.0f));
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 2.0f, 0.0f, 0.0f]\n[0.0f, 0.0f, 2.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 2.0f]\n%s", ToString(f4x4).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 2.0f, 3.0f, 4.0f]\n[5.0f, 6.0f, 7.0f, 8.0f]\n[9.0f, 10.0f, 11.0f, 12.0f]\n[13.0f, 14.0f, 15.0f, 16.0f]\n%s", ToString(f4x42).c_str());
//LOG_SYSTEM_PRINT("Expect: 107.0f, 126.0f, 145.0f, 164.0f - %s\n", ToString(f4x42.Multiply(f4)).c_str());
//
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 4.0f, 6.0f, 8.0f]\n[10.0f, 12.0f, 14.0f, 16.0f]\n[18.0f, 20.0f, 22.0f, 24.0f]\n[26.0f, 28.0f, 30.0f, 32.0f]\n%s", ToString(f4x42.Multiply(f4x4)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[4.0f, 8.0f, 12.0f, 16.0f]\n[20.0f, 24.0f, 28.0f, 32.0f]\n[36.0f, 40.0f, 44.0f, 48.0f]\n[52.0f, 56.0f, 60.0f, 64.0f]\n%s", ToString(f4x42.Multiply(2.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[6.0f, 8.0f, 12.0f, 16.0f]\n[20.0f, 26.0f, 28.0f, 32.0f]\n[36.0f, 40.0f, 46.0f, 48.0f]\n[52.0f, 56.0f, 60.0f, 66.0f]\n%s", ToString(f4x42.Add(f4x4)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[11.0f, 13.0f, 17.0f, 21.0f]\n[25.0f, 31.0f, 33.0f, 37.0f]\n[41.0f, 45.0f, 51.0f, 53.0f]\n[57.0f, 61.0f, 65.0f, 71.0f]\n%s", ToString(f4x42.Add(5.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[9.0f, 13.0f, 17.0f, 21.0f]\n[25.0f, 29.0f, 33.0f, 37.0f]\n[41.0f, 45.0f, 49.0f, 53.0f]\n[57.0f, 61.0f, 65.0f, 69.0f]\n%s", ToString(f4x42.Subtract(f4x4)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 6.0f, 10.0f, 14.0f]\n[18.0f, 22.0f, 26.0f, 30.0f]\n[34.0f, 38.0f, 42.0f, 46.0f]\n[50.0f, 54.0f, 58.0f, 62.0f]\n%s", ToString(f4x42.Subtract(7.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 3.0f, 5.0f, 7.0f]\n[9.0f, 11.0f, 13.0f, 15.0f]\n[17.0f, 19.0f, 21.0f, 23.0f]\n[25.0f, 27.0f, 29.0f, 31.0f]\n%s", ToString(f4x42.Divide(2.0f)).c_str());
//
//LOG_SYSTEM_PRINT("Expect: false - ");
//if (f4x42.Equals(f4x4))
//{
//	LOG_SYSTEM_PRINT("true\n");
//}
//else
//{
//	LOG_SYSTEM_PRINT("false\n");
//}
//
//Float4x4 mat;
//mat.SetRow(0, 2.0f, 4.0f, 6.0f, -4.0f);
//mat.SetRow(1, 5.0f, 3.0f, 1.0f, 2.0f);
//mat.SetRow(2, -2.0f, 0.0f, 2.0f, 3.5f);
//mat.SetRow(3, 2.0f, -1.0f, 2.0f, 3.5f);
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 4.0f, 6.0f, -4.0f]\n[5.0f, 3.0f, 1.0f, 2.0f]\n[-2.0f, 0.0f, 2.0f, 3.5f]\n[2.0f, -1.0f, 2.0f, 3.5f]\n%s\n%s\n%s\n%s\n",
//	ToString(mat.GetRow(0)).c_str(), ToString(mat.GetRow(1)).c_str(), ToString(mat.GetRow(2)).c_str(), ToString(mat.GetRow(3)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 4.0f, 6.0f, -4.0f]\n[5.0f, 3.0f, 1.0f, 2.0f]\n[-2.0f, 0.0f, 2.0f, 3.5f]\n[2.0f, -1.0f, 2.0f, 3.5f]\n%.6f, %.6f, %.6f, %.6f\n%.6f, %.6f, %.6f, %.6f\n%.6f, %.6f, %.6f, %.6f\n%.6f, %.6f, %.6f, %.6f\n",
//	mat.GetElement(0, 0), mat.GetElement(0, 1), mat.GetElement(0, 2), mat.GetElement(0, 3),
//	mat.GetElement(1, 0), mat.GetElement(1, 1), mat.GetElement(1, 2), mat.GetElement(1, 3),
//	mat.GetElement(2, 0), mat.GetElement(2, 1), mat.GetElement(2, 2), mat.GetElement(2, 3),
//	mat.GetElement(3, 0), mat.GetElement(3, 1), mat.GetElement(3, 2), mat.GetElement(3, 3));
//
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 5.0f, -2.0f, 2.0f]\n[4.0f, 3.0f, 0.0f, -1.0f]\n[6.0f, 1.0f, 2.0f, 2.0f]\n[-4.0f, 2.0f, 3.5f, 3.5f]\n%s", ToString(mat.GetTranspose()).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.0009f, 0.0543f, -0.1966f, 0.1666f]\n[0.0037f, 0.2172f, 0.2134f, -0.3333f]\n[0.1189f, -0.1029f, 0.0280f, 0.1666f]\n[-0.0674f, 0.0898f, 0.1573f, 0.0f]\n%s", ToString(mat.GetInverse()).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.5f, 2.0f, 64.5f, 36.0f]\n[29.0f, 116.0f, -55.0f, 48.0f]\n[-105.0f, 114.0f, 15.0f, 84.0f]\n[89.0f, -178.0f, 89.0f, 0.0f]\n%s", ToString(mat.Cofactor()).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.5f, 29.0f, -105.0f, 89.0f]\n[2.0f, 116.0f, 114.0f, -178.0f]\n[63.5f, -55.0f, 15.0f, 89.0f]\n[-36.0f, 48.0f, 84.0f, 0.0f]\n%s", ToString(mat.Adjugate()).c_str());
//LOG_SYSTEM_PRINT("Expect: 534.0f - %.6f\n", mat.Determinant());
//
//
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 1.0f, 0.0f, 0.0f]\n[0.0f, 0.0f, 1.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::Identity()).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 1.0f, 0.0f, 0.0f]\n[0.0f, 0.0f, 1.0f, 0.0f]\n[-1.0f, 5.0f, 8.0f, 1.0f]\n%s", ToString(Float4x4::Translation(Vec3f(-1.0f, 5.0f, 8.0f))).c_str());
//
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 0.6427f, 0.7660f, 0.0f]\n[0.0f, -0.7660f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::RotationX(ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.0f, -0.7660f, 0.0f]\n[0.0f, 1.0f, 0.0f, 0.0f]\n[0.7660f, 0.0f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::RotationY(ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.7660f, 0.0f, 0.0f]\n[-0.7660f, 0.6427f, 0.0f, 0.0f]\n[0.0f, 0.0f, 1.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::RotationZ(ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 0.6427f, 0.7660f, 0.0f]\n[0.0f, -0.7660f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::Rotation(Vec3f(1.0f, 0.0f, 0.0f), ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.0f, -0.7660f, 0.0f]\n[0.0f, 1.0f, 0.0f, 0.0f]\n[0.7660f, 0.0f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::Rotation(Vec3f(0.0f, 1.0f, 0.0f), ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.7660f, 0.0f, 0.0f]\n[-0.7660f, 0.6427f, 0.0f, 0.0f]\n[0.0f, 0.0f, 1.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::Rotation(Vec3f(0.0f, 0.0f, 1.0f), ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 0.6427f, 0.7660f, 0.0f]\n[0.0f, -0.7660f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::RotationRollPitchYaw(0.0f, ToRadiansF(50.0f), 0.0f)).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.0f, -0.7660f, 0.0f]\n[0.0f, 1.0f, 0.0f, 0.0f]\n[0.7660f, 0.0f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::RotationRollPitchYaw(0.0f, 0.0f, ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.7660f, 0.0f, 0.0f]\n[-0.7660f, 0.6427f, 0.0f, 0.0f]\n[0.0f, 0.0f, 1.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::RotationRollPitchYaw(ToRadiansF(50.0f), 0.0f, 0.0f)).c_str());
//
//LOG_SYSTEM_PRINT("Expect:\n[0.6683f, -0.3329f, -0.6652f]\n[0.4860f, 0.8724f, 0.0516f]\n[0.5631f, -0.3578f, 0.7448f]\n%s", ToString(Float4x4::Rotation(Vec3f(0.5f, 1.5f, -1.0f), ToRadiansF(50.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[0.5f, 0.0f, 0.0f, 0.0f]\n[0.0f, 0.75f, 0.0f, 0.0f]\n[0.0f, 0.0f, 2.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::Scale(Vec3f(0.5f, 0.75f, 2.0f))).c_str());
//LOG_SYSTEM_PRINT("Expect:\n[2.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 2.0f, 0.0f, 0.0f]\n[0.0f, 0.0f, 2.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::Scale(2.0f)).c_str());
