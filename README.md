# zbaselib

zbaselib库封装了自己常用的组件，跨平台，支持linux/windows/Android。  
组件包括:  
array：动态数组  
list：双向链表  
queue:  
stack:    
socket:封装socket常用操作和一个sockbuffer，支持自增自减。有部分代码来自ngnix和libevent，libuv  
thread：封装线程常用，有部分代码来自ngnix和libevent，libuv  



目录结构：  
include/：头文件  
/*.c:源文件  
Makefile:编译库的Makefile  
test/:测试代码  


linux编译：  
1. 进入根目录：make  
2. cd test;make  



是的，这个库是重复造轮子。


email:zengdukan@163.com

