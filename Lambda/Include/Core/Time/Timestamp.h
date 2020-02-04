#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	class CTimestamp
	{
	public:
		_forceinline CTimestamp(uint64 ns = 0)
			: m_Ns(ns)
		{
		}

		_forceinline CTimestamp(CTimestamp&& other)
			: m_Ns(other.m_Ns)
		{
		}

		_forceinline CTimestamp(const CTimestamp& other)
			: m_Ns(other.m_Ns)
		{
		}

		template<typename T = uint64>
		_forceinline T AsSeconds() const
		{
			constexpr T second = T(1000) * T(1000) * T(1000);
			return T(m_Ns) / second;
		}

		template<typename T = uint64>
		_forceinline T AsMilliSeconds() const
		{
			constexpr T ms = T(1000) * T(1000);
			return T(m_Ns) / ms;
		}

		template<typename T = uint64>
		_forceinline T AsMicroSeconds() const
		{
			constexpr T us = T(1000);
			return T(m_Ns) / us;
		}

		template<typename T = uint64>
		_forceinline T AsNanoSeconds() const
		{
			return T(m_Ns);
		}

		_forceinline CTimestamp& operator=(const CTimestamp& other)
		{
			m_Ns = other.m_Ns;
			return *this;
		}

		_forceinline bool operator==(const CTimestamp& other) const
		{
			return m_Ns == other.m_Ns;
		}

		_forceinline bool operator!=(const CTimestamp& other) const
		{
			return m_Ns != other.m_Ns;
		}

		_forceinline CTimestamp& operator+=(const CTimestamp& right)
		{
			m_Ns += right.m_Ns;
			return *this;
		}

		_forceinline CTimestamp& operator-=(const CTimestamp& right)
		{
			m_Ns -= right.m_Ns;
			return *this;
		}

		_forceinline CTimestamp& operator*=(const CTimestamp& right)
		{
			m_Ns *= right.m_Ns;
			return *this;
		}

		_forceinline CTimestamp& operator/=(const CTimestamp& right)
		{
			m_Ns /= right.m_Ns;
			return *this;
		}

		template<typename T = uint64>
		_forceinline static CTimestamp Seconds(T s)
		{
			constexpr T second = T(1000) * T(1000) * T(1000);
			return CTimestamp(uint64(s * second));
		}

		template<typename T = uint64>
		_forceinline static CTimestamp MilliSeconds(T ms)
		{
			constexpr T millisecond = T(1000) * T(1000);
			return CTimestamp(uint64(ms * millisecond));
		}

		template<typename T = uint64>
		_forceinline static CTimestamp MicroSeconds(T us)
		{
			constexpr T microsecond = T(1000);
			return CTimestamp(uint64(us * microsecond));
		}

		template<typename T = uint64>
		_forceinline static CTimestamp NanoSeconds(T ns)
		{
			return CTimestamp(uint64(ns));
		}

		friend CTimestamp operator+(const CTimestamp& left, const CTimestamp& right);
		friend CTimestamp operator-(const CTimestamp& left, const CTimestamp& right);
		friend CTimestamp operator*(const CTimestamp& left, const CTimestamp& right);
		friend CTimestamp operator/(const CTimestamp& left, const CTimestamp& right);
		friend bool operator>(const CTimestamp& left, const CTimestamp& right);
		friend bool operator<(const CTimestamp& left, const CTimestamp& right);
		friend bool operator>=(const CTimestamp& left, const CTimestamp& right);
		friend bool operator<=(const CTimestamp& left, const CTimestamp& right);
	private:
		uint64 m_Ns;
	};

	_forceinline CTimestamp operator+(const CTimestamp& left, const CTimestamp& right)
	{
		return CTimestamp(left.m_Ns + right.m_Ns);
	}

	_forceinline CTimestamp operator-(const CTimestamp& left, const CTimestamp& right)
	{
		return CTimestamp(left.m_Ns - right.m_Ns);
	}

	_forceinline CTimestamp operator*(const CTimestamp& left, const CTimestamp& right)
	{
		return CTimestamp(left.m_Ns * right.m_Ns);
	}

	_forceinline CTimestamp operator/(const CTimestamp& left, const CTimestamp& right)
	{
		return CTimestamp(left.m_Ns / right.m_Ns);
	}

	_forceinline bool operator>(const CTimestamp& left, const CTimestamp& right)
	{
		return left.m_Ns > right.m_Ns;
	}

	_forceinline bool operator<(const CTimestamp& left, const CTimestamp& right)
	{
		return left.m_Ns < right.m_Ns;
	}

	_forceinline bool operator>=(const CTimestamp& left, const CTimestamp& right)
	{
		return (left.m_Ns >= right.m_Ns);
	}

	_forceinline bool operator<=(const CTimestamp& left, const CTimestamp& right)
	{
		return (left.m_Ns <= right.m_Ns);
	}
}