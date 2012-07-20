# add custom variables to this file

# OF_ROOT allows to move projects outside apps/* just set this variable to the
# absoulte path to the OF root folder

OF_ROOT = ../../../..


# USER_CFLAGS allows to pass custom flags to the compiler
# for example search paths like:
# USER_CFLAGS = -I src/objects

USER_CFLAGS = -I../../addons/ofxTuioWrapper/src/ -I../../addons/ofxTuioWrapper/src/core -I../../addons/ofxMSAInteractiveObject/src/ -I../../addons/ofxLabGui/src/ -I../../addons/ofxLabGui/src/elements -I../../addons/ofxCvHaarTracker/src -I../../addons/ofxOpenCVExtensions/src/ -I../../addons/ofxTSPS/src/ -I../../addons/ofxTSPS/src/communication -I../../addons/ofxTSPS/src/gui -I../../addons/ofxTSPS/src/utils



# USER_LDFLAGS allows to pass custom flags to the linker
# for example libraries like:
# USER_LDFLAGS = libs/libawesomelib.a

USER_LDFLAGS =

USER_LIBS = -lusb-1.0 -lwebsockets

EXCLUDE_FROM_SOURCE="../addons/ofxMSAInteractiveObject/example,../addons/ofxKinect/libs/libusb/win,../addons/ofxFileDialog,bin,.xcodeproj,obj"

# change this to add different compiler optimizations to your project

USER_COMPILER_OPTIMIZATION = -march=native -mtune=native -Os


# android specific, in case you want to use different optimizations
USER_LIBS_ARM = 
USER_LIBS_ARM7 = 
USER_LIBS_NEON = 

# android optimizations

ANDROID_COMPILER_OPTIMIZATION = -Os

NDK_PLATFORM = android-8

# uncomment this for custom application name (if the folder name is different than the application name)
#APPNAME = folderName

# uncomment this for custom package name, must be the same as the java package that contains OFActivity
#PKGNAME = cc.openframeworks.$(APPNAME)





# linux arm flags

LINUX_ARM7_COMPILER_OPTIMIZATIONS = -march=armv7-a -mtune=cortex-a8 -finline-functions -funroll-all-loops  -O3 -funsafe-math-optimizations -mfpu=neon -ftree-vectorize -mfloat-abi=hard -mfpu=vfp



