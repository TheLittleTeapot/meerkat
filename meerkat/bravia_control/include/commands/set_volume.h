#ifndef MEERKAT_BRAVIA_COMMANDS_SET_VOLUME_H
#define MEERKAT_BRAVIA_COMMANDS_SET_VOLUME_H

#include "commands/base.h"
#include "commands/command_type.h"

#include <functional>
#include <sstream>
#include <iomanip>

namespace bravia::command
{
	class SetVolume : public Base
	{
	public:
		using Callback = std::function<void(bool, int)>;

		SetVolume(Callback callback, uint32_t volume)
		{
			setCommandType(Type::Control);
			setFunction(Functions::Volume);

			std::stringstream ss;
			ss << std::setw(16) << std::right << std::setfill('0') << static_cast<int>(volume);

			const std::string parameterBuffer = ss.str();
			std::memcpy(&m_buffer[7], &parameterBuffer[0], 16);
		}

		operator const std::vector<char>() const
		{
			return{ m_buffer.begin(), m_buffer.end() };
		}

	private:
		Callback m_cb;
	};
}

#endif // MEERKAT_BRAVIA_COMMANDS_SET_VOLUME_H