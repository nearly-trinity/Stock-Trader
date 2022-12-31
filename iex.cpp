#include <cjson/cJSON.h> // https://github.com/DaveGamble/cJSON
#include <curl/curl.h>
#include <iostream>
#include <memory>
 
const char* REQUEST_URL = "https://api.twelvedata.com/time_series?symbol=AAPL&interval=1min&outputsize=12&apikey=YOUR_API_KEY_HERE";
 
// Take response data and write into the string
std::size_t write_callback(const char* data, std::size_t size, std::size_t count, std::string* out) {
    const std::size_t result = size * count;
    out->append(data, result);
 
    return result;
}
 
int main() {
    std::unique_ptr<std::string> responseData(new std::string());
    cJSON* json = nullptr;
    CURL* curl = curl_easy_init();
 
    curl_easy_setopt(curl, CURLOPT_URL, REQUEST_URL);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, responseData.get());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_perform(curl);
 
    json = cJSON_Parse(responseData->c_str());
    cJSON* metaField = cJSON_GetObjectItem(json, "meta");
    cJSON* valuesField = cJSON_GetObjectItem(json, "values");
    cJSON* symbolField = cJSON_GetObjectItem(metaField, "symbol");
    cJSON* closeField = cJSON_GetObjectItem(cJSON_GetArrayItem(valuesField, 0), "close");
 
    std::cout << "Received symbol: " << cJSON_GetStringValue(symbolField) << ", ";
    std::cout << "close: " << cJSON_GetStringValue(closeField) << std::endl;
 
    cJSON_Delete(json);
    curl_easy_cleanup(curl);
 
    return 0;
}