#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    //-----
    //CTime
    //-----
	class CTime
	{
	public:
		_forceinline CTime(uint64 ns = 0)
			: m_Ns(ns)
		{
		}


		_forceinline CTime(CTime&& other)
			: m_Ns(other.m_Ns)
		{
		}


		_forceinline CTime(const CTime& other)
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


		_forceinline CTime& operator=(const CTime& other)
		{
			m_Ns = other.m_Ns;
			return *this;
		}


		_forceinline bool operator==(const CTime& other) const
		{
			return m_Ns == other.m_Ns;
		}


		_forceinline bool operator!=(const CTime& other) const
		{
			return m_Ns != other.m_Ns;
		}


		_forceinline CTime& operator+=(const CTime& right)
		{
			m_Ns += right.m_Ns;
			return *this;
		}


		_forceinline CTime& operator-=(const CTime& right)
		{
			m_Ns -= right.m_Ns;
			return *this;
		}


		_forceinline CTime& operator*=(const CTime& right)
		{
			m_Ns *= right.m_Ns;
			return *this;
		}


		_forceinline CTime& operator/=(const CTime& right)
		{
			m_Ns /= right.m_Ns;
			return *this;
		}

		friend CTime operator+(const CTime& left, const CTime& right);
		friend CTime operator-(const CTime& left, const CTime& right);
		friend CTime operator*(const CTime& left, const CTime& right);
		friend CTime operator/(const CTime& left, const CTime& right);
		friend bool operator>(const CTime& left, const CTime& right);
		friend bool operator<(const CTime& left, const CTime& right);
		friend bool operator>=(const CTime& left, const CTime& right);
		friend bool operator<=(const CTime& left, const CTime& right);
	private:
		uint64 m_Ns;
	public:
		_forceinline static CTime Seconds(float s)
		{
			constexpr float second = 1000.0f * 1000.0f * 1000.0f;
			return CTime(uint64(s * second));
		}


		_forceinline static CTime MilliSeconds(float ms)
		{
			constexpr float millisecond = 1000.0f * 1000.0f;
			return CTime(uint64(ms * millisecond));
		}


		_forceinline static CTime MicroSeconds(float us)
		{
			constexpr float microsecond = 1000.0f;
			return CTime(uint64(us * microsecond));
		}


		_forceinline static CTime NanoSeconds(uint64 ns)
		{
			return CTime(ns);
		}
	};


	_forceinline CTime operator+(const CTime& left, const CTime& right)
	{
		return CTime(left.m_Ns + right.m_Ns);
	}


	_forceinline CTime operator-(const CTime& left, const CTime& right)
	{
		return CTime(left.m_Ns - right.m_Ns);
	}


	_forceinline CTime operator*(const CTime& left, const CTime& right)
	{
		return CTime(left.m_Ns * right.m_Ns);
	}


	_forceinline CTime operator/(const CTime& left, const CTime& right)
	{
		return CTime(left.m_Ns / right.m_Ns);
	}


	_forceinline bool operator>(const CTime& left, const CTime& right)
	{
		return left.m_Ns > right.m_Ns;
	}


	_forceinline bool operator<(const CTime& left, const CTime& right)
	{
		return left.m_Ns < right.m_Ns;
	}


	_forceinline bool operator>=(const CTime& left, const CTime& right)
	{
		return (left.m_Ns >= right.m_Ns);
	}


	_forceinline bool operator<=(const CTime& left, const CTime& right)
	{
		return (left.m_Ns <= right.m_Ns);
	}
}
