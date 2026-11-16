#ifndef CRYPT_H
#define CRYPT_H

#include <iostream>
#include <string>
#include "Mat4x4i.hpp"

namespace htn
{
	struct Crypt
	{
	public:
		static void Encrypt(char* data, size_t size, std::string key);
		static void Decrypt(char* encrypted, size_t size, std::string key);

		static void AES_Encrypt(char* data, size_t size, std::string key);
		static void AES_Decrypt(char* data, size_t size, std::string key);

	private:
		static Mat4x4i mAuxiliar;

		static void InitAuxiliar();
	};
}

#endif