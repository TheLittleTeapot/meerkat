#ifndef MEERKAT_BRAVIA_COMMANDS_BASE_H
#define MEERKAT_BRAVIA_COMMANDS_BASE_H

#include "command_type.h"
#include "functions.h"

#include <buffer.h>

#include <array>
#include <algorithm>

namespace bravia::command
{
	class Base
	{
	public:
		Base();
		~Base() = default;

	protected:
		std::array<char, 24> m_buffer;

		void setCommandType(Type type);
		void setFunction(Functions function);
	};
}

#endif