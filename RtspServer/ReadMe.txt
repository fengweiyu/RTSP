before run RtspServer ,must config the URL:
at main.cpp line 49,change the "RTSP_SERVER_PORT" to your port

eg:
book@book-desktop:/work/project/rtsp/RtspClient$ make clean;make
book@book-desktop:/work/project/rtsp/RtspClient$ ./RtspServer 192.168.43.235 sintel.h264 
Rtsp server url:rtsp://192.168.43.235:8554/1
eg:your IP:192.168.43.235 Port:8554

attention：
1.
run "./RtspServer xxx xxx.h264 ",will printf url "Rtsp server url:rtsp://192.168.43.235:8554/1",the url "rtsp://192.168.43.235:8554/1"
will be used for RtspClient or vlc(媒体->打开网络串流->网络,填入url):
book@book-desktop:/work/project/rtsp/RtspClient$ ./RtspClient rtsp://192.168.43.235:8554/1
2.
only support use udp to trans rtp packet