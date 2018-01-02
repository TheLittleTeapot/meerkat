#ifndef MEERKAT_BRAVIA_COMMANDS_POWER_H
#define MEERKAT_BRAVIA_COMMANDS_POWER_H

#include <bravia/commands/base.h>
#include <bravia/commands/command_type.h>

#include <functional>
#include <sstream>
#include <iomanip>

namespace bravia::command
{
	class SetPower : public Base
	{
	public:
		using Callback = std::function<void(bool)>;

		SetPower(bool newStatus)
		{
			setCommandType(Type::Control);
			setFunction(Functions::Power);

			std::stringstream ss;
			ss << std::setw(16) << std::right << std::setfill('0') << static_cast<int>(newStatus);

			const std::string parameterBuffer = ss.str();
			std::memcpy(&m_buffer[7], &parameterBuffer[0], 16);
		}

		operator const std::vector<char>() const
		{
			return{ m_buffer.begin(), m_buffer.end() };
		}
		
	};
}

#endif // MEERKAT_BRAVIA_COMMANDS_POWER_H