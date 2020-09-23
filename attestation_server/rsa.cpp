#include<iostream>
#include <CkRsaW.h>
#include <CkPfxW.h>
#include <CkPrivateKeyW.h>

std::string signData(std::string data) {
	// This example assumes the Chilkat API to have been previously unlocked.
    // See Global Unlock Sample for sample code.

	std::wstring widestr = std::wstring(data.begin(), data.end());
	const wchar_t * strData = widestr.c_str();

    CkRsaW rsa;

    // Load the .pfx/.p12
    CkPfxW pfx;
    bool success = pfx.LoadPfxFile(L"attst-server.p12",L"qwerty");
    if (success != true) {
        wprintf(L"%s\n",pfx.lastErrorText());
        //return 1;
    }

    // Get the default private key.
    CkPrivateKeyW *privKey = pfx.GetPrivateKey(0);
    if (pfx.get_LastMethodSuccess() != true) {
        wprintf(L"%s\n",pfx.lastErrorText());
        //return 1;
    }

    // Import the private key into the RSA component:
    success = rsa.ImportPrivateKeyObj(*privKey);
    if (success != true) {
        wprintf(L"%s\n",rsa.lastErrorText());
        delete privKey;
        //return 1;
    }

    delete privKey;

    // This example will sign a string, and receive the signature
    // in a base64-encoded string.  Therefore, set the encoding mode
    // to "base64":
    rsa.put_EncodingMode(L"base64");

    // If some other non-Chilkat application or web service is going to be verifying
    // the signature, it is important to match the byte-ordering.
    // The LittleEndian property may be set to true
    // for little-endian byte ordering, 
    // or false  for big-endian byte ordering.
    // Microsoft apps typically use little-endian, while
    // OpenSSL and other services (such as Amazon CloudFront)
    // use big-endian.
    rsa.put_LittleEndian(false);

    //const wchar_t *strData = L"This is the string to be signed.";

    // Sign the string using the sha-256 hash algorithm.
    // Other valid choices are "sha-1", "md2" and "md5".
    const wchar_t *base64Sig = rsa.signStringENC(strData,L"sha-256");

    //std::wcout << base64Sig << std::endl;

    //std::wcout << L"Success\n" << std::endl;

    // Your wchar_t*
	std::wstring ws(base64Sig);
	// your new String
	std::string base64SigStr(ws.begin(), ws.end());

    return base64SigStr;
}

int main(void) {
    std::string base64Sig = signData("This is the string to be signed");

	std::cout << base64Sig << std::endl;
	std::cout << "Success\n" << std::endl;  

    return 0;
}