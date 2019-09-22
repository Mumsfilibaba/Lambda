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
		Timestep(uint64 ns = 0);
		Timestep(Timestep&& other);
		Timestep(const Timestep& other);
		~Timestep() = default;

		float AsSeconds() const;
		float AsMilliSeconds() const;
		float AsMicroSeconds() const;
		uint64 AsNanoSeconds() const;

		Timestep& operator=(const Timestep& other);

		bool operator==(const Timestep& other) const;
		bool operator!=(const Timestep& other) const;

		Timestep& operator+=(const Timestep& right);
		Timestep& operator-=(const Timestep& right);
		Timestep& operator*=(const Timestep& right);
		Timestep& operator/=(const Timestep& right);

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
		static Timestep Seconds(float s);
		static Timestep MilliSeconds(float ms);
		static Timestep MicroSeconds(float us);
		static Timestep NanoSeconds(uint64 ns);
	};


	inline Timestep::Timestep(uint64 ns)
		: m_Ns(ns)
	{
	}


	inline Timestep::Timestep(Timestep&& other)
		: m_Ns(other.m_Ns)
	{
	}


	inline Timestep::Timestep(const Timestep& other)
		: m_Ns(other.m_Ns)
	{
	}
    

	inline float Timestep::AsSeconds() const
	{
		constexpr float second = 1000.0f * 1000.0f * 1000.0f;
		return m_Ns / second;
	}


	inline float Timestep::AsMilliSeconds() const
	{
		constexpr float ms = 1000.0f * 1000.0f;
		return m_Ns / ms;
	}


	inline float Timestep::AsMicroSeconds() const
	{
		constexpr float us = 1000.0f;
		return m_Ns / us;
	}


	inline uint64 Timestep::AsNanoSeconds() const
	{
		return m_Ns;
	}


	inline Timestep& Timestep::operator=(const Timestep& other)
	{
		m_Ns = other.m_Ns;
		return *this;
	}


	inline bool Timestep::operator==(const Timestep& other) const
	{
		return m_Ns == other.m_Ns;
	}


	inline bool Timestep::operator!=(const Timestep& other) const
	{
		return m_Ns != other.m_Ns;
	}


	inline Timestep& Timestep::operator+=(const Timestep& right)
	{
		m_Ns += right.m_Ns;
		return *this;
	}


	inline Timestep& Timestep::operator-=(const Timestep& right)
	{
		m_Ns -= right.m_Ns;
		return *this;
	}


	inline Timestep& Timestep::operator*=(const Timestep& right)
	{
		m_Ns *= right.m_Ns;
		return *this;
	}


	inline Timestep& Timestep::operator/=(const Timestep& right)
	{
		m_Ns /= right.m_Ns;
		return *this;
	}


	inline Timestep Timestep::Seconds(float s)
	{
		constexpr float second = 1000.0f * 1000.0f * 1000.0f;
		return Timestep(uint64(s * second));
	}


	inline Timestep Timestep::MilliSeconds(float ms)
	{
		constexpr float millisecond = 1000.0f * 1000.0f;
		return Timestep(uint64(ms * millisecond));
	}


	inline Timestep Timestep::MicroSeconds(float us)
	{
		constexpr float microsecond = 1000.0f;
		return Timestep(uint64(us * microsecond));
	}


	inline Timestep Timestep::NanoSeconds(uint64 ns)
	{
		return Timestep(ns);
	}


	inline Timestep operator+(const Timestep& left, const Timestep& right)
	{
		return Timestep(left.m_Ns + right.m_Ns);
	}


	inline Timestep operator-(const Timestep& left, const Timestep& right)
	{
		return Timestep(left.m_Ns - right.m_Ns);
	}


	inline Timestep operator*(const Timestep& left, const Timestep& right)
	{
		return Timestep(left.m_Ns * right.m_Ns);
	}


	inline Timestep operator/(const Timestep& left, const Timestep& right)
	{
		return Timestep(left.m_Ns / right.m_Ns);
	}


	inline bool operator>(const Timestep& left, const Timestep& right)
	{
		return left.m_Ns > right.m_Ns;
	}


	inline bool operator<(const Timestep& left, const Timestep& right)
	{
		return left.m_Ns < right.m_Ns;
	}


	inline bool operator>=(const Timestep& left, const Timestep& right)
	{
		return (left.m_Ns >= right.m_Ns);
	}


	inline bool operator<=(const Timestep& left, const Timestep& right)
	{
		return (left.m_Ns <= right.m_Ns);
	}
}
