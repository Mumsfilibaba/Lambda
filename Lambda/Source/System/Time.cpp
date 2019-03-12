#include <LambdaPch.h>
#include <System/Time.hpp>

namespace Lambda
{
	Time::Time(float ns)
		: m_Ns(ns)
	{
	}

	Time::Time(Time&& other)
		: m_Ns(other.m_Ns)
	{
	}
	
	Time::Time(const Time& other)
		: m_Ns(other.m_Ns)
	{
	}

	Time::~Time()
	{
	}
	
	float Time::AsSeconds() const
	{
		return AsMilliSeconds() / 1000.0f;
	}
	
	float Time::AsMilliSeconds() const
	{
		return AsMicroSeconds() / 1000.0f;
	}
	
	float Time::AsMicroSeconds() const
	{
		return m_Ns / 1000.0f;
	}
	
	float Time::AsNanoSeconds() const
	{
		return m_Ns;
	}

	Time& Time::operator=(const Time& other)
	{
		m_Ns = other.m_Ns;
		return *this;
	}

	bool Time::operator==(const Time& other) const
	{
		return m_Ns == other.m_Ns;
	}

	bool Time::operator!=(const Time& other) const
	{
		return m_Ns != other.m_Ns;
	}

	Time& Time::operator+=(const Time& right)
	{
		m_Ns += right.m_Ns;
		return *this;
	}
	
	Time& Time::operator-=(const Time& right)
	{
		m_Ns -= right.m_Ns;
		return *this;
	}

	Time& Time::operator*=(const Time& right)
	{
		m_Ns *= right.m_Ns;
		return *this;
	}
	
	Time& Time::operator/=(const Time& right)
	{
		m_Ns /= right.m_Ns;
		return *this;
	}
	
	Time Time::Seconds(float s)
	{
		return Time(s * 1000.f * 1000.0f * 1000.0f);
	}

	Time Time::MilliSeconds(float ms)
	{
		return Time(ms * 1000.f * 1000.0f);
	}

	Time Time::MicroSeconds(float qs)
	{
		return Time(qs * 1000.f);
	}
	
	Time Time::NanoSeconds(float ns)
	{
		return Time(ns);
	}

	Time operator+(const Time& left, const Time& right)
	{
		return Time(left.m_Ns + right.m_Ns);
	}
	 
	Time operator-(const Time& left, const Time& right)
	{
		return Time(left.m_Ns - right.m_Ns);
	}
	
	Time operator*(const Time& left, const Time& right)
	{
		return Time(left.m_Ns * right.m_Ns);
	}

	Time operator/(const Time& left, const Time& right)
	{
		return Time(left.m_Ns / right.m_Ns);
	}
}