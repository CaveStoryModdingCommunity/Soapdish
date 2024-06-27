# Soapdish

It's a dll mod for Soaprun that lets you change what server it connects to.

See `server_config.ini` for more info.

# Usage

- Download a release zip file from the releases page
  - It should only contain a handful of dll files and a single `server_config.ini` file. If it has more than this then you probably downloaded the source code on accident
- Unzip the files next to `soaprun.exe`
- Open `server_config.ini` with your favorite text editor and follow the instructions
- Start `soaprun.exe` to join the server

# Building

I used vcpkg to manage dependencies and Visual Studio 2022 to compile the code, so if you have those then you're probably good.

## Dependencies

- [curllib](https://curl.se/)
	- [zlib](https://www.zlib.net/)
- [simpleini](https://github.com/brofield/simpleini)


# Credits

- Brayconn - The actual patching code
- Periwinkle - The code that I stole from [his Cave Story dll base](https://github.com/periwinkle9/CSCustomDLLTemplate) (sorry that I didn't try and replicate all the other compilation options)