#ifndef MEERKAT_BRAVIA_COMMANDS_SET_IRCC_CODE_H
#define MEERKAT_BRAVIA_COMMANDS_SET_IRCC_CODE_H

#include "commands/base.h"
#include "commands/command_type.h"
#include <functional>

#include <sstream>
#include <iomanip>

namespace bravia::command
{
	class SetIrccCode : public Base
	{
	public:
		using Callback = std::function<void(bool)>;

		SetIrccCode(Callback callback, const int& ir_code)
		{
			setCommandType(Type::Control);
			setFunction(Functions::Ircc);

			std::stringstream ss;
			ss << std::setw(16) << std::right << std::setfill('0') << ir_code;

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

#endif // MEERKAT_BRAVIA_COMMANDS_SET_IRCC_CODE_H