#include <httplib.h>
#include <CkCrypt2.h>
#include <CkByteData.h>
#include <CkStringBuilder.h>
#include <CkRsaW.h>
#include <CkPfxW.h>
#include <CkPrivateKeyW.h>
#include <CkBinData.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>

#define SEVER_ACCEPTED_USERNAME "redis-proxy-cd497e6b-10cf-4ed2-be63-18b6b16375f6"
#define SEVER_ACCEPTED_PASSWORD "2grvTkqUhS7wE4R4WRjnuXTLzsxnAu"

#define REDIS_SERVER_BINARY_PATH "/usr/local/bin/redis-server"
#define REDIS_CONFIG_FILE_PATH "/usr/local/etc/redis/redis.conf"
#define ATTESTATION_SERVER_BIN_PATH "/home/attestation_server/attestation-server"

#define ATTESTATION_KEY_STORE L"/home/attestation_server/attestation-identity-key.p12"
#define ATTESTATION_KEY_STORE_PASSWORD L"K!waNR4HHx4UxRwXxR*vXiG4v"

#define REDIS_MR_ENCLAVE_FILE "/home/redis/mrenclave"
#define ATTESTATION_SERVER_MR_ENCLAVE_FILE "/home/attestation_server/mrenclave"

std::string readMrEnclave(std::string file_path) {
    std::fstream newfile;

    newfile.open(file_path, std::ios:: in );
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

void authenticateRequest(const httplib::Request & req) {
    if (req.has_header("Authorization")) {
        auto header = req.get_header_value("Authorization");

        std::string delimiter = " ";
        std::string auth = header.substr((header.find(delimiter) + 1), header.length());

        CkBinData bd;

        bool success = bd.AppendEncoded(auth.c_str(),"base64");
        if (!success) {
            throw 66233;
        }

        std::string decodedAuth = bd.getString("utf-8");

        delimiter = ":";
        std::string username = decodedAuth.substr(0, decodedAuth.find(delimiter));
        std::string password = decodedAuth.substr((decodedAuth.find(delimiter) + 1), decodedAuth.length());

        if (username != SEVER_ACCEPTED_USERNAME || password != SEVER_ACCEPTED_PASSWORD) {
            throw 49213;
        }

    } else {
        throw 32222;
    }
}

void logAttestationRequest(std::string nonceStr, std::string remoteIp, time_t start_time_string, std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point end) {
    std::string start_time = ctime( & start_time_string);
    start_time = start_time.substr(0, start_time.length() - 1); // remove newline at the end of datetime

    std::cout << "Remote attestation request (" << nonceStr << ") from " << remoteIp << " started at " << start_time << " and took " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
}

int main(void) {

    using namespace httplib;

    httplib::Server svr;

    std::list<long> nonceList;

    svr.Get("/attest", [&](const httplib::Request & req, httplib::Response & res) {

        try {
            authenticateRequest(req);

            if (!req.has_param("nonce")) {
                throw 2973;
            }

            std::string nonceStr = req.get_param_value("nonce");

            long nonce = std::stol(nonceStr);
            bool found = (std::find(nonceList.begin(), nonceList.end(), nonce) != nonceList.end());

            if (found) {
                throw 59873;
            } else {
                nonceList.insert(nonceList.end(), nonce);
            }

            // current date/time based on current system
            time_t now = time(0);
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

            std::string redisServerHash = hashFile(REDIS_SERVER_BINARY_PATH);
            std::string redisConfigHash = hashFile(REDIS_CONFIG_FILE_PATH);
            std::string redisMrEnclave = readMrEnclave(REDIS_MR_ENCLAVE_FILE);
            std::string attestServerHash = hashFile(ATTESTATION_SERVER_BIN_PATH);
            std::string attstServerMrEnclave = readMrEnclave(ATTESTATION_SERVER_MR_ENCLAVE_FILE);

            std::string quote;
            quote.append(redisServerHash);
            quote.append("|");
            quote.append(redisConfigHash);
            quote.append("|");
            quote.append(redisMrEnclave);
            quote.append("|");
            quote.append(attestServerHash);
            quote.append("|");
            quote.append(attstServerMrEnclave);
            quote.append("|");
            quote.append(std::to_string((nonce + 1)));

            std::string quoteSignature = signData(quote);

            // Pretty Print Response
            std::string response;
            response.append("{\r\n\t\"quote\": {\r\n\t\t\"challenges\": [\r\n");

            // Redis Server Object
            response.append("\t\t\t{\r\n");
            response.append("\t\t\t\t\"filename\": \"");
            response.append(REDIS_SERVER_BINARY_PATH);
            response.append("\",\r\n");
            response.append("\t\t\t\t\"hash\": \"");
            response.append(redisServerHash);
            response.append("\"\r\n\t\t\t},\r\n");

            // Redis Config Object
            response.append("\t\t\t{\r\n");
            response.append("\t\t\t\t\"filename\": \"");
            response.append(REDIS_CONFIG_FILE_PATH);
            response.append("\",\r\n");
            response.append("\t\t\t\t\"hash\": \"");
            response.append(redisConfigHash);
            response.append("\"\r\n\t\t\t},\r\n");

            // Redis Mr Enclave Object
            response.append("\t\t\t{\r\n");
            response.append("\t\t\t\t\"filename\": \"");
            response.append(REDIS_MR_ENCLAVE_FILE);
            response.append("\",\r\n");
            response.append("\t\t\t\t\"hash\": \"");
            response.append(redisMrEnclave);
            response.append("\"\r\n\t\t\t},\r\n");

            // Attestation Server Object
            response.append("\t\t\t{\r\n");
            response.append("\t\t\t\t\"filename\": \"");
            response.append(ATTESTATION_SERVER_BIN_PATH);
            response.append("\",\r\n");
            response.append("\t\t\t\t\"hash\": \"");
            response.append(attestServerHash);
            response.append("\"\r\n\t\t\t},\r\n");

            // Attestation Server Mr Enclave Object
            response.append("\t\t\t{\r\n");
            response.append("\t\t\t\t\"filename\": \"");
            response.append(ATTESTATION_SERVER_MR_ENCLAVE_FILE);
            response.append("\",\r\n");
            response.append("\t\t\t\t\"hash\": \"");
            response.append(attstServerMrEnclave);
            response.append("\"\r\n\t\t\t}\r\n");

            // nonce
            response.append("\t\t],\r\n");
            response.append("\t\t\"nonce\": ");
            response.append(std::to_string((nonce + 1)));
            response.append(",\r\n");

            // complete quote signature
            response.append("\t\t\"quoteSignature\": \"");
            response.append(quoteSignature);
            response.append("\"\r\n\t}\r\n");

            response.append("}");

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

            logAttestationRequest(nonceStr, req.remote_addr, now, begin, end);

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
            case 32222:
                response.append("\t\"error\": \"No authorization header provided\",\r\n");
                res.status = 401;
                break;
            case 66233:
                response.append("\t\"error\": \"An unexpected error occured while decoding authorization header\",\r\n");
                res.status = 500;
                break;
            case 49213:
                response.append("\t\"error\": \"Authentication Failed\",\r\n");
                res.status = 403;
                break;
            case 59873:
                response.append("\t\"error\": \"Nonce has already been used. Request denied.\",\r\n");
                res.status = 403;
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