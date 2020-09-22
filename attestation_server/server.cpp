#include <httplib.h>
#include <CkCrypt2.h>
#include <CkByteData.h>
#include <CkStringBuilder.h>
#include <iostream>

int main(void)
{
  using namespace httplib;

  httplib::Server svr;

  svr.Get("/attest", [](const httplib::Request &, httplib::Response &res) {
  	// This example requires the Chilkat API to have been previously unlocked.
    // See Global Unlock Sample for sample code.

    CkCrypt2 crypt;

    // Choose the hash algorithm.
    // Can be  "sha1", "sha256", "sha384", "sha512", "md2", "md5", "haval", "ripemd128", "ripemd160","ripemd256", or "ripemd320".
    crypt.put_HashAlgorithm("sha256");

    CkByteData hashBytes;
    bool success;
    success = crypt.HashFile("/home/attestation_server/file.txt",hashBytes);

    // Let's examine the hash as a hex string.
    CkStringBuilder sb;
    sb.AppendEncoded(hashBytes,"hex");

    std::string response;
    response.append("{\r\n");
    response.append("\t\"filename\": \"/home/attestation_server/file.txt\",\r\n");
    response.append("\t\"sha256\": \"");
    response.append(sb.getAsString());
    response.append("\"\r\n}");

    std::cout << "--- Started Attestation Procedure ---" << std::endl;
    std::cout << "\t file.txt: " << sb.getAsString() << std::endl;
    std::cout << "--- End of Attestation Procedure ---" << std::endl;

    res.set_content(response, "application/json");
  });

  std::cout << "Server started at 0.0.0.0 port 8080" << std::endl;
  svr.listen("0.0.0.0", 8080);
}