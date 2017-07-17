
#include <system.h>
#include <messaging/message_dispatcher.h>
#include <messaging/message_listener.h>
#include <messaging/message.h>

#include <iostream>
#include <string>

class Test : public System
{
public:
	Test() = default;

	void threadUpdate() override
	{
	}

private:
	void handleMessage(messaging::SharedMessage)
	{
		printf("ping\n");

		m_listener->sendMessage(std::make_shared<messaging::Message>(messaging::Type::Pong));
	}

};

int main(void)
{
	auto dispatcher = std::make_shared<messaging::MessageDispatcher>();
	Test asd;

	asd.init(std::move(dispatcher->createListener({ messaging::Type::Ping })));

	auto uniqueListener = dispatcher->createListener({ messaging::Type::Pong });

	for (;;)
	{
		while (auto message = uniqueListener->popMessage())
		{
			printf("pong\n");
		}

		printf("Enter a message:\n");

		std::string input;
		std::getline(std::cin, input);
		if (!input.empty())
		{
			if (input == "q")
				break;


			auto message = std::make_shared<messaging::Message>(messaging::Type::Ping);

			dispatcher->dispatch(std::move(message));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
	}

	asd.term();
}