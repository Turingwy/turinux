# turinux-os
an unix-like toy os kernel(unfinished and being developed)

***
####目前功能 （2016-01-26）
* bootloader加载(更换为grub软盘加载)
* 进入protected mode(grub引导内核之前已经做好,但在内核临时页表建立完成之后又重新创建了gdt)
* 加载内核
* 几个终端输出的函数,printk.
* 建立全局描述符表
    * 由于打算做的是纯页式内存管理，gdt中仅有4个描述符且基址全为0，也就是只用了段表的权限管理功能
* 页式内存管理
    * 内核空间3G-4G,所以内核空间可以映射0G-1G真实物理地址，暂不能访问高端地址。
    * 刚刚进入保护模式之后，创建临时page table(包含两个页目录项)，在加载完成之后将page table重新替换
* 建立中断向量表
    * 目前仅仅完成中断函数的框架，待以后实现进程调度时对32号中断进行补充
* 采用空闲链表的方式管理整个内存可见物理地址空间，实现对物理内存的管理
* kmalloc() 以及 kfree() 采用侵入式链表的方式, 将申请到的内存4k页进行管理



