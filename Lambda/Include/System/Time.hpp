#pragma once
#include "Defines.h"

namespace Lambda
{
	class Time
	{
	public:
		Time(uint64 ns = 0);
		Time(Time&& other);
		Time(const Time& other);
		~Time();

		float AsSeconds() const;
		float AsMilliSeconds() const;
		float AsMicroSeconds() const;
		uint64 AsNanoSeconds() const;

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
		uint64 m_Ns;

	public:
		static Time Seconds(float s);
		static Time MilliSeconds(float ms);
		static Time MicroSeconds(float qs);
		static Time NanoSeconds(uint64 ns);
	};


	inline Time::Time(uint64 ns)
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
		constexpr float second = 1000.0f * 1000.0f * 1000.0f;
		return m_Ns / second;
	}


	inline float Time::AsMilliSeconds() const
	{
		constexpr float ms = 1000.0f * 1000.0f;
		return m_Ns / ms;
	}


	inline float Time::AsMicroSeconds() const
	{
		constexpr float us = 1000.0f;
		return m_Ns / us;
	}


	inline uint64 Time::AsNanoSeconds() const
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
		constexpr float second = 1000.0f * 1000.0f * 1000.0f;
		return Time(uint64(s * second));
	}


	inline Time Time::MilliSeconds(float ms)
	{
		constexpr float millisecond = 1000.0f * 1000.0f;
		return Time(uint64(ms * millisecond));
	}


	inline Time Time::MicroSeconds(float us)
	{
		constexpr float microsecond = 1000.0f;
		return Time(uint64(us * microsecond));
	}


	inline Time Time::NanoSeconds(uint64 ns)
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