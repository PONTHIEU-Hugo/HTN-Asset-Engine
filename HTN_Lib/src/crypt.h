#ifndef CRYPT_H
#define CRYPT_H

#include <iostream>
#include <string>
#include "Mat4x4i.hpp"

namespace htn
{
	//A simplified version of the AES encryption method (without the generated keys)
	//Sources : https://fr.wikipedia.org/wiki/Advanced_Encryption_Standard, https://members.loria.fr/PZimmermann/cours/id12-2005-4.pdf
	struct Crypt
	{
	public:
		static std::string AES_Encrypt(char* data, size_t size, std::string key);
		static std::string AES_Decrypt(char* data, size_t size, std::string key);

	private:
		static MatGF4x4 mAuxiliar;
		static MatGF4x4 mInverseAuxiliar;

		static void InitAuxiliar();
	};
}

#endif