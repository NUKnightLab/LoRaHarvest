# EmbeddedProject

Starter project setup for PlatformIO embedded projects with Unity unit testing


## Testing

```
  $ cd EmbeddedProject
  $ pio test
  $ pio test -e uno
  $ pio test -e adafruit_feather_m0
  $ pio test -e native
```

## Overview

This repository is designed to be a starter project for building a new
embedded application using PlatformIO with Unity unit testing. It serves a
similar purpose to the example unit testing projects that PlatformIO provides
(see https://github.com/platformio/platformio-examples/tree/develop/unit-testing)
however, I have encoded some personal preferences and lessons learned here.

Some features in particular:

 * A common set of print / println statements to support standard out / Serial
   output on both native and embedded environments.
 * Organization of tests into the lib folder with minimal test runner code
   under the main test folder; lib-first code organization that
   encourages modular development.
 * Examples showing sub-organization of tests under a given target
 * Demonstration of multiple test scaffoldings (setUp and tearDown functions)
   within a given target test group.


## Ideology

The code organization is designed to encourage modular development and to
facilitate a testing approach of minimizing the load on embedded devices
through development and testing cycles. To this end, you should consider
organizing your project such that as much of the logic as possible can be
natively executed and tested by the common tests, reserving embedded-only
code and tests for specific resources only available on embedded devices.

As an example, consider a configuration library that is meant to read a
config file from an SD card that you wish to use with maybe an Arduino SD
shield, or with an Adalogger Feather Wing.

In this scenario, consider abstracting any of the logical aspects of the code,
(e.g. the interpolation of config strings into typed values) from the SD card
specific aspects of the code (e.g. read & write functions). The former would be
tested with common tests, while the latter, which depends on an actual SD card
reader, would be tested in the embedded tests.

## print and println

A common set of print / println functions are provided in the Console library.
This should greatly facilitate the ability to write code that will run both
natively and embedded. Instead of `Serial.print` and `Serial.println` use these
provided functions which work across runtime targets.

This project example is littered with a handful of print statements that are
there mostly for demonstration purposes. In reality, you would not rely so
much on print statements during coding.


## Much ado about "unit testing"

It's all too easy to get hung up about the nature of "proper" unit testing.
A quick search on embedded or Arduino-specific unit testing will yield a
number of purists who believe that you should not run any of your tests
on the embedded platform, but should mock out all of your resources. I say
good luck with that. My recommendation is not to get too hung up about right
and wrong and instead reach for what is practical. Try to push most of your
development cycles into native test iterations for the sake of your own sanity.
Try to keep embedded tests minimal, but don't wrangle your project into some
rediculous arbitrary split along platform lines in order to acheive some sense
of purism. PlatformIO makes it insanely easy to test your code on the actual
device where it will run -- you should take advantage of this where it makes
sense to do so.

## Usage

Fork or copy this repository, open it in PlatformIO and run the tests:

```
  $ cd EmbeddedProject
  $ pio test
  $ pio test -e uno
  $ pio test -e adafruit_feather_m0
  $ pio test -e native
```

I recommend running specific test targets. Run `test -e native` through most
of your development cycles while you are working through the logic of your
code, then run `test` or embedded-specific targets for occasional sanity checks.
As described above, some of your tests will be embedded-specific which, in my
opinion, is okay and is what PlatformIO's testing resources are designed to do.
PlatformIO gives you good tools for running embedded tests; use them.

Note: While desktop tests are provided for completeness, it seems unlikely that
most projects will need them. Unless you are building accompanying tools that
are desktop specific, your native tests will probably go into common so that
they get executed on embedded targets also.

You may need to add your device to platformio.ini. I have included the Uno and
the Adafruit Feather M0 because this is what I have access to and work with. I
will probably add in the Feather 32u4 soon as well.

## Non-library projects

The structure here is library-centric to encourage modular design and because
setting up a library project is less intuitive than getting started on a main
project.

If you are creating a main runtime project, simply build out that code in the
src directory with main.cpp. I would still recommend identifying functional
areas of your code that you can factor into lib. Tests for main program code
would likely be organized under the main test folder rather than the lib
test folder.

## Arduino IDE compatibility?

Note that if you want Arduino IDE compatibility, rather than a main.cpp, you
would have an .ino file that is named the same as this project. PlatformIO will
automatically convert this to a .cpp file during compilation.

For example, we might have `src/EmbeddedProject/EmbeddedProject.ino` then in
the platformio.ini file under the `[platformio]` config section:
`src_dir = EmbeddedProject`. In this way, you could also have multiple
related projects that you target by modifying the src_dir parameter as
needed.

Please note, I have not actually tested this in the Arduino IDE. There is the
obvious caveat that you don't really have access to the unit testing setup
in the Arduino IDE, or anything that is not PlatformIO. Thus I recommend one of
the several PlatformIO approaches for development (VS Code, Atom, and
command-line are all options).
