#pragma once
#include <vector>
#include <any>

using Parameters = std::vector<std::reference_wrapper<std::any>>;

class IPlugIn
{
public:
	virtual ~IPlugIn() {}
	virtual void ExecuteSomething(Parameters& params) = 0;
};

namespace Plugin
{
	template<typename T>
	bool AssignParameter(const T& arg, std::any& param)
	{
		try
		{
			arg = std::any_cast<const T&>(param);
		}
		catch (const std::bad_any_cast&)
		{
			return false;
		}

		return true;
	}

	template<typename T>
	bool AssignParameter(T& arg, std::any& param)
	{
		try
		{
			arg = std::any_cast<T&>(param);
		}
		catch (const std::bad_any_cast&)
		{
			return false;
		}

		return true;
	}

	template<typename... Args>
	bool GetParameters(Args&... args, Parameters& params)
	{
		if (sizeof...(args) != params.size())
		{
			return false;
		}

		size_t index = 0;
		bool success = true;
		([&]
			{
				if (not AssignParameter(args, params[index].get()))
				{
					success = false;
				}
				++index;
			}(), ...);

		return success;
	}
}