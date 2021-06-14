#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include <RestConn/json.hpp>
using json = nlohmann::json;

namespace RestConn {

enum RESTCONN_CURL_CMD {
  POST, PUT, GET, SEARCH, DELETE
};

void restConn_deinit();

void restConn_init(const std::string& _credentials,
                   const std::string& _caCertFilepath,
                   const std::string& _caPass,
                   const std::string& _token);

std::string restConn(const std::string& _url,
                     const std::string& _credentials,
                     const std::string& _caCertFilepath,
                     const std::string& _caPass,
                     const std::string& _token,
                     const std::string& _message,
                     RESTCONN_CURL_CMD _cmdType = POST, bool _singleCall = false, int _verbose = 0);

json restConn(const std::string& _url,
              const std::string& _credentials,
              const std::string& _caCertFilepath,
              const std::string& _caPass,
              const std::string& _token,
              const json& _message,
              RESTCONN_CURL_CMD _cmdType = POST, bool _singleCall = false, int _verbose = 0);

} // namespace RestConn
