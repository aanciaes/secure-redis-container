#include <httplib.h>
#include <CkCrypt2.h>
#include <CkByteData.h>
#include <CkStringBuilder.h>
#include <CkRsaW.h>
#include <CkPfxW.h>
#include <CkPrivateKeyW.h>
#include <iostream>
#include <fstream>
#include <ctime>

// #define REDIS_SERVER_BINARY_PATH "/home/attestation_server/file.txt"
#define REDIS_CONFIG_FILE_PATH "/usr/local/etc/redis/redis.conf"
#define ATTESTATION_KEY_STORE L"/home/attestation_server/attestation-identity-key.p12"
#define ATTESTATION_KEY_STORE_PASSWORD L"thesis-manciaes"

#define MR_ENCLAVE_FILE "/home/attestation_server/mrenclave"

std::string readMrEnclave() {
  std::fstream newfile;

  newfile.open(MR_ENCLAVE_FILE, std::ios:: in );
  if (newfile.is_open()) {

    std::string tp;
    getline(newfile, tp);

    newfile.close();

    return tp;

  } else {
    throw 3321;
  }
}

std::string signData(std::string data) {

  std::wstring widestr = std::wstring(data.begin(), data.end());
  const wchar_t * strData = widestr.c_str();

  CkRsaW rsa;

  // Load the ..p12
  CkPfxW pfx;
  bool success = pfx.LoadPfxFile(ATTESTATION_KEY_STORE, ATTESTATION_KEY_STORE_PASSWORD);
  if (success != true) {
    throw 5477;
  }

  // Get the default private key.
  CkPrivateKeyW * privKey = pfx.GetPrivateKey(0);
  if (pfx.get_LastMethodSuccess() != true) {
    throw 6455;
  }

  // Import the private key into the RSA component:
  success = rsa.ImportPrivateKeyObj( * privKey);
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
  const wchar_t * base64Sig = rsa.signStringENC(strData, L"sha-256");

  // Convert to string
  std::wstring ws(base64Sig);
  std::string base64SigStr(ws.begin(), ws.end());

  return base64SigStr;
}

std::string hashFile(std::string path) {
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
  sb.AppendEncoded(hashBytes, "hex");

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

  svr.Get("/attest", [](const httplib::Request & req, httplib::Response & res) {

    try {

      if (!req.has_param("nonce")) {
        throw 2973;
      }

      std::string nonceStr = req.get_param_value("nonce");

      long nonce = std::stol(nonceStr);

      std::string redisServerHash = hashFile(REDIS_CONFIG_FILE_PATH);
      std::string redisServerSigned = signData(redisServerHash);

      std::string mrEnclave = readMrEnclave();
      std::string mrEnclaveSigned = signData(mrEnclave);

      // Pretty Print Response
      std::string response;
      response.append("{\r\n\t\"quote\": {\r\n\t\t\"challenges\": [\r\n");

      // Redis Server Object
      response.append("\t\t\t{\r\n");
      response.append("\t\t\t\t\"filename\": \"");
      response.append(REDIS_CONFIG_FILE_PATH);
      response.append("\",\r\n");
      response.append("\t\t\t\t\"hash\": \"");
      response.append(redisServerHash);
      response.append("\",\r\n\t\t\t\t\"signature\": \"");
      response.append(redisServerSigned);
      response.append("\"\r\n\t\t\t},\r\n");

      // Mr Enclave Object
      response.append("\t\t\t{\r\n");
      response.append("\t\t\t\t\"filename\": \"");
      response.append(MR_ENCLAVE_FILE);
      response.append("\",\r\n");
      response.append("\t\t\t\t\"hash\": \"");
      response.append(mrEnclave);
      response.append("\",\r\n\t\t\t\t\"signature\": \"");
      response.append(mrEnclaveSigned);
      response.append("\"\r\n\t\t\t}\r\n");

      // nonce
      response.append("\t\t],\r\n");
      response.append("\t\t\"nonce\": ");
      response.append(std::to_string((nonce + 1)));
      response.append(",\r\n");

      // complete quote signature
      response.append("\t\t\"quoteSignature\": ");
      response.append("\"Insert quote signature here. Challenges + nonce\"");
      response.append("\r\n\t}\r\n");

      response.append("}");

      // current date/time based on current system
      time_t now = time(0);

      // convert now to string form
      std::string dt = ctime( & now);
      dt = dt.substr(0, dt.length() - 1); // remove newline at the end of datetime 

      // Logging
      std::cout << "--- Started Attestation Procedure at: " << dt << " ----" << std::endl;

      // Redis Server Object
      std::cout << "\t filename: " << REDIS_CONFIG_FILE_PATH << std::endl;
      std::cout << "\t hash: " << redisServerHash << std::endl;
      std::cout << "\t sig: " << redisServerSigned << std::endl;

      std::cout << "\t ---" << std::endl;

      // Mr Enclave Object
      std::cout << "\t filename: " << MR_ENCLAVE_FILE << std::endl;
      std::cout << "\t hash: " << mrEnclave << std::endl;
      std::cout << "\t sig: " << mrEnclaveSigned << std::endl;
      std::cout << "------------------- End of Attestation Procedure -------------------" << std::endl;

      res.set_content(response, "application/json");
    } catch (int errorCode) {

      std::string response;
      response.append("{\r\n");

      switch (errorCode) {
      case 43552:
        response.append("\t\"error\": \"An unexpected error occured while hashing the file\",\r\n");
        res.status = 500;
        break;
      case 5477:
        response.append("\t\"error\": \"An unexpected error occured while loading attestation keystore\",\r\n");
        res.status = 500;
        break;
      case 6455:
        response.append("\t\"error\": \"An unexpected error occured while loading attestation private key\",\r\n");
        res.status = 500;
        break;
      case 6456:
        response.append("\t\"error\": \"An unexpected error occured while loading attestation private key\",\r\n");
        res.status = 500;
        break;
      case 2973:
        response.append("\t\"error\": \"No nonce challenge was provided. Please provide a nonce\",\r\n");
        res.status = 400;
        break;
      case 3321:
        response.append("\t\"error\": \"An unexpected error occured while reading mr enclave file\",\r\n");
        res.status = 500;
        break;
      default:
        response.append("\t\"error\": \"An unexpected error occured.\",\r\n");
      }

      response.append("\r\n\t\"errorCode\": \"");
      response.append(std::to_string(errorCode));
      response.append("\"\r\n}");

      res.set_content(response, "application/json");
    }
  });

  std::cout << "Attestation Server Started at 0.0.0.0 port 8541" << std::endl;
  svr.listen("0.0.0.0", 8541);
}