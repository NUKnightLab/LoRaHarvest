#ifndef TEST_COMMON_H
#define TEST_COMMON_H

/*
class TestLoRa {
public:
  TestLoRa();
  virtual int read();
private:
  int _index = 0;
  int _buffer[255];
};
*/

/**
 * Tests are namespaced for organizational sanity. It seems likely that this is unnecessary,
 * however you may need to be more specific about naming things that are not namespaced.
 */
namespace Test_CommonThings {
    void test_all();
}

#endif