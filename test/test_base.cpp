#include <RestConn/RestConn.h>

#include <thread>
#include <gtest/gtest.h>

namespace rc = RestConn;

const std::string TEST_URI = "https://jsonplaceholder.typicode.com/todos/1";
const std::string TEST_CRED = "user:password";
const std::string TEST_CERT = "/path/to/your/cert.pem";
const std::string TEST_PASS = "PasswordForCert";
const std::string TEST_TOKEN = "a^4&234crx12$3ys1^441";

TEST(RestConnTest, simple) {
  json jResponse;
  json jRequest;

  jResponse = rc::restConn(TEST_URI, "", "", "", "", jRequest,
        rc::GET, true, 0);

  std::cout << jResponse.dump(3) << std::endl;
}

TEST(RestConnTest, withCredantials) {
  json jResponse;
  json jRequest;

  EXPECT_NO_THROW( jResponse = rc::restConn(TEST_URI, TEST_CRED, TEST_CERT, TEST_PASS, "", jRequest,
      rc::GET, true, false) );

  std::cout << jResponse.dump(3) << std::endl;
}

TEST(RestConnTest, withToken) {
  json jResponse;
  json jRequest;

  EXPECT_NO_THROW( jResponse = rc::restConn(TEST_URI, "", "", "", TEST_TOKEN, jRequest,
      rc::GET, true, false) );

  std::cout << jResponse.dump(3) << std::endl;
}


