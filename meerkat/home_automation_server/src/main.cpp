
#include <system.h>
#include <messaging/message_dispatcher.h>
#include <messaging/message_listener.h>
#include <messaging/message.h>

#include <iostream>
#include <string>

class Test : public System
{
public:
	Test(messaging::MessageDispatcher& dispatcher):
		System(dispatcher)
	{
		dispatcher.addListener(this, { messaging::Type::Unknown });
	}

	void threadUpdate() override
	{
		/*printf(".");*/
	}

private:
	void handleMessage(messaging::UniqueMessage)
	{
		printf("hello\n");
	}

};

int main(void)
{
	messaging::MessageDispatcher dispatcher;
	Test asd(dispatcher);

	asd.init();

	for (;;)
	{
		printf("Enter a message:\n");

		std::string input;
		std::getline(std::cin, input);
		if (!input.empty())
		{
			if (input == "q")
				break;


			auto message = std::make_unique<messaging::Message>(messaging::Type::Unknown);

			dispatcher.dispatch(std::move(message));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
	}

	asd.term();
}