#ifndef MEERKAT_BRAVIA_COMMANDS_BASE_H
#define MEERKAT_BRAVIA_COMMANDS_BASE_H

#include "command_type.h"
#include "functions.h"

#include <meerkat/buffer.h>

#include <array>
#include <algorithm>

namespace bravia::command
{
	static bool getBufferInfo(const meerkat::Buffer& buffer, Type& type, Functions& function)
	{
		if (buffer.size() != 24)
			return false;

		char typeChr = buffer[2];
		if (typeChr == 'C') type = Type::Control;
		else if (typeChr == 'E') type = Type::Enquiry;
		else if (typeChr == 'A') type = Type::Answer;
		else if (typeChr == 'N') type = Type::Notify;
		else return false;

		std::string functionName = { buffer.begin() + 3, buffer.begin() + 7 };
		auto findRes = std::find_if(functionCodes.begin(), functionCodes.end(), [&functionName](const std::pair<Functions, std::string>& pair) {return pair.second == functionName; });

		if (findRes == functionCodes.end())
			return false;

		function = findRes->first;

		return true;
	}

	class Base
	{
	public:
		Base();
		~Base() = default;

		operator const std::vector<char>() const
		{
			return{ m_buffer.begin(), m_buffer.end() };
		}

	protected:
		std::array<char, 24> m_buffer;

		void setCommandType(Type type);
		void setFunction(Functions function);

		
	};
}

#endif