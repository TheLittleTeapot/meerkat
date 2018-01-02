#ifndef MEERKAT_BRVIA_CONTROLLER_H
#define MEERKAT_BRVIA_CONTROLLER_H

#include "meerkat/mg_manager.h"

#include "commands/set_ircc_code.h"
#include "commands/set_volume.h"
#include "commands/get_volume.h"
#include "commands/power.h"

namespace bravia
{
	class Controller
	{
	public:
		Controller(const std::string& host):
			m_host(host)
		{
		}

		void update()
		{
			m_manager.poll(std::chrono::milliseconds(100));
		}

		bool setIrccCode(command::IrCode irc_code, command::SetIrccCode::Callback cb)
		{
			if ((int)irc_code < 0 || (int)irc_code  > 97)
				return false;

			bravia::command::SetIrccCode ircCommand(cb, irc_code);

			const auto weakConnection = m_manager.connect(m_host);
			if (auto connection = weakConnection.lock())
			{
				connection->m_onConnect = [](int result)
				{
				};

				connection->m_onReceive = [cb](meerkat::Connection& conn, const meerkat::Buffer& buff)
				{
					std::string str{ buff.begin(), buff.end() };
					printf("Client received: %s\n", str.c_str());

					// TODO Parse into response

					cb(true);
				};

				connection->send(ircCommand);
			}
		}

		// setPowerStatus

		bool setPowerStatus(bool newStatus, command::SetPower::Callback cb)
		{
			command::SetPower setPower{ newStatus };

			const auto weakConnection = m_manager.connect(m_host);
			if (auto connection = weakConnection.lock())
			{
				connection->m_onConnect = [cb, weakConnection](int result)
				{
					auto connection = weakConnection.lock();
					if (result != 0 && connection)
					{
						connection->close();
						cb(false);
					}
				};

				connection->send(setPower);
				return true;
			}
			return false;
		}

		// getPowerStatus
		// setAudioVolume

		bool setAudioVolume(uint32_t volume, command::SetVolume::Callback cb)
		{
			if (volume > 100)
				return false;

			command::SetVolume setVolumeCommand(cb, volume);

			const auto weakConnection = m_manager.connect(m_host);
			if (auto connection = weakConnection.lock())
			{
				connection->m_onConnect = [cb, weakConnection](int result)
				{
					auto connection = weakConnection.lock();
					if (result != 0 && connection)
					{
						connection->close();
						cb(false, 0);
					}
				};

				connection->m_onReceive = [cb, volume](meerkat::Connection& conn, const meerkat::Buffer& buff)
				{
					std::string str{ buff.begin(), buff.end() };

					bravia::command::Type type;
					bravia::command::Functions function;
					if (bravia::command::getBufferInfo(buff, type, function))
					{
						if (type == command::Type::Answer)
						{
							command::SetVolume::Answer answer{ buff };
							cb(answer.m_success, volume);
						}

						
					}
					conn.close();
				};

				connection->send(setVolumeCommand);
			}
		}

		bool getAudioVolume(command::GetVolume::Callback cb)
		{
			command::GetVolume getVolumeCommand(cb);

			const auto weakConnection = m_manager.connect(m_host);
			if (auto connection = weakConnection.lock())
			{
				connection->m_onConnect = [cb, weakConnection](int result)
				{
					auto connection = weakConnection.lock();
					if (result != 0 && connection)
					{
						connection->close();
						cb(false, 0);
					}
				};

				connection->m_onReceive = [cb](meerkat::Connection& conn, const meerkat::Buffer& buff)
				{
					std::string str{ buff.begin(), buff.end() };

					bravia::command::Type type;
					bravia::command::Functions function;
					if (bravia::command::getBufferInfo(buff, type, function))
					{
						if (type == command::Type::Answer)
						{
							command::GetVolume::Answer answer{ buff };
							cb(answer.m_success, answer.m_volume);
						}


					}
					conn.close();
				};

				connection->send(getVolumeCommand);
			}
			return true;
		}

		// setAudioMute
		// getAudioMute
		// setChannel
		// getChannel
		// setTripletChannel
		// getTripletChannel
		// setInputSource
		// getInputSource
		// setInput
		// getInput
		// setPictureMute
		// getPictureMute
		// togglePictureMute
		// setPip
		// getPip
		// togglePip
		// togglePipPosition
		// getBroadcastAddress

	private:
		meerkat::Manager m_manager;

		std::string m_host;
	};
}

#endif // MEERKAT_BRVIA_CONTROLLER_H
