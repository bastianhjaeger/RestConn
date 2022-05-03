#include <RestConn/RestConn.h>

#include <iostream>
#include <mutex>

namespace RestConn {

std::mutex global_init_mutex;
bool global_init = false;

std::map<CURL*, bool> is_curl_ptr_init;

size_t curl_write_callback(void *ptr, size_t size, size_t nmemb, std::string* data) {
  data->append((char*) ptr, size * nmemb);
  return size * nmemb;
}

void restConn_init(CURL* _curl,
                   const std::string& _credentials,
                   const std::string& _caCertFilepath,
                   const std::string& _caPass,
                   const std::string& _token) {
  {
    std::lock_guard<std::mutex> lock_mutex (global_init_mutex);
    if ( !global_init ) {
      curl_global_init(CURL_GLOBAL_ALL);
      global_init = true;
    }
  }

  const bool curl_not_null = ( _curl != nullptr );
  const bool curl_ptr_not_init = ( is_curl_ptr_init.count(_curl) == 0 );
  if ( curl_not_null && curl_ptr_not_init ) {
    is_curl_ptr_init[_curl] = true;

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    if ( _token != "" ) {
      std::string auth = "Authorization: Bearer " + _token;
      headers = curl_slist_append(headers, auth.c_str());
    }
    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(_curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(_curl, CURLOPT_TCP_KEEPALIVE, 1L);

    curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, false); // TODO make this optional by parameter.

    if ( _caCertFilepath != "" ) {
      std::ifstream ifs(_caCertFilepath);
      std::string content( (std::istreambuf_iterator<char>(ifs) ),
                           (std::istreambuf_iterator<char>()    ) );

      curl_easy_setopt(_curl, CURLOPT_CAINFO, content.c_str());
    }

    if ( _caPass != "" ) {
      curl_easy_setopt(_curl, CURLOPT_TLSAUTH_PASSWORD, _caPass);
    }

    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, curl_write_callback);

    curl_easy_setopt(_curl, CURLOPT_USERPWD, _credentials.c_str());
  }
}

void restConn_deinit(CURL* _curl) {
  if ( _curl != nullptr ) {
    is_curl_ptr_init.erase(_curl);
    curl_easy_cleanup(_curl);
    _curl = nullptr;
  }
}

std::string restConn(CURL* _curl,
                     const std::string& _url,
                     const std::string& _credentials,
                     const std::string& _caCertFilepath,
                     const std::string& _caPass,
                     const std::string& _token,
                     const std::string& _message,
                     RESTCONN_CURL_CMD _cmdType, bool _singleCall, int _verbose) {
  std::string message = _message;

  restConn_init(_curl, _credentials, _caCertFilepath, _caPass, _token);

  if ( _curl ) {
    curl_easy_setopt(_curl, CURLOPT_URL,_url.c_str());
    if ( message.empty() ) {
      message = "";
    }

#ifndef NDEBUG
    if ( _verbose > 0 ) {
      std::cout << "message: " << message << std::endl;
    }
#endif // NDEBUG

    if ( _cmdType == POST ) {
      curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "POST");
      curl_easy_setopt(_curl, CURLOPT_POST, 1L);
#ifndef NDEBUG
      if ( _verbose > 0 ) {
        std::cout << "curl command POST" << std::endl;
      }
#endif // NDEBUG
    }
    else if ( _cmdType == PUT ) {
      curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PUT");
#ifndef NDEBUG
      if ( _verbose > 0 ) {
        std::cout << "curl command PUT" << std::endl;
      }
#endif // NDEBUG
    }
    else if ( _cmdType == GET ) {
      curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "GET");
#ifndef NDEBUG
      if ( _verbose > 0 ) {
        std::cout << "curl command GET" << std::endl;
      }
#endif // NDEBUG
    }
    else if ( _cmdType == SEARCH ) {
      curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "SEARCH");
#ifndef NDEBUG
      if ( _verbose > 0 ) {
        std::cout << "curl command GET" << std::endl;
      }
#endif // NDEBUG
    }
    else if ( _cmdType == DELETE ) {
      curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
#ifndef NDEBUG
      if ( _verbose > 0 ) {
        std::cout << "curl command DELETE" << std::endl;
      }
#endif // NDEBUG
    }

    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, message.c_str());
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, message.size());

    std::string response;
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &response);

    curl_easy_perform(_curl);

//    if ( _singleCall) {
//      restConn_deinit(_curl);
//    }

    return response;
  }

  return "";
}

json restConn(CURL* _curl,
              const std::string& _url,
              const std::string& _credentials,
              const std::string& _caCertFilepath,
              const std::string& _caPass,
              const std::string& _token,
              const json& _message,
              RESTCONN_CURL_CMD _cmdType, bool _singleCall, int _verbose) {
  std::string message = "";

  if ( !_message.empty() ) {
    message = _message.dump();
  }

  const std::string response = restConn(_curl, _url, _credentials, _caCertFilepath, _caPass, _token, message, _cmdType, _singleCall, _verbose);

  if ( response.empty() ) {
    return json();
  }
  else {
    return json::parse(response);
  }
}


RestConn::RestConn() {
  curl_ = curl_easy_init();
}

RestConn::~RestConn() {
  restConn_deinit(curl_);
}



json RestConn::call(const std::string& _url,
                    const std::string& _credentials,
                    const std::string& _caCertFilepath,
                    const std::string& _caPass,
                    const std::string& _token,
                    const std::string& _message,
                    RESTCONN_CURL_CMD _cmdType, bool _singleCall, int _verbose) const {
  return restConn(curl_, _url, _credentials, _caCertFilepath, _caPass, _token, _message, _cmdType, _singleCall, _verbose);
}

json RestConn::call(const std::string& _url,
                    const std::string& _credentials,
                    const std::string& _caCertFilepath,
                    const std::string& _caPass,
                    const std::string& _token,
                    const json& _message,
                    RESTCONN_CURL_CMD _cmdType, bool _singleCall, int _verbose) const {
  return restConn(curl_, _url, _credentials, _caCertFilepath, _caPass, _token, _message, _cmdType, _singleCall, _verbose);
}

} // namespace RestConn
