#include "LambdaPch.h"

#include "Core/Log/EngineLog.h"
#include "Core/Configuration/EngineConfig.h"

#include <fstream>

namespace Lambda
{
	CEngineConfig::CEngineConfig()
		: m_VarTable()
	{
	}

	bool CEngineConfig::LoadConfigFile(const char* pFilename)
	{
		m_Filename = std::string(pFilename);
		
		std::ifstream file(m_Filename);
		if (file.is_open())
		{
			std::string line;
			while (std::getline(file, line))
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
			D_LOG_CORE_ERROR("Failed to open configuration file '%s'", pFilename);
			return false;
		}
	}

	bool CEngineConfig::WriteConfigFile()
	{
		return false;
	}

	CEngineConfig& CEngineConfig::Get()
	{
		static CEngineConfig instance;
		return instance;
	}
}