# wowparser
A World of Warcraft combat log parser.

## about
`wowparser` is a high performance World of Warcraft log parser, capable of 
reading the text based `WoWCombatLog.txt` format and producing a binary that is 
optimized for O(1) and O(n)-complexity queries for consumption by log analyzer 
front ends.

## license
`wowparser` is free, open-source software licenses under the MIT/X11 license.

## compiling
To compile `wowparser` you need a very modern C++ toolchain that has full 
support for C++17 and support for C++20 concepts, additionally `wowparser` has
a dependency on `Boost.Iostreams` and `Boost.Program_options`. The latest 
versions of MSVC, Clang and GCC are known to compile `wowparser` without 
errors.

## usage
`wowparser` currently is pre-alpha software and is not ready for production
use.  It'll look for the `WoWCombatLog.txt` file in the default installation 
path: `C:\Program Files (x86)\World of Warcraft\_retail_\Logs` and will exit if 
it cannot find it there, you can specify a path with the `-L` or `--combat-log`
command line option. A full list of options can be viewed by invoking 
`wowparser -h`.

## contributing
Contributions are welcome, open an issue if you discover any bugs. Pull 
requests are welcome too, there is no real etiquette due to the project being 
in very early stages right now. If it compiles and the code looks acceptable, 
I'll probably use it.
