#ifndef MEERKAT_BRAVIA_COMMANDS_COMMAND_TYPES_H
#define MEERKAT_BRAVIA_COMMANDS_COMMAND_TYPES_H

namespace bravia::command
{

	enum class Type
	{
		Control = 0x43,
		Enquiry = 0x45,
		Answer = 0x41,
		Notify = 0x4E,
	};
}

#endif // MEERKAT_BRAVIA_COMMANDS_COMMAND_TYPES_H