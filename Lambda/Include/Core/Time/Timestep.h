#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	class CTimestep
	{
	public:
		_forceinline CTimestep(uint64 ns = 0)
			: m_Ns(ns)
		{
		}

		_forceinline CTimestep(CTimestep&& other)
			: m_Ns(other.m_Ns)
		{
		}

		_forceinline CTimestep(const CTimestep& other)
			: m_Ns(other.m_Ns)
		{
		}

		_forceinline float AsSeconds() const
		{
			constexpr float second = 1000.0f * 1000.0f * 1000.0f;
			return m_Ns / second;
		}

		_forceinline float AsMilliSeconds() const
		{
			constexpr float ms = 1000.0f * 1000.0f;
			return m_Ns / ms;
		}

		_forceinline float AsMicroSeconds() const
		{
			constexpr float us = 1000.0f;
			return m_Ns / us;
		}

		_forceinline uint64 AsNanoSeconds() const
		{
			return m_Ns;
		}

		_forceinline CTimestep& operator=(const CTimestep& other)
		{
			m_Ns = other.m_Ns;
			return *this;
		}

		_forceinline bool operator==(const CTimestep& other) const
		{
			return m_Ns == other.m_Ns;
		}

		_forceinline bool operator!=(const CTimestep& other) const
		{
			return m_Ns != other.m_Ns;
		}

		_forceinline CTimestep& operator+=(const CTimestep& right)
		{
			m_Ns += right.m_Ns;
			return *this;
		}

		_forceinline CTimestep& operator-=(const CTimestep& right)
		{
			m_Ns -= right.m_Ns;
			return *this;
		}

		_forceinline CTimestep& operator*=(const CTimestep& right)
		{
			m_Ns *= right.m_Ns;
			return *this;
		}

		_forceinline CTimestep& operator/=(const CTimestep& right)
		{
			m_Ns /= right.m_Ns;
			return *this;
		}

		_forceinline static CTimestep Seconds(float s)
		{
			constexpr float second = 1000.0f * 1000.0f * 1000.0f;
			return CTimestep(uint64(s * second));
		}

		_forceinline static CTimestep MilliSeconds(float ms)
		{
			constexpr float millisecond = 1000.0f * 1000.0f;
			return CTimestep(uint64(ms * millisecond));
		}

		_forceinline static CTimestep MicroSeconds(float us)
		{
			constexpr float microsecond = 1000.0f;
			return CTimestep(uint64(us * microsecond));
		}

		_forceinline static CTimestep NanoSeconds(uint64 ns)
		{
			return CTimestep(ns);
		}

		friend CTimestep operator+(const CTimestep& left, const CTimestep& right);
		friend CTimestep operator-(const CTimestep& left, const CTimestep& right);
		friend CTimestep operator*(const CTimestep& left, const CTimestep& right);
		friend CTimestep operator/(const CTimestep& left, const CTimestep& right);
		friend bool operator>(const CTimestep& left, const CTimestep& right);
		friend bool operator<(const CTimestep& left, const CTimestep& right);
		friend bool operator>=(const CTimestep& left, const CTimestep& right);
		friend bool operator<=(const CTimestep& left, const CTimestep& right);
	private:
		uint64 m_Ns;
	};

	_forceinline CTimestep operator+(const CTimestep& left, const CTimestep& right)
	{
		return CTimestep(left.m_Ns + right.m_Ns);
	}

	_forceinline CTimestep operator-(const CTimestep& left, const CTimestep& right)
	{
		return CTimestep(left.m_Ns - right.m_Ns);
	}

	_forceinline CTimestep operator*(const CTimestep& left, const CTimestep& right)
	{
		return CTimestep(left.m_Ns * right.m_Ns);
	}

	_forceinline CTimestep operator/(const CTimestep& left, const CTimestep& right)
	{
		return CTimestep(left.m_Ns / right.m_Ns);
	}

	_forceinline bool operator>(const CTimestep& left, const CTimestep& right)
	{
		return left.m_Ns > right.m_Ns;
	}

	_forceinline bool operator<(const CTimestep& left, const CTimestep& right)
	{
		return left.m_Ns < right.m_Ns;
	}

	_forceinline bool operator>=(const CTimestep& left, const CTimestep& right)
	{
		return (left.m_Ns >= right.m_Ns);
	}

	_forceinline bool operator<=(const CTimestep& left, const CTimestep& right)
	{
		return (left.m_Ns <= right.m_Ns);
	}
}