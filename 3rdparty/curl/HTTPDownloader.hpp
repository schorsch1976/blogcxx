/**
 * HTTPDownloader.hpp
 *
 * A simple C++ wrapper for the libcurl easy API.
 *
 * Written by Uli Köhler (techoverflow.net)
 * Modified to include logging for blogcpp.
 * Published under CC0 1.0 Universal (public domain)
 */
#ifndef HTTPDOWNLOADER_HPP
#define HTTPDOWNLOADER_HPP

#include <string>
#ifdef WITH_DEBUGLOG
#include "../../src/DebugClass.h"
#endif

/**
 * A non-threadsafe simple libcURL-easy based HTTP downloader
 */
class HTTPDownloader {
  public:
    HTTPDownloader();
    ~HTTPDownloader();
    /**
     * Download a file using HTTP GET and store in in a std::string
     * @param url The URL to download
     * @return The download result
     */
    std::string download(const std::string& url);
  private:
    void* curl;
};

#endif  /* HTTPDOWNLOADER_HPP */
