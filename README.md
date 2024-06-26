# t(urbo)fetch
A quick and dirty fetch utility, designed to be lightweight and fast.
It doesn't come with all the bells and whistles other fetch utilities may have but it gets the job done and doesn't take 2-3 business days to display anything.

![pgfetch](./tfetch.png)

___

# Building
All you need is CMake and a C compiler.
Run the build.sh script and enjoy.

## Installing
If you want to install tfetch just run the install.sh script instead.

## debug.sh
debug.sh should only be used if you plan on forking tfetch and make it your own, all you need it valgrind installed in order to check for memory leaks.

___

# Customizing
You can easily customize the color scheme using the provided macros defined in `src/main.c`.  
`ACCENT_COLOR` : The accent color used for titles (os, kernel, etc...)  
`BOLD_COLOR` : Only used when displaying hostname (user@hostname)  
`TEXT_COLOR` : Color used when displaying the actual information (cpu name, available memory, etc...)  
`ERROR_COLOR` : You shouldn't have to edit that one, only used when displaying errors like being unable to get your os name and version  

You can also edit `const palette` if you would like to display different colors on the left side of the fetch output.  
Make sure to use [ANSI color codes](https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit) when editing either the palette or color scheme.
