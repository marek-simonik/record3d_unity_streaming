# Record3D-Unity bridging library

**2021/07/28 Update**: Now supporting higher-quality LiDAR RGB streaming. **Updated for Record3D 1.6 and newer.**

**2020/09/17 Update**: Introduced LiDAR support

This library connects [Record3D](https://record3d.app)'s live-streaming library [`record3d_cpp`](https://github.com/marek-simonik/record3d) with [Unity demo project](https://github.com/marek-simonik/record3d_unity_demo). The goal of this library is to transform stream of RGBD video into Point Clouds that can be fed into Unity VFX Graph as attribute maps (textures).

### Building on Windows (pre-built libs included)
Building on Windows is quite cumbersome, which is why I am including pre-built library.

##### Pre-built library (the recommended, pain-free way)

The library consists the following file, which you can download: [`record3d_unity_streaming.dll`](https://github.com/marek-simonik/record3d_unity_streaming/releases/download/v1.3.0/record3d_unity_streaming.dll). You will need to make sure it is in `PATH`. The easiest way is to copy the file into `C:\Windows\System32` and/or `C:\Windows\SysWOW64`. Alternatively, you can add the folder containing the `.dll` into `PATH`.

##### Building the library yourself (the hard way)

Before installing, make sure to have installed **CMake >=3.13** (this is crucial).

1. **Build [`record3d_cpp`](https://github.com/marek-simonik/record3d)**
    1. `git clone https://github.com/marek-simonik/record3d`
    1. `cd record3d`
	2. Generate build files and open the .sln file in Visual Studio
		```
		md build
		cd build
		cmake -G "Visual Studio 16 2019" -A x64 ..
		```

	3. Open the generated .sln file in Visual Studio, change to Release build
	3. Right click on 'record3d_cpp' > Build
	4. After build, copy all .lib files to the "record3d\build\Release" folder (needed in further steps):
		```
		# In the "record3d\build" folder:
		cp libs\win\lzfse-lzfse-1.0\Release\lzfse.lib Release\
		cp libs\win\libusbmuxd\Release\usbmuxd.lib Release\
		cp libs\win\libusbmuxd\libs\libplist\Release\plist.lib Release\
		```

2. **Build `record3d_unity_streaming` (this library)**
    
    1. `git clone https://github.com/marek-simonik/record3d_unity_streaming`
    1. `cd record3d_unity_streaming`
	2. Generate build files and open the .sln file in Visual Studio
        ```
        md build
        cd build
		cmake -G "Visual Studio 16 2019" -A x64 ..
        ```

	3. Open the generated .sln file in Visual Studio, change to Release build
	1. record3d_unity_streaming > Right Click > Properties > Linker > General > Additional Library Directories > `[path-to-record3d]\build\Release;%(AdditionalLibraryDirectories)`
	2. record3d_unity_streaming > Right Click > Properties > Linker > Input > Additional Dependencies > add "`ws2_32.lib;lzfse.lib;plist.lib;usbmuxd.lib;record3d_cpp.lib;`"
	3. Right click on 'record3d_unity_streaming' > Build
	4. Make sure that `record3d_unity_streaming.dll` (located in `[record3d_unity_streaming]\build\Release`) is in the `PATH`, this can be done in two ways:
		2. Copy `record3d_unity_streaming.dll` into `C:\Windows\System32` and/or `C:\Windows\SysWOW64`
		1. Add `[record3d_unity_streaming]\build\Release` to the `PATH`
		

You can now open the [Unity demo project](https://github.com/marek-simonik/record3d_unity_demo).


#### Building on macOS/Linux

##### Pre-built library for macOS
Similar to Windows, only one `dylib` file is needed: [`librecord3d_unity_streaming.dylib`](https://github.com/marek-simonik/record3d_unity_streaming/releases/download/v1.3.0/librecord3d_unity_streaming.dylib). Copy the `.dylib` file into `/usr/local/lib/`.


##### Building the library yourself
Before building, **make sure you have installed CMake >=3.13** (this is crucial). After you have CMake installed, you can proceed to building and installing 2 libraries;

- [`record3d_cpp`](https://github.com/marek-simonik/record3d) — handles raw RGBD streaming from the Record3D iOS app via USB cable to a computer

		# Install the record3d_cpp library
		git clone https://github.com/marek-simonik/record3d
		cd record3d
		mkdir build && cd build
		cmake -DCMAKE_BUILD_TYPE=Release ..
		make -j8 record3d_cpp
		sudo make install

- `record3d_unity_streaming` (this library) — acts as a bridging library between `record3d_cpp` and Unity; produces point cloud (texture) data from `record3d_cpp`'s RGBD stream and provides API suitable for C# wrapper code

		# Install the record3d_unity_streaming library
		git clone https://github.com/marek-simonik/record3d_unity_streaming
		cd record3d_unity_streaming
		mkdir build && cd build
		cmake -DCMAKE_BUILD_TYPE=Release ..
		make -j8
		sudo make install

If the installation of both libraries was successful, you can run [the Unity demo project](https://github.com/marek-simonik/record3d_unity_demo).
