before run RtspClient ,must export LD_LIBRARY_PATH=./lib

eg:
book@book-desktop:/work/project/rtsp/RtspClient$ make clean;make
book@book-desktop:/work/project/rtsp/RtspClient$ export LD_LIBRARY_PATH=./lib
book@book-desktop:/work/project/rtsp/RtspClient$ ./RtspClient rtsp://192.168.1.133:8554/1

attention:
if want to use jrtplib,must :
book@book-desktop:/work/project/rtsp/RtspClient$ make clean;make JRTPLIB=YES
book@book-desktop:/work/project/rtsp/RtspClient$ export LD_LIBRARY_PATH=./lib
book@book-desktop:/work/project/rtsp/RtspClient$ ./RtspClient rtsp://192.168.1.133:8554/1

说明：

a、LD_LIBRARY_PATH 这个环境变量是大家最为熟悉的，它告诉loader：在哪些目录中可以找到共享库。可以设置多个搜索目录，这些目录之间用冒号分隔开。export LD_LIBRARY_PATH=./lib，然后再运行编译，即可通过。这种方法只是暂时修改路径，在重启shell后会失效。
或者是配置在环境变量文件/etc/profile中，重启或者source /etc/profile 生效，source 只在本控制台生效。

b、永久生效的方法为修改动态链接库配置文件/etc/ld.so.conf，或者在/etc/ld.so.conf.d里创建一个新文件，并把需要的目录加到这个文件里。具体方法如下：(说明：这种修改动态链接库配置的方式需要使用超级用户权限，不然没有对共享库配置文件的写权限)


详细说明参考我的博客：http://www.cnblogs.com/yuweifeng/p/7666868.html