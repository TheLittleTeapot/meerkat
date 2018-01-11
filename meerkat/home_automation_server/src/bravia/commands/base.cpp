#include <bravia/commands/base.h>

namespace bravia::command
{
	Base::Base()
	{
		// Message Header
		m_buffer[0] = 0x2A; // *
		m_buffer[1] = 0x53; // S

		// Message Footer
		m_buffer[23] = 0x0A; // LF
	}

	void Base::setCommandType(Type type)
	{
		m_buffer[2] = static_cast<char>(type);
	}

	void Base::setFunction(Functions function)
	{
		std::memcpy(&m_buffer[3], functionCodes[function], functionLength);
	}
}