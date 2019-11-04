#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
    //--------
    //Timestep
    //--------
    
	class Timestep
	{
	public:
		_forceinline Timestep::Timestep(uint64 ns = 0)
			: m_Ns(ns)
		{
		}


		_forceinline Timestep::Timestep(Timestep&& other)
			: m_Ns(other.m_Ns)
		{
		}


		_forceinline Timestep::Timestep(const Timestep& other)
			: m_Ns(other.m_Ns)
		{
		}


		_forceinline float Timestep::AsSeconds() const
		{
			constexpr float second = 1000.0f * 1000.0f * 1000.0f;
			return m_Ns / second;
		}


		_forceinline float Timestep::AsMilliSeconds() const
		{
			constexpr float ms = 1000.0f * 1000.0f;
			return m_Ns / ms;
		}


		_forceinline float Timestep::AsMicroSeconds() const
		{
			constexpr float us = 1000.0f;
			return m_Ns / us;
		}


		_forceinline uint64 Timestep::AsNanoSeconds() const
		{
			return m_Ns;
		}


		_forceinline Timestep& Timestep::operator=(const Timestep& other)
		{
			m_Ns = other.m_Ns;
			return *this;
		}


		_forceinline bool Timestep::operator==(const Timestep& other) const
		{
			return m_Ns == other.m_Ns;
		}


		_forceinline bool Timestep::operator!=(const Timestep& other) const
		{
			return m_Ns != other.m_Ns;
		}


		_forceinline Timestep& Timestep::operator+=(const Timestep& right)
		{
			m_Ns += right.m_Ns;
			return *this;
		}


		_forceinline Timestep& Timestep::operator-=(const Timestep& right)
		{
			m_Ns -= right.m_Ns;
			return *this;
		}


		_forceinline Timestep& Timestep::operator*=(const Timestep& right)
		{
			m_Ns *= right.m_Ns;
			return *this;
		}


		_forceinline Timestep& Timestep::operator/=(const Timestep& right)
		{
			m_Ns /= right.m_Ns;
			return *this;
		}

		friend Timestep operator+(const Timestep& left, const Timestep& right);
		friend Timestep operator-(const Timestep& left, const Timestep& right);
		friend Timestep operator*(const Timestep& left, const Timestep& right);
		friend Timestep operator/(const Timestep& left, const Timestep& right);
		friend bool operator>(const Timestep& left, const Timestep& right);
		friend bool operator<(const Timestep& left, const Timestep& right);
		friend bool operator>=(const Timestep& left, const Timestep& right);
		friend bool operator<=(const Timestep& left, const Timestep& right);
	private:
		uint64 m_Ns;
	public:
		_forceinline static Timestep Timestep::Seconds(float s)
		{
			constexpr float second = 1000.0f * 1000.0f * 1000.0f;
			return Timestep(uint64(s * second));
		}


		_forceinline static Timestep Timestep::MilliSeconds(float ms)
		{
			constexpr float millisecond = 1000.0f * 1000.0f;
			return Timestep(uint64(ms * millisecond));
		}


		_forceinline static Timestep Timestep::MicroSeconds(float us)
		{
			constexpr float microsecond = 1000.0f;
			return Timestep(uint64(us * microsecond));
		}


		_forceinline static Timestep Timestep::NanoSeconds(uint64 ns)
		{
			return Timestep(ns);
		}
	};


	_forceinline Timestep operator+(const Timestep& left, const Timestep& right)
	{
		return Timestep(left.m_Ns + right.m_Ns);
	}


	_forceinline Timestep operator-(const Timestep& left, const Timestep& right)
	{
		return Timestep(left.m_Ns - right.m_Ns);
	}


	_forceinline Timestep operator*(const Timestep& left, const Timestep& right)
	{
		return Timestep(left.m_Ns * right.m_Ns);
	}


	_forceinline Timestep operator/(const Timestep& left, const Timestep& right)
	{
		return Timestep(left.m_Ns / right.m_Ns);
	}


	_forceinline bool operator>(const Timestep& left, const Timestep& right)
	{
		return left.m_Ns > right.m_Ns;
	}


	_forceinline bool operator<(const Timestep& left, const Timestep& right)
	{
		return left.m_Ns < right.m_Ns;
	}


	_forceinline bool operator>=(const Timestep& left, const Timestep& right)
	{
		return (left.m_Ns >= right.m_Ns);
	}


	_forceinline bool operator<=(const Timestep& left, const Timestep& right)
	{
		return (left.m_Ns <= right.m_Ns);
	}
}
