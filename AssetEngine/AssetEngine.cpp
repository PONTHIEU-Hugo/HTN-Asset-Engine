#include "AssetEngine.h"

int main()
{
	std::string txt = "Yo la team :3";
	std::cout << "Start : " << txt << std::endl;

	std::string encrypted = htn::Crypt::AES_Encrypt(txt.data(), txt.size(), "Hugo");
	std::cout << "Encrypted : " << encrypted << std::endl;

	std::string decrypted = htn::Crypt::AES_Decrypt(encrypted.data(), txt.size(), "Hugo");
	std::cout << "Decrypted : " << decrypted << std::endl;

	return 0;
}
