# GMVR

Based off of the work of [Bizzclaw](https://github.com/bizzclaw/gmVR)

This is a C++ module and small Lua implementation to connect OpenVR to Garry's mod.

## Usage

* Download the latest [openvr_api.dll](https://github.com/ValveSoftware/openvr/raw/master/bin/win32/openvr_api.dll) and place it in your root Garry's mod folder (Where hl2.exe is).
* Place the latest [Release](https://github.com/TehFlaminTaco/GMVR/releases) in your Garrysmod/garrysmod folder. (It should merge with the lua folder, and not create any new folders.)
* Run a single player game. Pray.

## How to Build

Ensure you have the latest [OpenVR SDK](https://github.com/ValveSoftware/openvr) downloaded somewhere on your computer.

If you've got MinGW set up correctly, replace OPENVR_SDK in compile.bat with the location you downloaded it then run compile.bat, and it will spew out a valid release.
If you're using a different Compiler, just set it up as follows. May vary depending on your compiler. (No I'm not making a MAKEFILE)

* Input File: `gm_gmvr.cpp`
* Output File: `gmcl_gmvr_win32.dll`
* Shared, Ensure that the compiler produces a DLL
* Includes:
    * ./include
    * OPENVR_SDK/headers
 * Link: OPENVR_SDK\lib\win32\openvr_api.lib

And if that doesn't work, I have no idea. Google maybe?