#ifndef MEERKAT_MESSAGE_H
#define MEERKAT_MESSAGE_H

#include <memory>
#include <vector>

namespace messaging
{
	enum class Type
	{
		Unknown,
	};


	class Message
	{
	public:
		Message(Type type): m_type(type){}

	private:
		Type m_type;
	};
}

#endif // MEERKAT_MESSAGE_H
