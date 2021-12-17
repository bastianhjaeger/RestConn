#include <RestConn/RestConn.h>

#include<iostream>
#include <curl/curl.h>

namespace RestConn {

CURL* curl = nullptr;

size_t curl_write_callback(void *ptr, size_t size, size_t nmemb, std::string* data) {
  data->append((char*) ptr, size * nmemb);
  return size * nmemb;
}

void restConn_init(const std::string& _credentials,
                   const std::string& _caCertFilepath,
                   const std::string& _caPass,
                   const std::string& _token) {
  if ( !curl ) {
    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    if ( _token != "" ) {
      std::string auth = "Authorization: Bearer " + _token;
      headers = curl_slist_append(headers, auth.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);


    if ( _caCertFilepath != "" ) {
      std::ifstream ifs(_caCertFilepath);
      std::string content( (std::istreambuf_iterator<char>(ifs) ),
                           (std::istreambuf_iterator<char>()    ) );

      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
      curl_easy_setopt(curl, CURLOPT_CAINFO, content.c_str());
    }

    if ( _caPass != "" ) {
      curl_easy_setopt(curl, CURLOPT_TLSAUTH_PASSWORD, _caPass);
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_callback);

    curl_easy_setopt(curl, CURLOPT_USERPWD, _credentials.c_str());
  }
}

void restConn_deinit() {
  if ( curl ) {
    curl_easy_cleanup(curl);
    curl = nullptr;
  }
}

std::string restConn(const std::string& _url,
                        const std::string& _credentials,
                        const std::string& _caCertFilepath,
                        const std::string& _caPass,
                        const std::string& _token,
                        const std::string& _message,
                        RESTCONN_CURL_CMD _cmdType, bool _singleCall, int _verbose) {
  std::string message = _message;

  restConn_init(_credentials, _caCertFilepath, _caPass, _token);

  if ( curl ) {
    curl_easy_setopt(curl, CURLOPT_URL,_url.c_str());
    if ( message.empty() ) {
      message = "";
    }

#ifndef NDEBUG
    if ( _verbose > 0 ) {
      std::cout << "message: " << message << std::endl;
    }
#endif // NDEBUG

    if ( _cmdType == POST ) {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
      curl_easy_setopt(curl, CURLOPT_POST, 1L);
#ifndef NDEBUG
      if ( _verbose > 0 ) {
        std::cout << "curl command POST" << std::endl;
      }
#endif // NDEBUG
    }
    else if ( _cmdType == PUT ) {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
#ifndef NDEBUG
      if ( _verbose > 0 ) {
        std::cout << "curl command PUT" << std::endl;
      }
#endif // NDEBUG
    }
    else if ( _cmdType == GET ) {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
#ifndef NDEBUG
      if ( _verbose > 0 ) {
        std::cout << "curl command GET" << std::endl;
      }
#endif // NDEBUG
    }
    else if ( _cmdType == SEARCH ) {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "SEARCH");
#ifndef NDEBUG
      if ( _verbose > 0 ) {
        std::cout << "curl command GET" << std::endl;
      }
#endif // NDEBUG
    }
    else if ( _cmdType == DELETE ) {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
#ifndef NDEBUG
      if ( _verbose > 0 ) {
        std::cout << "curl command DELETE" << std::endl;
      }
#endif // NDEBUG
    }

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, message.size());

    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_perform(curl);

    if ( _singleCall ) {
      restConn_deinit();
    }

    return response;
  }

  return "";
}

json restConn(const std::string& _url,
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

  const std::string response = restConn(_url, _credentials, _caCertFilepath, _caPass, _token, message, _cmdType, _singleCall, _verbose);

  if ( response.empty() ) {
    return json();
  }
  else {
    return json::parse(response);
  }
}

} // namespace RestConn
