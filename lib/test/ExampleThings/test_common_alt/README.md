This folder and corresponding test runner folders in the main tests (test_common_thing1, and test_commong_thing2) represent an alternative structure for common tests, and demonstrate a number of advanced topics:

1. Modularization of platform-specific tests into more more specific groups of functionality
2. Benefits of namespacing the test groups
3. How to have multiple setUp and tearDown structures for testing a given runtime platform

## Modularizing platform-specific tests

For simplicity, `lib/test/ExampleThings` contains a flat set of files corresponding to various
targeted runtime platforms. `test_common_alt` demonstrates that these can easily be organized
into folders. You will want to be sure to match the folder structure when importing the header
file into the main test runner (see e.g. `test/test_common_thing1/test_common_thin1.cpp`)

## Namespacing test files

The tests here for Thing1 and Thing2 demonstrate how namespacing helps to mitigate potential conflicts. Here, both test sets have a `thing` variable defined, which is made possible by encapsulating each test set into its own namespace.

## Multiple setUp and tearDown functions for a given target

The main test runners under `test/test_common_thing1` and `test/test_common_thing2` demonstrate how it is possible to have distinct test scaffolding for tests within a given runtime target. The effect of the given structure is to create distinct test groups, resulting in multiple test groups that are considered to be "common". Note, this has implications for what you might add to the test_ignore configurations in your platformio.ini.

Please pay attention to the structure of the main test runners under the ProjectDir/test. Note that the following naive structure will not work for us:

test
 + - test_common_alt
   + - test_thing1
     > test_thing1.cpp
   + - test_thing2
     > test_thing2.cpp

While this seems like it should work, PlatformIO takes the entire structure under the subdirectory
to test (test_common_alt in this case) as a single test group. As a result, we will end up with
multiple definitions, e.g. to setUp and tearDown. Thus, we need to use the less ideal structure:

test
 + - test_common_thing1
   > test_common_thing1.cpp
 + - test_common_thing
   > test_common_thing2.cpp

thereby creating distinct test groups, each with its own scaffolding.