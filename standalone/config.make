# add custom variables to this file

# OF_ROOT allows to move projects outside apps/* just set this variable to the
# absoulte path to the OF root folder

OF_ROOT = ../../..


# USER_CFLAGS allows to pass custom flags to the compiler
# for example search paths like:
# USER_CFLAGS = -I src/objects

USER_CFLAGS = -g -w -I/usr/include/libusb-1.0/ -Iaddons/ofxKinect/libs/libusb/osx/include -Iaddons/ofxOpenCVExtensions/src/ -Iaddons/ofxCvHaarTracker/src -Iaddons/ofxFileDialog/src/ -Iaddons/ofxKinect/src/ -Iaddons/ofxKinect/libs/libfreenect -Iaddons/ofxLabGui/src/ -Iaddons/ofxLabGui/src/elements -Iaddons/ofxMSAInteractiveObject/src/ -Iaddons/ofxTSPS/src/ -Iaddons/ofxTSPS/src/communication -Iaddons/ofxTSPS/src/gui -Iaddons/ofxTSPS/src/utils -Iaddons/ofxTuioWrapper/src/ -Iaddons/ofxTuioWrapper/src/core -Iaddons/ofxWebUI_LAB/src/ -Iaddons/ofxWebUI_LAB/libs/b64/include -Iaddons/ofxWebUI_LAB/libs/b64/src -Iaddons/ofxWebUI_LAB/libs/libwebsockets/include -Iaddons/ofxWebUI_LAB/libs/protobuf/include/google/protobuf





# USER_LDFLAGS allows to pass custom flags to the linker
# for example libraries like:
# USER_LD_FLAGS = libs/libawesomelib.a

USER_LDFLAGS = 


# use this to add system libraries for example:
# USER_LIBS = -lpango
 
USER_LIBS = -lusb-1.0 -lwebsockets -L/usr/local/lib


# change this to add different compiler optimizations to your project

USER_COMPILER_OPTIMIZATION = -march=native -mtune=native -Os


EXCLUDE_FROM_SOURCE="addons/ofxKinect,addons/ofxMSAInteractiveObject/example,addons/ofxKinect/libs/libusb/win,addons/ofxFileDialog,bin,.xcodeproj,obj,.git"
