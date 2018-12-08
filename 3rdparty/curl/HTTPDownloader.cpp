/**
 * HTTPDownloader.cpp
 *
 * A simple C++ wrapper for the libcurl easy API.
 *
 * Written by Uli Köhler (techoverflow.net)
 * Modified to include logging for blogcpp.
 * Published under CC0 1.0 Universal (public domain)
 */
#include "HTTPDownloader.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include <sstream>
#include <iostream>

using namespace std;

#ifdef WITH_DEBUGLOG
stringstream ss_debuglog;
#endif

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    string data((const char*) ptr, (size_t) size * nmemb);
    *((stringstream*) stream) << data << endl;
    return size * nmemb;
}

HTTPDownloader::HTTPDownloader() {
    curl = curl_easy_init();
}

HTTPDownloader::~HTTPDownloader() {
    curl_easy_cleanup(curl);
}

string HTTPDownloader::download(const std::string& url) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    /* example.com is redirected, so we tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
    std::stringstream out;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
    /* Perform the request, res will get the return code */
    CURLcode res = curl_easy_perform(curl);
    /* Check for errors */
    if (res != CURLE_OK) {
#ifdef WITH_DEBUGLOG
        ss_debuglog << "curl_easy_perform() failed: " << curl_easy_strerror(res);
        DebugLog::debuglog(ss_debuglog.str());
#endif
        return "";
    }
    return out.str();
}
