please see source/MediaPlayer/demo/,the others is relative library what the demo need.

if you want to test this demo,please do as follow:

1.go to buildroot path
2.'make menuconfig' and select LocalPlayer
3.'make -j8' and get the rootfs.img
4.update the rootfs.img to your device.
5.exec the command 'LocalPlayer /data/test.mp3' to play the audio.

The Files Decription:

LocalPlayer            source/MediaPlayer/demo/
libMediaPlayer.so      source/MediaPlayer
libPlaylistParser.so   source/PlaylistParser
libLPCommon.so         source/LPCommon
head file              source/ThirdParty
