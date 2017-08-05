#ifndef MEERKAT_BRAVIA_COMMANDS_FUNCTIONS_H
#define MEERKAT_BRAVIA_COMMANDS_FUNCTIONS_H

#include <map>

namespace bravia::command
{
	enum class Functions
	{
		Ircc,
		Power,
		Volume,
		AudioMute,
		Channel,
		TripletChannel,
		InputSource,
		Input,
		PictureMute,
		PictureMuteToggle,
		PictureInPicture,
		PictureInPictureToggle,
		PictureInPicturePositionToggle,
		BroadcastAddress,
		MacAddress,
	};

	static std::map<Functions, const char*> functionCodes
	{
		{ Functions::Ircc, "IRCC" },
		{ Functions::Power, "POWR" },
		{ Functions::Volume, "VOLU" },
		{ Functions::AudioMute, "AMUT" },
		{ Functions::Channel, "CHNN" },
		{ Functions::TripletChannel, "TCHN" },
		{ Functions::InputSource, "ISRC" },
		{ Functions::Input, "INPT" },
		{ Functions::PictureMute, "PMUT" },
		{ Functions::PictureMuteToggle, "TPMU" },
		{ Functions::PictureInPicture, "PIPI" },
		{ Functions::PictureInPictureToggle, "TPIP" },
		{ Functions::PictureInPicturePositionToggle, "TPPP" },
		{ Functions::BroadcastAddress, "BADR" },
		{ Functions::MacAddress, "MADR" }
	};

	static const int functionLength = 4;
}
#endif // MEERKAT_BRAVIA_COMMANDS_FUNCTIONS_H