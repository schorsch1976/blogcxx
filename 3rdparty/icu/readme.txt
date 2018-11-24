 
18 Apr 2017
====================================================================================================================== 
 url: http://www.npcglib.org/~stathis/blog/precompiled-icu 
====================================================================================================================== 
These are the pre-built ICU Libraries v59.1. They are compiled with Cygwin/MSVC  
for 32/64-bit Windows, using Visual Studio 2017. 
 
----------------------------------------------------------------------- 
32-bit shared release runtime dlls: bin\icu*.dll 
32-bit shared release import libs: lib\icu*.lib 
32-bit shared debug runtime dlls: bin\icu*d.dll 
32-bit shared debug import libs: lib\icu*d.lib 
32-bit static release libs: lib\sicu*.lib 
32-bit static debug libs: lib\sicu*d.lib 
----------------------------------------------------------------------- 
64-bit shared release runtime dlls: bin64\icu*.dll 
64-bit shared release import libs: lib64\icu*.lib 
64-bit shared debug runtime dlls: bin64\icu*d.dll 
64-bit shared debug import libs: lib64\icu*d.lib 
64-bit static release libs: lib64\sicu*.lib 
64-bit static debug libs: lib64\sicu*d.lib 
----------------------------------------------------------------------- 
 
When using them you may need to specify an environment variable ICU_DATA pointing to the data/ folder. 
This is where the icudtXXl.dat file lives. 
    e.g. set ICU_DATA=F:\icu\data 
====================================================================================================================== 
If you have any comments or problems send me an email at: 
stathis <stathis@npcglib.org> 
