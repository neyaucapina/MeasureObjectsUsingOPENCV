TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_calib3d249 -lopencv_contrib249 -lopencv_core249 -lopencv_features2d249 -lopencv_flann249 -lopencv_gpu249 -lopencv_highgui249 -lopencv_imgproc249 -lopencv_legacy249 -lopencv_ml249 -lopencv_nonfree249 -lopencv_objdetect249 -lopencv_ocl249 -lopencv_photo249 -lopencv_stitching249 -lopencv_superres249 -lopencv_ts249 -lopencv_video249 -lopencv_videostab249
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../opencv/build/x86/vc11/lib/ -lopencv_calib3d249d -lopencv_contrib249d -lopencv_core249d -lopencv_features2d249d -lopencv_flann249d -lopencv_gpu249d -lopencv_highgui249d -lopencv_imgproc249d -lopencv_legacy249d -lopencv_ml249d -lopencv_nonfree249d -lopencv_objdetect249d -lopencv_ocl249d -lopencv_photo249d -lopencv_stitching249d -lopencv_superres249d -lopencv_ts249d -lopencv_video249d -lopencv_videostab249d

INCLUDEPATH += $$PWD/../../opencv/build/include
DEPENDPATH += $$PWD/../../opencv/build/include/opencv2
