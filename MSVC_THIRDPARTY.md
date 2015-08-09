## Third party

While some tools in homm3tools can be built without dependencies, others link
against the following third party libraries:
* libudis86 
* Qt5 
* zlib 

###libudis86 (used by various)

libudis86 is provided in the repository and set to be part of the homm3tools
VS solution and as such has no special build instructions.

###Qt5 (used by hde_launcher_mod)

(instructions from http://codeyarns.com/2013/12/13/how-to-create-qt-applications-using-visual-studio/)

* Download a suitable Qt installer from the downloads page and install it. Make 
sure you pick an installer that is 32-bit and built for the Visual Studio 
version that you have.

* Download and install the Qt Visual Studio add-in. You can find it at the bottom 
of the downloads page. Make sure you pick an installer that matches the Visual 
Studio version that you have. 

* Open your Visual Studio. In the toolbar, there is a new menu named Qt5. Go to Qt5 > Qt Options. 
In the Qt Versions tab, add the path to the Qt you installed in Step 1. Note that you need to add the path to the parent directory of the bin directory of Qt. For example, on my particular installation this path was C:\Qt\Qt5.2.0\5.2.0\msvc2012.

####zlib 1.2.8 (used by gzip_utils, which in turn is used by various)

(instructions from https://raw.githubusercontent.com/telegramdesktop/tdesktop/master/MSVC.md)

http://www.zlib.net/ > Download [**zlib source code, version 1.2.8, zipfile format**](http://zlib.net/zlib128.zip)

Extract to **homm3tools\3rdparty\zlib-1.2.8**

#####Building library

* Open in VS2013 **homm3tools\3rdparty\zlib-1.2.8\contrib\vstudio\vc11\zlibvc.sln** > One-way upgrade – **OK**
* We are interested only in **zlibstat** project, but it depends on some custom pre-build step, so build all
* For **Debug** configuration
  * zlibstat Properties > C/C++ > Code Generation > Runtime Library = **Multi-threaded Debug (/MTd)** – **OK**
* For **Release** configuration
  * zlibstat Properties > C/C++ > Code Generation > Runtime Library = **Multi-threaded (/MT)** – **OK**
* Build Solution for Debug configuration – only **zlibstat** project builds successfully
* Build Solution for Release configuration – only **zlibstat** project builds successfully
