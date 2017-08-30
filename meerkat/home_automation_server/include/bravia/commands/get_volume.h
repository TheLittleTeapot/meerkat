#ifndef MEERKAT_BRAVIA_COMMANDS_GET_VOLUME_H
#define MEERKAT_BRAVIA_COMMANDS_GET_VOLUME_H

#include <bravia/commands/base.h>
#include <bravia/commands/command_type.h>

#include <functional>
#include <sstream>
#include <iomanip>

namespace bravia::command
{
	class GetVolume : public Base
	{
	public:

		class Answer
		{
		public:
			Answer(meerkat::Buffer buffer)
			{
				// assert(buffer.size() == 24);

				std::string paramBuffer{ buffer.begin() + 7, buffer.begin() + 23 };
				m_volume = std::stol(paramBuffer);
				m_success = m_volume;
			};
			
			bool m_success;
			int m_volume;
		};

		using Callback = std::function<void(bool, int)>;

		GetVolume(Callback cb)
		{
			setCommandType(Type::Enquiry);
			setFunction(Functions::Volume);

			std::stringstream ss;
			ss << std::setw(16) << std::right << std::setfill('#') << ""; // << static_cast<int>(volume);

			std::memset(&m_buffer[7], '#', 16);

		//	const std::string parameterBuffer = ss.str();
			//std::memcpy(&m_buffer[7], &parameterBuffer[0], 16);
		}

	private:

	};
}

#endif // MEERKAT_BRAVIA_COMMANDS_GET_VOLUME_H