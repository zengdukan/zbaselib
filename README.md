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



是的，这个库是重复造轮子。关个重复造轮子，有的人觉得没有必要性，我觉得不是这样。只有会造轮子且掌握了轮子的原理，才能造出更好的东西。即使轮子造的不好，对原理的理解和个人成长还是很有帮助的。当然现实工作以现实为主。


email:zengdukan@163.com

