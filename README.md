# wowparser
A World of Warcraft log parser

## about
`wowparser` is a high performance World of Warcraft log parser, capable of reading the text based `WoWCombatLog.txt` format and producing a binary that is optimized for O(1) and O(n)-complexity queries for consumption by log analyzer front ends.

## license
`wowparser` is free, open-source software licenses under the MIT/X11 license.

## compiling
To compile `wowparser` you need a very modern C++ toolchain that has full support for C++17 and support for C++20 concepts. The latest versions of MSVC, Clang and GCC are known to compile `wowparser` without errors. In the future, it is likely that `wowparser` will need the Boost libraries, too.

## usage
`wowparser` currently is pre-alpha software and has no command line flags yet. It'll look for the `WoWCombatLog.txt` file in the default installation path: `C:\World of Warcraft\Program Files (x86)\World of Warcraft\_retail_\Logs` and will exit if it cannot find it there.

## contributing
Contributions are welcome, open an issue if you discover any bugs. Pull requests are welcome too, there is no real etiquette due to the project being in very early stages right now. If it compiles and the code looks acceptable, I'll probably use it.
