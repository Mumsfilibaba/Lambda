#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    //----
    //Time
    //----

	class Time
	{
	public:
		/////////////////////////////////
		_forceinline Time(uint64 ns = 0)
			: m_Ns(ns)
		{
		}

		/////////////////////////////////
		_forceinline Time(Time&& other)
			: m_Ns(other.m_Ns)
		{
		}

		//////////////////////////////////////
		_forceinline Time(const Time& other)
			: m_Ns(other.m_Ns)
		{
		}

		////////////////////////////////////
		_forceinline float AsSeconds() const
		{
			constexpr float second = 1000.0f * 1000.0f * 1000.0f;
			return m_Ns / second;
		}

		///////////////////////////////////
		_forceinline float AsMilliSeconds() const
		{
			constexpr float ms = 1000.0f * 1000.0f;
			return m_Ns / ms;
		}

		/////////////////////////////////////////
		_forceinline float AsMicroSeconds() const
		{
			constexpr float us = 1000.0f;
			return m_Ns / us;
		}

		/////////////////////////////////////////
		_forceinline uint64 AsNanoSeconds() const
		{
			return m_Ns;
		}

		/////////////////////////////////////////////////
		_forceinline Time& operator=(const Time& other)
		{
			m_Ns = other.m_Ns;
			return *this;
		}

		//////////////////////////////////////////////////////
		_forceinline bool operator==(const Time& other) const
		{
			return m_Ns == other.m_Ns;
		}

		//////////////////////////////////////////////////////
		_forceinline bool operator!=(const Time& other) const
		{
			return m_Ns != other.m_Ns;
		}

		//////////////////////////////////////////////////
		_forceinline Time& operator+=(const Time& right)
		{
			m_Ns += right.m_Ns;
			return *this;
		}

		//////////////////////////////////////////////////
		_forceinline Time& operator-=(const Time& right)
		{
			m_Ns -= right.m_Ns;
			return *this;
		}

		//////////////////////////////////////////////////
		_forceinline Time& operator*=(const Time& right)
		{
			m_Ns *= right.m_Ns;
			return *this;
		}

		//////////////////////////////////////////////////
		_forceinline Time& operator/=(const Time& right)
		{
			m_Ns /= right.m_Ns;
			return *this;
		}

		friend Time operator+(const Time& left, const Time& right);
		friend Time operator-(const Time& left, const Time& right);
		friend Time operator*(const Time& left, const Time& right);
		friend Time operator/(const Time& left, const Time& right);
		friend bool operator>(const Time& left, const Time& right);
		friend bool operator<(const Time& left, const Time& right);
		friend bool operator>=(const Time& left, const Time& right);
		friend bool operator<=(const Time& left, const Time& right);
	private:
		uint64 m_Ns;
	public:
		//////////////////////////////////////////
		_forceinline static Time Seconds(float s)
		{
			constexpr float second = 1000.0f * 1000.0f * 1000.0f;
			return Time(uint64(s * second));
		}

		////////////////////////////////////////////////
		_forceinline static Time MilliSeconds(float ms)
		{
			constexpr float millisecond = 1000.0f * 1000.0f;
			return Time(uint64(ms * millisecond));
		}

		////////////////////////////////////////////////
		_forceinline static Time MicroSeconds(float us)
		{
			constexpr float microsecond = 1000.0f;
			return Time(uint64(us * microsecond));
		}

		////////////////////////////////////////////////
		_forceinline static Time NanoSeconds(uint64 ns)
		{
			return Time(ns);
		}
	};

	///////////////////////////////////////////////////////////////////
	_forceinline Time operator+(const Time& left, const Time& right)
	{
		return Time(left.m_Ns + right.m_Ns);
	}

	///////////////////////////////////////////////////////////////////
	_forceinline Time operator-(const Time& left, const Time& right)
	{
		return Time(left.m_Ns - right.m_Ns);
	}

	///////////////////////////////////////////////////////////////////
	_forceinline Time operator*(const Time& left, const Time& right)
	{
		return Time(left.m_Ns * right.m_Ns);
	}

	///////////////////////////////////////////////////////////////////
	_forceinline Time operator/(const Time& left, const Time& right)
	{
		return Time(left.m_Ns / right.m_Ns);
	}

	///////////////////////////////////////////////////////////////////
	_forceinline bool operator>(const Time& left, const Time& right)
	{
		return left.m_Ns > right.m_Ns;
	}

	///////////////////////////////////////////////////////////////////
	_forceinline bool operator<(const Time& left, const Time& right)
	{
		return left.m_Ns < right.m_Ns;
	}

	///////////////////////////////////////////////////////////////////
	_forceinline bool operator>=(const Time& left, const Time& right)
	{
		return (left.m_Ns >= right.m_Ns);
	}

	///////////////////////////////////////////////////////////////////
	_forceinline bool operator<=(const Time& left, const Time& right)
	{
		return (left.m_Ns <= right.m_Ns);
	}
}
