#include <iostream>
#include <string>
#include <curl/curl.h>

using namespace std;

class Http {
private:
    const char* url;
    CURL* curl;
    string token; // Stores the token

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
        size_t totalSize = size * nmemb;
        response->append((char*)contents, totalSize);
        return totalSize;
    }

    void RetrieveToken() {
    // Spotify API token endpoint
    string tokenUrl = "https://accounts.spotify.com/api/token";

    string clientId = "e68713239e204c378b00c1feab80ad06";
    string clientSecret = "1cd5d94b6eca4e3cb16f7f2985028a4d";

    // Prepare POST data
    string postData = "grant_type=client_credentials&client_id=" + clientId + "&client_secret=" + clientSecret;

    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, tokenUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

    // Add the "Content-Type" header
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Capture the response
    string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers); // Clean up headers

    if (res != CURLE_OK) {
        cerr << "Error retrieving token: " << curl_easy_strerror(res) << endl;
        token = ""; // Empty token on failure
    } else {
        // Extract token from response (simple parsing example)
        size_t tokenPos = response.find("\"access_token\":\"");
        if (tokenPos != string::npos) {
            size_t start = tokenPos + 16; // Length of "access_token\":\""
            size_t end = response.find("\"", start);
            token = response.substr(start, end - start);
            cout << token << endl;
        } else {
            cerr << "Failed to parse token from response!" << endl;
            token = "";
        }
    }
}


public:
    Http() {
        curl = curl_easy_init();
        if (curl) {
            RetrieveToken(); // Retrieve token upon initialization
        }
    }

    void SetUrl(string url) {
        this->url = url.c_str();
        curl_easy_setopt(curl, CURLOPT_URL, this->url);
        cout << "URL is set to: " + url << endl;
    }

    string Get() {
        string response;

        if (token.empty()) {
            return "Token is not available. Please check token retrieval.";
        }

        struct curl_slist* headers = NULL;
        string authHeader = "Authorization: Bearer " + token;
        headers = curl_slist_append(headers, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET"); // Explicitly set GET method
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            return "Error: " + string(curl_easy_strerror(res));
        } else if (httpCode != 200) {
            return "HTTP Error Code: " + to_string(httpCode) + "\nResponse: " + response;
        } else {
            return "GET request successful!\nResponse: " + response;
        }
    }

    void RetrieveTokenWithAuthCode(const string& authorizationCode) {
        string tokenUrl = "https://accounts.spotify.com/api/token";

        string clientId = "e68713239e204c378b00c1feab80ad06";
        string clientSecret = "1cd5d94b6eca4e3cb16f7f2985028a4d";
        string redirectUri = "http://localhost:8888/callback";

        // Prepare POST data
        string postData = "grant_type=authorization_code&code=" + authorizationCode +
                        "&redirect_uri=" + redirectUri +
                        "&client_id=" + clientId +
                        "&client_secret=" + clientSecret;

        curl_easy_setopt(curl, CURLOPT_URL, tokenUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        // Add headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            cerr << "Error retrieving token: " << curl_easy_strerror(res) << endl;
            token = "";
        } else {
            // Extract token from response
            size_t tokenPos = response.find("\"access_token\":\"");
            if (tokenPos != string::npos) {
                size_t start = tokenPos + 16;
                size_t end = response.find("\"", start);
                token = response.substr(start, end - start);
                cout << "Token retrieved: " << token << endl;
            } else {
                cerr << "Failed to parse token from response!" << endl;
                token = "";
            }
        }
    }

    ~Http() {
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }
};

int main() {
    Http http;

    // Spotify API endpoint
    string url = "https://api.spotify.com/v1/me";

    //https://api.spotify.com/v1/me/player
    
    // Set the URL and perform the GET request
    http.SetUrl(url);
    cout << http.Get() << endl;

    return 0;
}
