#ifndef MEERKAT_BRAVIA_COMMANDS_SET_IRCC_CODE_H
#define MEERKAT_BRAVIA_COMMANDS_SET_IRCC_CODE_H

#include "commands/base.h"
#include "commands/command_type.h"
#include <functional>

#include <sstream>
#include <iomanip>

namespace bravia::command
{
	enum class IrCode
	{
		Power_Off,			// 00
		Input,				// 01
		Guide,				// 02
		EPG,				// 03
		Favorites,			// 04
		Display,			// 05
		Home,				// 06
		Options,			// 07
		Return,				// 08
		Up,					// 09
		Down,				// 10
		Right,				// 11
		Left,				// 12
		Confirm,			// 13
		Red,				// 14
		Green,				// 15
		Yellow,				// 16
		Blue,				// 17
		Num1,				// 18
		Num2,				// 19
		Num3,				// 20
		Num4,				// 21
		Num5,				// 22
		Num6,				// 23
		Num7,				// 24
		Num8,				// 25
		Num9,				// 26
		Num0,				// 27
		Num11,				// 28
		Num12,				// 29
		Volume_Up,			// 30
		Volume_Down,		// 31
		Mute,				// 32
		Channel_Up,			// 33
		Channel_Down,		// 34
		Subtitle,			// 35
		Closed_Caption,		// 36
		Enter,				// 37
		DOT,				// 38
		Analog,				// 39
		Teletext,			// 40
		Exit,				// 41
		Analog2,			// 42
		AD,					// 43
		Digital,			// 44
		Analog_,			// 45
		BS,					// 46
		CS,					// 47
		BS_CS,				// 48
		Ddata,				// 49
		Pic_Off,			// 50
		Tv_Radio,			// 51
		Theater,			// 52
		SEN,				// 53
		Internet_Widgets,	// 54
		Internet_Video,		// 55
		Netflix,			// 56
		Scene_Select,		// 57
		Mode3D,				// 58
		iManual,			// 59
		Audio,				// 60
		Wide,				// 61
		Jump,				// 62
		PAP,				// 63
		MyEPG,				// 64
		Program_Description, // 65
		Write_Chapter,		// 66
		TrackID,			// 67
		Ten_Key,			// 68
		AppliCast,			// 69
		acTVila,			// 70
		Delete_Video,		// 71
		Photo_Frame,		// 72
		TV_Pause,			// 73
		Key_Pad,			// 74
		Media,				// 75
		Sync_Menu,			// 76
		Forward,			// 77
		Play,				// 78
		Rewind,				// 79
		Prev,				// 80
		Stop,				// 81
		Next,				// 82
		Rec,				// 83
		Pause,				// 84
		Eject,				// 85
		Flash_Plus,			// 86
		Flash_Minus,		// 87
		Top_Menu,			// 88
		Popup_Menu,			// 89
		Rakuraku_Start,		// 90
		One_Touch_Time_Rec, // 91
		One_Touch_View,		// 92
		One_Touch_Rec,		// 93
		One_Touch_Stop,		// 94
		DUX,				// 95
		Football_Mode,		// 96
		Social,				// 97
	};


	class SetIrccCode : public Base
	{
	public:
		using Callback = std::function<void(bool)>;

		SetIrccCode(Callback callback, IrCode ir_code)
		{
			setCommandType(Type::Control);
			setFunction(Functions::Ircc);

			std::stringstream ss;
			ss << std::setw(16) << std::right << std::setfill('0') << static_cast<int>(ir_code);

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