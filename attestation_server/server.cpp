#include <httplib.h>
#include <CkCrypt2.h>
#include <CkByteData.h>
#include <CkStringBuilder.h>
#include <CkRsaW.h>
#include <CkPfxW.h>
#include <CkPrivateKeyW.h>
#include <iostream>

std::string signData(std::string data) {

  std::wstring widestr = std::wstring(data.begin(), data.end());
  const wchar_t * strData = widestr.c_str();

  CkRsaW rsa;

  // Load the ..p12
  CkPfxW pfx;
  bool success = pfx.LoadPfxFile(L"attst-server.p12",L"qwerty");
  if (success != true) {
    throw 5477;
  }

  // Get the default private key.
  CkPrivateKeyW *privKey = pfx.GetPrivateKey(0);
  if (pfx.get_LastMethodSuccess() != true) {
    throw 6455;
  }

  // Import the private key into the RSA component:
  success = rsa.ImportPrivateKeyObj(*privKey);
  if (success != true) {
    delete privKey;
    throw 6456;
  }

  delete privKey;

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

  // Sign the string using the sha-256 hash algorithm.
  // Other valid choices are "sha-1", "md2" and "md5".
  const wchar_t *base64Sig = rsa.signStringENC(strData,L"sha-256");

  // COnvert to string
  std::wstring ws(base64Sig);
  std::string base64SigStr(ws.begin(), ws.end());

  return base64SigStr;
}

std::string hashFile (std::string path) {
	const char * filePath = path.c_str();

	CkCrypt2 crypt;

    // Choose the hash algorithm.
    // Can be  "sha1", "sha256", "sha384", "sha512", "md2", "md5", "haval", "ripemd128", "ripemd160","ripemd256", or "ripemd320".
    crypt.put_HashAlgorithm("sha256");

    CkByteData hashBytes;
    bool success;
    success = crypt.HashFile(filePath, hashBytes);

    if (!success) {
    	throw 43552;
    }
    
    CkStringBuilder sb;
    sb.AppendEncoded(hashBytes,"hex");

    std::string hash = sb.getAsString();
    
    // convert string to lower case
    std::for_each(hash.begin(), hash.end(), [](char & c) {
        c = ::tolower(c);
    });

    return hash;
}

int main(void) {

  using namespace httplib;

  httplib::Server svr;

  svr.Get("/attest", [](const httplib::Request &, httplib::Response &res) {
  
  	try {
  		std::string hash = hashFile("/home/attestation_server/file.txt");
    	std::string signedData = signData(hash);

    	std::string response;
   		response.append("{\r\n");
    	response.append("\t\"filename\": \"/home/attestation_server/file.txt\",\r\n");
    	response.append("\t\"sha256\": \"");
    	response.append(hash);
    	response.append("\",\r\n\t\"signature\": \"");
    	response.append(signedData);
    	response.append("\"\r\n}");

    	std::cout << "--- Started Attestation Procedure ---" << std::endl;
    	std::cout << "\t file.txt: " << hash << std::endl;
    	std::cout << "\t sig: " << signedData << std::endl;
    	std::cout << "--- End of Attestation Procedure ---" << std::endl;

    	res.set_content(response, "application/json");
  	} catch (int errorCode) {

  		std::string response;
   		response.append("{\r\n");

   		switch (errorCode) {
   			case 43552:
    			response.append("\t\"error\": \"An unexpected error occured while hashing the file\",\r\n");
    			break;
    		case 5477:
    			response.append("\t\"error\": \"An unexpected error occured while loading attestation keystore\",\r\n");
    			break;
    		case 6455:
    			response.append("\t\"error\": \"An unexpected error occured while loading attestation private key\",\r\n");
    			break;
    		case 6456:
    			response.append("\t\"error\": \"An unexpected error occured while loading attestation private key\",\r\n");
    			break;
    		default:
    			response.append("\t\"error\": \"An unexpected error occured.\",\r\n");
   		}
   		
   		response.append("\r\n\t\"errorCode\": \"");
    	response.append(std::to_string(errorCode));
    	response.append("\"\r\n}");

  		res.set_content(response, "application/json");
  		res.status = 500;
  	}
  });

  std::cout << "Server started at 0.0.0.0 port 8080" << std::endl;
  svr.listen("0.0.0.0", 8080);
}