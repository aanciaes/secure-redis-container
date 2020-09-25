#include "CkCrypt2.h"
#include "CkByteData.h"
#include "CkStringBuilder.h"
#include <iostream>

int main(void)
    {
    // This example requires the Chilkat API to have been previously unlocked.
    // See Global Unlock Sample for sample code.

    CkCrypt2 crypt;

    // Choose the hash algorithm.
    // Can be  "sha1", "sha256", "sha384", "sha512", "md2", "md5", "haval", "ripemd128", "ripemd160","ripemd256", or "ripemd320".
    crypt.put_HashAlgorithm("sha256");

    CkByteData hashBytes;
    bool success;
    success = crypt.HashFile("./file.txt",hashBytes);

    // Let's examine the hash as a hex string.
    CkStringBuilder sb;
    sb.AppendEncoded(hashBytes,"hex");
    std::cout << "SHA256 hash = " << sb.getAsString() << "\r\n";
    }
