xrtcdemo
========

Building xrtcdemo by "testWebRTC.xcodeproj".
--------

1. It depends on librtc(https://github.com/adychen/librtc.git), Enter direcory of "librtc/scripts": 
--------
    (1) run "sh build.sh ios webrtc" for google webrtc, 

    (2) copy "librtc/libs/ios/libwebrtc_all.a" into "xrtcdemo/xrtc/libs/ios"

    (3) run "sh build.sh ios xrtc" for librtc
    It will generate ios project under "librtc/bld/librtc.xcodeporj";
    Add and link to librtc.a in "testWebRTC.xcodeproj".

2. It depends on gloox(http://github.com/PeterXu/sipstack.git)
--------
    Add "sipstack/gloox-1.0.10/build/ios/gloox/gloox.xcodeproj" into "testWebRTC.xcodeproj".
    And link to libgloox.a in "testWebRTC.xcodeproj".

3. When running by "armv7" in two devices(A and B),
--------
    (1) Login by account: "2013" on A, 
    (2) Login by account: "2015" on B,
    (3) Call by account "2015" on A,
    (4) And then audio connected between A and B.

