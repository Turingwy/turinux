# What is turinux?
* turinux is an unix-like toy os kernel,
* relies to a great degree on [MIT 6.828 Xv6](http://pdos.csail.mit.edu/6.828/2011/xv6.html)

# Why am I Bulid this Project?

This project is my first slightly large project, aimed to control the knowledge I learned.

#How to Compile
* Compile environment: Arch Linux

* Needed: git, make, gcc, ld, nasm, qemu, mkfs.minix

* Instructions
``` 
    git clone git@github.com:turingwy/turinux.git
    cd turinux
    make
    make qemu
```

# 目前功能
* bootloader加载
* 进入protected mode(grub引导内核之前已经做好,但在内核临时页表建立完成之后又重新创建了gdt)
* 加载内核
* 几个终端输出的函数,`printk()`.
* 全局描述符表
* 页式内存管理
    * 内核空间3G-4G,所以内核空间可以映射0G-1G真实物理地址，暂不能访问高端地址。
* 建立中断向量表
* 管理整个内存可见物理地址空间，实现对物理内存的管理
    * `kpalloc()`分配物理页
    * `kpfree()`回收物理页
* 采用侵入式链表的方式, 将申请到的内存4k页进行管理
    * `kmalloc(size_t sz)`分配sz大小的内存
    * `kfree()`回收当前指针指向内存
* 简单进程管理与调度
    * 非抢占式内核，用户态时钟中断触发或调用`sleep()`主动放弃时间片时产生调度
    * 暂未实现优先级调度
    * 每一个进程都有一个进程控制块PCB(`struct proc`)，PCB位于进程内核栈顶
    * 每一个进程都拥有独立的页表，当调用`fork()`创建新进程时，`fork()`简单的把父进程页表的拷贝赋给子进程，当进程写数据时，通过page_fault中断处理函数进行写时复制。
    * 当进程调用`exit()`时，讲自身进程状态设为TASK_ZOMBIE，并唤醒父进程进行回收处理。
* 基于minix v1的文件系统
    * minix v1文件系统十分简陋，且没有日志层。但概念清晰，且资源丰富
    * 通过file descriptor屏蔽底层的硬件访问，对上层用户程序而言，所有的IO操作(包括stdin,stdout)都使用文件描述符进行操作
* 几个基本系统调用，如`fork`,`exec`,`exit`等。

#References
* [MIT 6.828 xv6](http://pdos.csail.mit.edu/6.828/2011/xv6.html)
* [Xv6 中文文档](https://github.com/ranxian/xv6-chinese)
* [hurlex-doc](https://github.com/hurley25/hurlex-doc)
