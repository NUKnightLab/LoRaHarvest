# Unit Testing


## Organization

Tests are generally organized to target a platform type. e.g.:

 * `test_desktop` targets desktop only
 * `test_embedded` targets embedded devices
 * `test_common` targets both

Targeting is managed by the `test_ignore` parameter for an environment in the
platformio.ini.

Broad platform type (ie. embedded vs. non-embedded) is not the only way to
think about test organization. A particular organizational case is the need
of distinct setUp and tearDown functions for a particular test group. Unity
does not give us a lot of control over this, so instead we can create distinct
test groups. This approach is exemplified here by the use of separate "common"
test groups for testing Thing1 and Thing2. The "common" here refers to common
across platforms (i.e. these tests will be run for both native and embedded),
however as distinct test groups, Thing1 tests will have distinct setUp and
tearDown from Thing2 tests.

Aside from this very specific need of distinct scaffolding, it is probably
best to stick with the platform-oriented test organization. Also, in as far
as you can make do with a single set of setUp and tearDown functions, you
probably should -- the structure here is merely illustrative if you need it
since it is not very intuitive how to go about creating separate scaffoldings.
(As a side note: I am super-open to other approaches here. I tried different
ways to create distinct structures and this is the only one that really
worked at all. That said, a lot of projects probably won't even need it).

Note: If you are developing an embedded project, it seems unlikely that you
will actually need desktop specific testing. In this case, the desktop
testing folders can be deleted. The common tests should be retained with the
idea that logic that is not specific to an embedded platform can be more
efficiently iterated by specifically targeting the native platform and then
subsequently tested for sanity on additional targeted devices.

## Principles

The standard unit-testing principles apply of testing small units of
functionality in isolation, with tests being as independent as possible.

This is a bit more of a challenge for embedded projects. Tests often depend
on very specific resources that can be difficult to mock. Test cycles in
general can be heavy due to the need to upload and run the tests on a
specific device.

For this latter reason, common tests should be used where possible with
iterative development targeting native and intermittent sanity checks against
all devices.


## More Info

More information about PIO Unit Testing:

https://docs.platformio.org/page/plus/unit-testing.html
