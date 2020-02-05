#pragma once
#include "LambdaCore.h"

#include <string>
#include <sstream>
#include <unordered_map>

namespace Lambda
{
	class LAMBDA_API CEngineConfig final
	{
	private:
		CEngineConfig();
		~CEngineConfig() = default;

	public:
		bool LoadConfigFile(const char* pFilename);
		bool WriteConfigFile();

		template<typename T>
		bool GetVar(const char* pVarName, T& value);
		
		template<typename T>
		void SetVar(const char* pVarName, T value);
		
		static CEngineConfig& Get();

	private:
		template <typename T>
		T ConvertValue(const std::string& value);

		template <typename T>
		std::string ConvertType(T type);

	private:
		std::string m_Filename;
		std::unordered_map<std::string, std::string> m_VarTable;
		bool m_HasChanged;
	};

	template<typename T>
	inline void CEngineConfig::SetVar(const char* pVarName, T value)
	{
		std::string varName = std::string(pVarName);
		m_VarTable[varName] = ConvertType<T>(value);

		m_HasChanged = true;
	}

	template<typename T>
	inline bool CEngineConfig::GetVar(const char* pVarName, T& value)
	{
		auto var = m_VarTable.find(std::string(pVarName));
		if (var != m_VarTable.end())
		{
			value = ConvertValue<T>(var->second);
			return true;
		}

		return false;
	}

	template <typename T>
	inline T CEngineConfig::ConvertValue(const std::string& value)
	{
		T result;

		std::stringstream input(value);
		input >> result;

		return result;
	}

	template <>
	inline std::string CEngineConfig::ConvertValue(const std::string& value)
	{
		return value;
	}

	template <typename T>
	inline std::string CEngineConfig::ConvertType(T type)
	{
		return std::to_string<T>(type);
	}

	template <>
	inline std::string CEngineConfig::ConvertType<const char*>(const char* pType)
	{
		return std::string(pType);
	}

	template <>
	inline std::string CEngineConfig::ConvertType<std::string>(std::string type)
	{
		return type;
	}
}