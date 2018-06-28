before run RtspServer ,must config the URL:
at main.cpp line 45,change the "RTSP_SERVER_URL" to your ip and port

eg:
book@book-desktop:/work/project/rtsp/RtspClient$ make clean;make
book@book-desktop:/work/project/rtsp/RtspClient$ ./RtspServer sintel.h264 
Rtsp server url:rtsp://192.168.1.133:8554/1
IP:192.168.1.133 Port:8554

attention£º
run "./RtspServer xxx.h264 ",will printf url "Rtsp server url:rtsp://192.168.1.133:8554/1",the url "rtsp://192.168.1.133:8554/1"
will be used for RtspClient:
book@book-desktop:/work/project/rtsp/RtspClient$ ./RtspClient rtsp://192.168.1.133:8554/1
