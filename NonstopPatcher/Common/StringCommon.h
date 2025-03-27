#include <string>
#include <optional>

std::optional<std::pair<std::string, std::string>> SplitByCharacter(const std::string& originString, const char& splitTarget)
{
	std::string left, right;

	size_t pos = originString.find(splitTarget);
	if (pos != std::string::npos)
	{
		left = originString.substr(0, pos);
		right = originString.substr(pos + 1);
	}
	else
	{
		std::cout << "Invalid input string " << originString << std::endl;
		return std::nullopt;
	}

	return std::make_pair(left, right);
}
