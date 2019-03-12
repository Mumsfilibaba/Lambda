#pragma once
#include "Defines.h"

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

	inline Time::Time(float ns)
		: m_Ns(ns)
	{
	}

	inline Time::Time(Time&& other)
		: m_Ns(other.m_Ns)
	{
	}

	inline Time::Time(const Time& other)
		: m_Ns(other.m_Ns)
	{
	}

	inline Time::~Time()
	{
	}

	inline float Time::AsSeconds() const
	{
		return AsMilliSeconds() / 1000.0f;
	}

	inline float Time::AsMilliSeconds() const
	{
		return AsMicroSeconds() / 1000.0f;
	}

	inline float Time::AsMicroSeconds() const
	{
		return m_Ns / 1000.0f;
	}

	inline float Time::AsNanoSeconds() const
	{
		return m_Ns;
	}

	inline Time& Time::operator=(const Time& other)
	{
		m_Ns = other.m_Ns;
		return *this;
	}

	inline bool Time::operator==(const Time& other) const
	{
		return m_Ns == other.m_Ns;
	}

	inline bool Time::operator!=(const Time& other) const
	{
		return m_Ns != other.m_Ns;
	}

	inline Time& Time::operator+=(const Time& right)
	{
		m_Ns += right.m_Ns;
		return *this;
	}

	inline Time& Time::operator-=(const Time& right)
	{
		m_Ns -= right.m_Ns;
		return *this;
	}

	inline Time& Time::operator*=(const Time& right)
	{
		m_Ns *= right.m_Ns;
		return *this;
	}

	inline Time& Time::operator/=(const Time& right)
	{
		m_Ns /= right.m_Ns;
		return *this;
	}

	inline Time Time::Seconds(float s)
	{
		return Time(s * 1000.f * 1000.0f * 1000.0f);
	}

	inline Time Time::MilliSeconds(float ms)
	{
		return Time(ms * 1000.f * 1000.0f);
	}

	inline Time Time::MicroSeconds(float qs)
	{
		return Time(qs * 1000.f);
	}

	inline Time Time::NanoSeconds(float ns)
	{
		return Time(ns);
	}

	inline Time operator+(const Time& left, const Time& right)
	{
		return Time(left.m_Ns + right.m_Ns);
	}

	inline Time operator-(const Time& left, const Time& right)
	{
		return Time(left.m_Ns - right.m_Ns);
	}

	inline Time operator*(const Time& left, const Time& right)
	{
		return Time(left.m_Ns * right.m_Ns);
	}

	inline Time operator/(const Time& left, const Time& right)
	{
		return Time(left.m_Ns / right.m_Ns);
	}
}