
#include <system.h>
#include <messaging/message_listener.h>
#include <messaging/message.h>

#include <iostream>
#include <string>

class Test : public System
{
public:
	void threadUpdate() override
	{
		printf(".");
	}

private:
	void handleMessage(messaging::UniqueMessage)
	{
		printf("hello\n");
	}

};

int main(void)
{
	Test asd;

	asd.init();

	for (;;)
	{
		printf("Enter a message:\n");

		std::string input;
		std::getline(std::cin, input);
		if (!input.empty())
		{
			auto message = std::make_unique<messaging::Message>(messaging::Type::Unknown);
			asd.addMessage(std::move(message));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
	}

	asd.term();
}