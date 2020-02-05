#include "LambdaPch.h"

#include "Core/Log/EngineLog.h"
#include "Core/Configuration/EngineConfig.h"

#include <fstream>

namespace Lambda
{
	CEngineConfig::CEngineConfig()
		: m_Filename(),
		m_VarTable(),
		m_HasChanged(false)
	{
	}

	bool CEngineConfig::LoadConfigFile(const char* pFilename)
	{
		m_Filename = std::string(pFilename);
		
		std::ifstream configFile(m_Filename);
		if (configFile.is_open())
		{
			std::string line;
			while (std::getline(configFile, line))
			{
				if (line.size() > 0 && line[0] != '#')
				{
					size_t index = 0;
					while (std::isspace(line[index]))
					{
						index++;
					}

					const size_t beginKeyString = index;
					while (!std::isspace(line[index]) && line[index] != '=')
					{
						index++;
					}

					const std::string key = line.substr(beginKeyString, index - beginKeyString);

					while (std::isspace(line[index]) || line[index] == '=')
					{
						index++;
					}

					const std::string value = line.substr(index, line.size() - index);

					//Set value/key pair
					m_VarTable.insert(std::make_pair(key, value));
				}
			}

			return true;
		}
		else
		{
			D_LOG_CORE_ERROR("Failed to open configuration file '%s'", m_Filename.c_str());
			return false;
		}
	}

	bool CEngineConfig::WriteConfigFile()
	{
		if (m_HasChanged)
		{
			std::ofstream configFile(m_Filename);
			if (configFile.is_open())
			{
				for (auto& keyValuePair : m_VarTable)
				{
					configFile << keyValuePair.first;
					configFile << '=';
					configFile << keyValuePair.second;
					configFile << '\n';
				}
			}
			else
			{
				D_LOG_CORE_ERROR("Failed to open configuration file '%s'", m_Filename.c_str());
				return false;
			}
		}

		return false;
	}

	CEngineConfig& CEngineConfig::Get()
	{
		static CEngineConfig instance;
		return instance;
	}
}