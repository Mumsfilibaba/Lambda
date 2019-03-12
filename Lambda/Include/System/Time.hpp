#pragma once

namespace Lambda
{
	class LAMBDA_API Time
	{
	public:
		Time(float ns = 0.0f);
		Time(Time&& other);
		Time(const Time& other);
		~Time();

		float AsSeconds() const;
		float AsMilliSeconds() const;
		float AsMicroSeconds() const;
		float AsNanoSeconds() const;

		Time& operator=(const Time& other);

		bool operator==(const Time& other) const;
		bool operator!=(const Time& other) const;

		Time& operator+=(const Time& right);
		Time& operator-=(const Time& right);
		Time& operator*=(const Time& right);
		Time& operator/=(const Time& right);

		friend Time operator+(const Time& left, const Time& right);
		friend Time operator-(const Time& left, const Time& right);
		friend Time operator*(const Time& left, const Time& right);
		friend Time operator/(const Time& left, const Time& right);

	private:
		float m_Ns;

	public:
		static Time Seconds(float s);
		static Time MilliSeconds(float ms);
		static Time MicroSeconds(float qs);
		static Time NanoSeconds(float ns);
	};
}