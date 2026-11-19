#include "AssetEngine.h"
#include "AssetManager.h"
using namespace htn;

int main()
{
	std::string name = "Hugo le Ponthieu :3";

	std::string encrypted = htn::Crypt::AES_Encrypt(name.data(), name.size(), "Hugo");
	std::string decrypted = htn::Crypt::AES_Decrypt(encrypted.data(), encrypted.size(), "Hugo");

	return 0;
}