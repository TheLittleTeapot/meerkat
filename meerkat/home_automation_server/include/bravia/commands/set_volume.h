#ifndef MEERKAT_BRAVIA_COMMANDS_SET_VOLUME_H
#define MEERKAT_BRAVIA_COMMANDS_SET_VOLUME_H

#include <bravia/commands/base.h>
#include <bravia/commands/command_type.h>

#include <functional>
#include <sstream>
#include <iomanip>

namespace bravia::command
{
	class SetVolume : public Base
	{
	public:

		class Answer
		{
		public:
			Answer(meerkat::Buffer buffer)
			{
				assert(buffer.size() == 24);

				std::string paramBuffer{buffer.begin()+7, buffer.begin()+23};
				auto result = std::stol(paramBuffer);
				m_success = !result;
			};
		//private:
			bool m_success;
		};


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

		

	private:
		Callback m_cb;
	};
}

#endif // MEERKAT_BRAVIA_COMMANDS_SET_VOLUME_H