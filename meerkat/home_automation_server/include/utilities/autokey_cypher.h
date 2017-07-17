#ifndef MEERKAT_UTILITIES_AUTOKEY_CYPHER_H
#define MEERKAT_UTILITIES_AUTOKEY_CYPHER_H

#include <vector>
#include <string>

namespace meerkat
{
	namespace utilities
	{
		static void autokeyEncrypt(std::vector<char>& input, const int& key_in)
		{
			int key = key_in;
			for (auto& chr : input)
			{
				char encryptedChar = (chr ^ key);
				key = encryptedChar;
				chr = encryptedChar;
			}
		}

		static void autokeyEncrypt(std::string& input, const int& key_in)
		{
			std::vector<char> inputVector{ input.begin(), input.end() };
			autokeyEncrypt(inputVector, key_in);
			input = { inputVector.begin(), inputVector.end() };
		}

		static void autokeyDecrypt(std::vector<char>& input, const int& key_in)
		{
			int key = key_in;
			int nextKey = 0;
			for (auto& chr : input)
			{
				nextKey = chr;
				chr = chr ^ key;
				key = nextKey;
			}
		}

		static void autokeyDecrypt(std::string& input, const int& key_in)
		{
			std::vector<char> inputVector{ input.begin(), input.end() };
			autokeyDecrypt(inputVector, key_in);
			input = { inputVector.begin(), inputVector.end() };
		}
	}
}
#endif // MEERKAT_UTILITIES_AUTOKEY_CYPHER_H