
# Compile for Android
## Prerequisites
To compile Lines for Android, you need ALL of these pre-requisites:
1. A Qt6 installation compatible with your current OS (the device you will compile from)
1. A Qt6 installation compatible with Android
1. An Android SDK
1. A compatible Android NDK
1. An Android device (emulator or physical configured in developper mode)
1. A java jdk installation
1. The androiddeployqt tool (which should be automatically installed somewhere in your Qt6 for current OS folder)

##Environment variables
Once you make sure you have all of the prerequisites, you are ready to compile with CMake, from the command line.
You will also need to specify a list of environment variables (either in your system, or directly in the first CMake command, as arguments preceded by "-D"):

- CMAKE_PREFIX_PATH=\<your qt6 for android installation\>
- CMAKE_FIND_ROOT_PATH=\<your qt6 for android installation\>
- Qt6_DIR=\<your qt6 for android installation\>/lib/cmake/Qt6
- ANDROID_SDK_ROOT=\<your android sdk location\>
- ANDROID_NDK=\<your android ndk location\>
- CMAKE_C_COMPILER=\<your android ndk location\>/\<the path to the clang compiler inside the ndk folder\>
- CMAKE_TOOLCHAIN_FILE=\<your android ndk location\>/\<the path to the android.toolchain.cmake file inside the ndk folder\>
- ANDROID_PLATFORM=android-\<your target android version (a number)\>
- ANDROID_ABI=\<your target android abi\>
- QT_ANDROID_ABIS=\<your target android abi\>
- ANDROID_STL=c++_shared
- ANDROID_USE_LEGACY_TOOLCHAIN_FILE=OFF
- QT_HOST_PATH=\<your qt6 for current OS installation\>
- QT_HOST_PATH_CMAKE_DIR=\<your qt6 for current OS installation\>/lib/cmake
- QT_NO_GLOBAL_APK_TARGET_PART_OF_ALL=ON

##Compiling from the command line
Now, open a terminal in the root of your Lines folder. If it doesn't exist yet, create a folder for the Android build. Here we choose to name it build_android:

    mkdir build_android

Then, configure your project with CMake:

    cmake . -B build_android

The next step is to compile the project:

    cmake --build build_android

To create and install to the traget device the Android package, run:

    androiddeployqt --output build_android/linesDesktop/android-build --input build_android/linesDesktop/android-Lines-deployment-settings.json --gradle --install --device <your android device identification number>

##Practical example
If you're reading this for the first time, it might seem like a lot. Honestly, it kind of is.
My recommandation is that you define your commands in a single script file (.sh or .bat depending on your OS). This is what I do to avoid having to cluter my environment variables with all 15 required variables.
Once you have all the dependencies and you've defined you environment variables correctly, fully compiling Lines and installing it to a physical Android device is just as easy as a single command.

As a bonus, I will provide you with a (slightly modified for privacy) version of my own script file that I use to compile Lines. This might make the process seem more concrete.

[Here you go!](./compile_android.example)

##Future Considerations
You may have noticed that a lot of the paths given as values for the environment variables are found multiple times in the list. In the future, it would be good to implement a way to define only once every variable part of the environment variables, and to isolate in an external file (for instance) the variables that are subject to change between compilations to make it as easy as possible to change the setup used (the target android version for example). Also, some environment variables are just fixed and could probably be placed in a CMakeLists.txt file.

##Further reading
[androiddeployqt documentation](https://doc.qt.io/qt-6/android-deploy-qt-tool.html)


####[Back to the main page](./../README.md)