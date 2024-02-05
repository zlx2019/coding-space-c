# Linux系统编程

本篇博客参考于《Linux C编程一站式学习》第三部分.

## IO系统调用

通过之前的学习，我们学习到了C标准库的IO函数最终都是通过**系统调用**来实现IO操作，系统调用其实就是调用系统提供的函数。

下面我们就开始学习这些更加底层的**系统函数**，系统函数的用法必须结合Linux内核的工作原理来理解，因为系统函数正是内核提供给应用程序的接口，而要理解内核的工作原理，必须熟练掌握C语言，因为内核也是用C语言写的，我们在描述内核工作原理时必然要用“指针”、“结构体”、“链表”这些名词来组织语言，就像只有掌握了英语才能看懂英文书一样，只有学好了C语言才能看懂我描述的内核工作原理。

### 1. 汇编程序的Hello world

之前我们学习了如何使用C标准IO库读写文件，本章详细讲解这些IO操作都是如何实现的。**所有的IO操作最终都是在操作系统内核中做的**，以前我们用的C标准IO库最终也是通过系统调用把IO操作从用户空间传给内核，然后让内核去做IO操作。

本章和下一章会介绍内核中I/O子系统的工作原理。首先看一个打印Hello world的汇编程序，了解I/O操作是怎样通过系统调用传给内核的。

```assembly
# 定义数据段
.data
	# 定义 msg .ascii字符串
	msg: .ascii "Hello，world!\n"
	# 计算 msg 的字符长度
	# . 表示当前地址，减去 msg 的首地址即得到 msg 的长度
	len = . - msg

# 代码段
.text
	# 指定程序入口点
	.global _start

# 程序入口
_start:
	movl		$len, %edx	# 将msg字符串长度移动到 edx 寄存器
	movl		$msg, %ecx	# 将msg字符串首地址移动到 ecx 寄存器
	movl		$1, %ebx		# 将$1(标准输出的文件描述符)移动到 ebx 寄存器
	movl		$4, %eax		# 将 write系统函数的调用号'4'移动到 eax 寄存器，表示要执行write系统调用
	
	# 触发系统调用，调用 eax 寄存器中的write系统调用
	# ebx、ecx、edx寄存器分别保存着write系统调用需要的三个参数
	# ebx 保存着要写入的文件描述符
	# ecx 保存着要写的缓冲区首地址
	# edx 保存着缓冲区的字节数
	int			$0x80
	
	
	movl		$0, %ebx		# 将 0 移动到 ebx 寄存器，作为程序退出状态码
	movl		$1, %eax		# 将$1(sys_exit 系统函数，用于退出程序)移动到 eax 寄存器.
	int			$0x80				# 触发系统调用，执行sys_exit退出程序
```

`.data`: 用于定义数据段。也就是程序内存中用于存储全局变量、静态变量等数据的区域;

`.text`: 用于定义代码段。代码段是存储程序指令的区域，程序在执行时会从代码段中读取指令并执行。

然后就可以在Linux系统中进行**汇编**、**链接**、**运行**:

```bash
$ as hello.s -o hello.o
$ ld hello.o -o hello
$ ./hello
Hello，world!
```

这段汇编相当于如下C代码:

```c
#include <unistd.h>

// 定义字符串和字符串长度
char msg[14] = "Hello, world!\n";
#define msg_len 14

int main(){
    // 直接通过系统调用函数，将msg 写入到 标准输出中
    write(1,msg,msg_len);
    // 结束程序
    _exit(0);
}
```

.data段有一个标号msg，代表字符串"Hello, world!\n"的首地址，相当于C程序的一个全局变量。注意在C语言中字符串的末尾隐含有一个'\0'，而汇编指示.ascii定义的字符串末尾没有隐含的'\0'。汇编程序中的len代表一个常量，它的值由当前地址减去符号msg所代表的地址得到，换句话说就是字符串"Hello, world!\n"的长度。现在解释一下这行代码中的.，汇编器总是从前到后把汇编代码转换成目标文件，在这个过程中维护一个地址计数器，当处理到每个段的开头时把地址计数器置成0，然后每处理一条汇编指示或指令就把地址计数器增加相应的字节数，在汇编程序中用.可以取出当前地址计数器的值，是一个常量。

在_start中调了两个系统调用，第一个是write系统调用，第二个是以前讲过的_exit系统调用。在调write系统调用时，eax寄存器保存着write的系统调用号4，ebx、ecx、edx寄存器分别保存着write系统调用需要的三个参数。ebx保存着文件描述符，进程中每个打开的文件都用一个编号来标识，称为文件描述符，文件描述符1表示标准输出，对应于C标准I/O库的stdout。ecx保存着输出缓冲区的首地址。edx保存着输出的字节数。write系统调用把从msg开始的len个字节写到标准输出。

### 2. C语言的系统调用I/O库

之前说到了C语言中的标准IO库都是通过系统调用实现的，那么究竟如何实现的呢？下面介绍一下标准IO与系统调用IO之间的关系:

- `fopen(3)`

  通过`open(2)`系统调用函数打开指定的文件，返回一个文件描述符(file descriptor)，其实就是一个int类型的编号，然后会分配一个`FILE`结构体用于记录该文件的描述符、IO缓冲区和当前读写位置等信息，最后返回这个`FILE`结构体地址。

- `fgetc(3)`

  通过传入的`FILE*`参数找到对应文件的描述符、IO缓冲区等信息，判断能否从I/O缓冲区中读到下一个字符，如果能读到就直接返回该字符，否则调用read(2)系统调用，将文件描述符传到内核，让内核读取该文件的数据到IO缓冲区，然后返回下一个字符。

  注意，对于C标准IO来说，打开的文件由`FILE*`指针标识，而对于内核来说，打开的文件由文件描述符来标识，文件描述符由`open(2)`系统调用获取，在使用`read`、`write`、`close`系统调用时都需要传入文件描述符。

- `fputc(3)`

  判断该文件的I/O缓冲区是否有空间再存放一个字符，如果有空间则直接保存在I/O缓冲区中并返回，如果I/O缓冲区已满就调用write(2)，让内核把I/O缓冲区的内容写回文件。

- `fclose(3)`

  如果I/O缓冲区中还有数据没写回文件，就调用write(2)写回文件，然后调用close(2)关闭文件，释放FILE结构体和I/O缓冲区。

`open`、`read`、`write`、`close`等系统函数称为无缓冲(UnBuffered I/O)函数，因为它们位于C标准IO库的底层，也就是这些函数在支撑着C标准IO库。C应用程序在读写文件时可以调用标准IO库，也可以直接调用底层的UnBufferIO函数库，那么用哪一组好呢？这个要分情况，具体的特点如下:

- 用Unbuffered I/O函数每次读写都要进内核，调一个系统调用比调一个用户空间的函数要慢很多，所以在用户空间开辟I/O缓冲区还是必要的，用C标准I/O库函数就比较方便，省去了自己管理I/O缓冲区的麻烦。
- 用C标准I/O库函数要时刻注意I/O缓冲区和实际文件有可能不一致，在必要时需调用`fflush(3)`。
- 在UNIX的传统中，贯彻一个理念: **一切皆是文件**，IO函数不仅可以用于读取普通文件，还可以读取设备，比如终端或网络等设备。在读写设备时通常是不希望有缓冲的，例如向代表网络设备文件写数据就是希望数据通过网络设备发送出去，而不希望只写到缓冲区里就算完事儿了，当网络设备接收到数据时应用程序也希望第一时间被通知到，所以网络编程通常直接调用Unbuffered I/O函数。

C标准IO库是C标准的一部分，而UnBuffered IO函数是Unix标准的一部分，在所有支持C语言的平台之上都可以使用C标准库，而只有UNIX平台上才能使用UnBuffered IO函数，所以C标准IO函数声明在`stdio.h`头文件中，而`read`、`write`等UnBuffered IO函数声明在`unistd.h`头文件。

在支持C语言的非UNIX操作系统上，标准I/O库的底层可能由另外一组系统函数支持，例如Windows系统的底层是Win32 API，其中读写文件的系统函数是ReadFile、WriteFile。

> 事实上Unbuffered I/O这个名词是有些误导的，虽然write系统调用位于C标准库I/O缓冲区的底层，但在write的底层也可以分配一个内核I/O缓冲区，所以write也不到一定是直接写到文件的，也可能写到内核I/O缓冲区中，至于究竟写到了文件中还是内核缓冲区中对于进程来说是没有差别的，如果进程A和进程B打开同一文件，进程A写到内核I/O缓冲区中的数据从进程B也能读到，而C标准库的I/O缓冲区则不具有这一特性（想一想为什么）。

### 3. 文件描述符

前面我们不止一次的提到过**文件描述符**这个东西，那么它到底是什么呢？接下来我们就可以好好的唠唠它了。

系统中的每个进程在Linux内核中都有一个对应的`task_struct`结构体，用来维护进程相关的信息，称之为**进程描述符(Process Descriptor)**，而在操作系统理论中称之为**进程控制块(PCB,Process Control Block)**。每一个进程描述符(task_struct)中都有一个指针指向一个`files_struct`结构体，称为文件描述符表，表中的每一个项都包含一个指针，指向一个已经打开的文件指针。如下图所示:

<img src="../asstes/1.4进程的文件描述符表.png" style="zoom:40%;" />

通过图片可以看到，文件描述符表其实就是一个序列表，它也拥有索引。用户程序并不能直接访问内核中的文件描述符表，而只能通过它的索引(即0、1、2、3这些数字)来访问每一个文件，而这些索引就称为**文件描述符(File Descriptor)**，用int类型变量保存。

每个程序启动时会自动打开三个文件：标准输入、标准输出和标准错误输出。在C标准库中分别使用`FILE*`类型的`stdin`、`stdout`、`stderr`表示。这三个文件是最先打开的，所以描述符分别是0、1、2，保存在对应的`FILE`结构体中。头文件`unistd.h`中有如下的宏定义来标识这三个文件描述符:

```c
// unistd.h

#define	 STDIN_FILENO	0	/* standard input file descriptor */
#define	STDOUT_FILENO	1	/* standard output file descriptor */
#define	STDERR_FILENO	2	/* standard error file descriptor */
```

### 4. open/close 系统调用

#### open

> `open`系统调用函数可以打开或创建一个文件。

```c
#include <fcntl.h>

函数声明
int open(const char* pathname, int flags);
int open(const char* pathname, int flags, mode_t mode);
返回值：成功返回新分配的文件描述符，出错返回-1并设置errno
```

在Man Page中open函数有两种形式，一种带两个参数，一种带三个参数，其实在C代码中open函数是这样声明的：

```c
int open(const char* pathname, int flags, ...);
```

最后的可变参数可以是0个或1个，由flags参数中的标志位决定，见下面的详细说明。

- `pathname`: 要打开或创建的文件名，和fopen一样，pathname既可以是相对路径也可以是绝对路径。

- `flags`: 文件打开模式，该参数有一系列常数值可供选择，可以同时选择多个常数使用`|`运算符连接起来，所以这些常数的宏定义都以o_开头，表示or。

  必选项: 以下三个常数中必须指定一个，并且这三个常数仅允许同时存在一个。

  - O_RDONLY: 只读;
  - O_WRONLY: 只写;
  - O_RDWR: 可读可写;

  以下可选项可以同时指定0个或多个，和必选项按位或起来作为flags参数。可选项有很多，这里只介绍一部分，其它选项可参考open(2)的Man Page:

  - O_APPEND: 表示追加写入;
  - O_CREAT: 如果文件不存在，则创建该文件。使用此选项需要提供第三个参数`mode`，表示该文件的访问权限;
  - O_EXCL: 如果同时指定了O_CREAT，并且文件已存在，则出错返回;
  - O_TRUNC: 如果文件已存在，并且以只写或可读可写方式打开，则将其长度截断（Truncate）为0字节;
  - O_NONBLOCK: 对于设备文件，以O_NONBLOCK方式打开可以做非阻塞I/O（Nonblock I/O），非阻塞I/O在下一节详细讲解;

- `mode`: 指定文件权限，可以用八进制数表示，比如0644表示`-rw-r--r--`，也可以用S_IRUSR、S_IWUSR等宏定义按位或起来表示，详见open(2)的Man Page。要注意的是，文件权限由open的mode参数和当前进程的umask掩码共同决定。

  如下补充: 

  Shell进程的umask掩码可以用umask命令查看：

  ```bash
  $ umask
  022
  ```

  使用 touch 命令创建一个文件时，创建权限是0666，而 touch 进程继承了Shell进程的umask掩码，所以最终的文件权限是 0666 &~ 022 = 0644:

  ```bash
  $ touch a.txt
  ❯ ll
  -rw-r--r--    1 zero  staff     0 Jan  2 22:09 a.txt
  ```

  同样的道理，用gcc编译生成可执行文件时，创建权限为0777，而最终的文件权限是 0777 &~ 022 = 0755:

  ```bash
  $ gcc main.c
  $ ll
  -rwxr-xr-x    1 zero  staff  16840 Jan  2 22:11 a.out
  -rw-r--r--    1 zero  staff     25 Jan  2 22:11 main.c
  ```

  我们看到的都是被umask掩码修改之后的权限，那么如何证明touch或gcc创建文件的权限本来应该是0666和0777呢？我们可以把Shell进程的umask改成0，再重复上述实验:

  ```bash
  $ umask 0 
  $ umask
  000
  $ touch a.txt
  $ ll
  -rw-rw-rw-    1 zero  staff      0 Jan  2 22:13 a.txt
  ```

  ```bash
  $ gcc main.c
  $ ll
  -rwxrwxrwx    1 zero  staff  16840 Jan  2 22:14 a.out
  -rw-r--r--    1 zero  staff     25 Jan  2 22:11 main.c
  ```

注意open函数与C标准I/O库的fopen函数有些细微的区别：

- 以可写的方式fopen一个文件时，如果文件不存在会自动创建，而open一个文件时必须明确指定O_CREAT才会创建文件，否则文件不存在就出错返回。
- 以w或w+方式fopen一个文件时，如果文件已存在就截断为0字节，而open一个文件时必须明确指定O_TRUNC才会截断文件，否则直接在原来的数据上改写。



#### close

> `close`系统调用函数用于关闭一个已经打开的文件。

```c
#include <unistd.h>

int close(int fd);
返回值：成功返回0，出错返回-1并设置errno
```

`fd`: 要关闭的文件描述符。

需要说明的是，当一个进程终止时，内核对该进程所有尚未关闭的文件描述符调用close关闭，所以即使用户程序不调用close，在终止时内核也会自动关闭它打开的所有文件。但是对于一个长年累月运行的程序（比如网络服务器），打开的文件描述符一定要记得关闭，否则随着打开的文件越来越多，会占用大量文件描述符和系统资源。

由open返回的文件描述符一定是该进程尚未使用的最小描述符。由于程序启动时自动打开文件描述符0、1、2，因此第一次调用open打开文件通常会返回描述符3，再调用open就会返回4。可以利用这一点在标准输入、标准输出或标准错误输出上打开一个新文件，实现重定向的功能。例如，首先调用close关闭文件描述符1，然后调用open打开一个常规文件，则一定会返回文件描述符1，这时候标准输出就不再是终端，而是一个常规文件了，再调用printf就不会打印到屏幕上，而是写到这个文件中了。后面要讲的dup2函数提供了另外一种办法在指定的文件描述符上打开文件。

```c
int main() {
    printf("");
    // 关闭标准输出，文件描述符表索引为1的位置将会空出来
    close(1);

    // 再创建一个文件，而这个文件的描述符则会使用1来标识
    // 这样就利用这个机制替代了标准输出
    int fd = open("./stdout.log",O_RDWR | O_CREAT,0644);
    if(fd != -1){
        // 向标准输出中输入数据
        // 此时并不会输出到终端，而是输出到 fd 描述符对应的文件中
        fprintf(stdout,"Hello，World! \n");
        // 关闭文件
        close(fd);
    }
    return 0;
}
```

下面通过一些习题进一步熟悉这两个系统调用函数:

1. 打开文件/home/akae.txt用于写操作，以追加方式打开
2. 打开文件/home/akae.txt用于写操作，如果该文件不存在则创建它
3. 打开文件/home/akae.txt用于写操作，如果该文件已存在则截断为0字节，如果该文件不存在则创建它
4. 打开文件/home/akae.txt用于写操作，如果该文件已存在则报错退出，如果该文件不存在则创建它

```c
#define FILE "/home/akae.txt"

int main() {
    // 1. 打开文件/home/akae.txt用于写操作，以追加方式打开
    int fd = open(FILE,O_WRONLY | O_APPEND);
    assert(fd != -1);
    close(fd);

    // 2. 打开文件/home/akae.txt用于写操作，如果该文件不存在则创建它
    int fd = open(FILE,O_WRONLY | O_CREAT,0666);
    assert(fd != -1);
    close(fd);

    // 3.打开文件/home/akae.txt用于写操作，如果该文件已存在则截断为0字节，如果该文件不存在则创建它
    int fd = open(FILE,O_WRONLY | O_CREAT | O_TRUNC);
    assert(fd != -1);
    close(fd);

    // 4. 打开文件/home/akae.txt用于写操作，如果该文件已存在则报错退出，如果该文件不存在则创建它
    int fd = open(FILE, O_WRONLY | O_CREAT | O_EXCL);
    assert(fd != -1);
    close(fd);

    return 0;
}
```

### 5. read/write 系统调用

#### read

> `read`系统调用函数可以从打开的设备或文件中读取数据。

```c
#include <unistd.h>

ssize_t	 read(int fd, void* buf, size_t count);
返回值：成功返回读取的字节数，出错返回-1并设置errno，如果在调read之前已到达文件末尾，则这次read返回0;
```

`fd`: 要读取的文件对应的文件描述符;

`buf`: 用于保存读取到的数据的缓冲区;

`count`: 请求读取的字节数;

参数count是请求读取的字节数，读上来的数据保存在缓冲区buf中，同时文件的当前读写位置向后移。注意: 这个读写位置和C标准IO库的读写位置有可能不同，这个读写位置是记录在内核中的，而C标准IO库的读写位置是用户空间IO缓冲区中的位置。比如用fgetc读一个字节，fgetc有可能从内核中预读1024个字节到I/O缓冲区中，再返回第一个字节，这时该文件在内核中记录的读写位置是1024，而在FILE结构体中记录的读写位置是1。

返回值类型为`ssize_t`，表示有符号的`size_t`，这样既可以返回0(表示读取到达文件末尾)也可以返回-1(表示出错)。正常返回值表明了本次读取的实际字节数，有些情况下实际读取到的字节数会小于`count`，例如:

- 读常规文件时，在读到count个字节之前已到达文件末尾。例如，距文件末尾还有30个字节而请求读100个字节，则read返回30，下次read将返回0。
- 从终端设备读，通常以行为单位，读到换行符就返回了。
- 从网络读，根据不同的传输层协议和内核缓存机制，返回值可能小于请求的字节数，后面socket编程部分会详细讲解。

#### write

> `write`系统调用函数可以向打开的设备或文件中写入数据。

```c
#include <unistd.h>

ssize_t write(int fd, const void* buf, size_t count);
返回值：成功返回写入的字节数，出错返回-1并设置errno
```

`fd`: 要写入的文件对应的文件描述符;

`buf`: 要写入的数据缓冲区;

`count`: 要写入的字节数;

向常规文件中写入时，返回值通常和`count`请求写入的字节数是一致的，但是向终端设备或网络设备写入则不一定,下面会说到原因。

#### 阻塞(Blocking)

对常规文件进行读写是不会产生阻塞的，以读取为例，不管读取多少字节，`read`一定会在有限的时间内返回。而从终端设备或网络设备读取则不一定，如果从终端输入的数据没有换行符，调用`read`读取终端就会一直阻塞，如果网络设备没有接受到数据包，调用`read`读取网络设备也会产生阻塞，至于阻塞多长时间也是不确定的，如果一直没有输入换行符或者接受到数据包就会一直阻塞下去。

现在明确一下**阻塞(Blocking)**这个概念:

当进程调用了一个阻塞的系统函数时，该进程会被置于**睡眠状态(Sleep)**，这时内核会调度其他的进程进行执行，直到该进程等待的事件发生了(比如网络上接到了数据包，或者调用sleep指定的睡眠时间到了)，它才有可能继续运行。

与睡眠状态相对的是**运行状态(Running)**，在Linux内核中，处于运行状态的进程分为两种情况:

- 正在被调度运行: CPU处于该进程的上下文环境中，程序计数器（eip）里保存着该进程的指令地址，通用寄存器里保存着该进程运算过程的中间结果，正在执行该进程的指令，正在读写该进程的地址空间。
- 就绪状态: 该进程不需要等待什么事件发生，随时都可以执行，但CPU暂时还在执行另一个进程，所以该进程在一个就绪队列中等待被内核调度。

系统中可能同时有多个就绪的进程，那么该调度谁执行呢？内核的调度算法是基于优先级和时间片的，而且会根据每个进程的运行情况动态调整它的优先级和时间片，让每个进程都能比较公平地得到机会执行，同时要兼顾用户体验，不能让和用户交互的进程响应太慢。

下面这个小案例实现从终端读取数据，再写回终端:

```c
int main(void) {
    char buf[10]; // 定义缓冲区
    int n;  // 读取到的字节数
    // 读取标准输入
    n = read(STDIN_FILENO,buf, sizeof(buf));
    if(n == -1){
        perror("read stdin failed:");
        exit(1);
    }
    // 将数据写回标准输出
    write(STDOUT_FILENO,buf,n);
    return 0;
}
```

执行结果如下:

```bash
$ ./a.out
hello (回车)
hello

$ ./a.out
hello world(回车)
hello worl$ d
zsh: command not found: d
```

第一次执行a.out的结果很正常，而第二次执行的过程有点特殊，现在分析一下：

1. Shell进程创建a.out进程，a.out进程开始执行，而Shell进程睡眠等待a.out进程退出。
2. a.out调用read时睡眠等待，直到终端设备输入了换行符才从read返回，read只读走10个字符，剩下的字符('d\n')仍然保存在内核的终端设备输入缓冲区中。
3. a.out进程打印结束并且退出，这时Shell进程恢复运行，Shell继续从终端读取用户输入的命令，于是读走了终端设备输入缓冲区中剩下的字符('d\n'),把它当成一条命令解释执行，结果发现执行不了，没有d这个命令。

#### 非阻塞(Non Blocking)

在使用`open`打开一个设备时指定`O_NONBLOCK`标志，`read/write`就不会阻塞。以read为例，如果设备暂时没有数据可读就返回-1，同时置errno为EWOULDBLOCK（或者EAGAIN，这两个宏定义的值相同），表示本来应该阻塞在这里（would block，虚拟语气）。事实上并没有阻塞而是直接返回错误，调用者应该试着再读一次（again）。这种行为方式称为轮询（Poll），调用者只是查询一下，而不是阻塞在这里死等，这样可以同时监视多个设备:

```c
while(1) {
    非阻塞read(设备1);
    if(设备1有数据到达)
   			处理数据;
    
  	非阻塞read(设备2);
    if(设备2有数据到达)
    		处理数据;
    ...
}
```

如果read(设备1)是阻塞的，那么只要设备1没有数据到达就会一直阻塞在设备1的read调用上，即使设备2有数据到达也不能处理，使用非阻塞I/O就可以避免设备2得不到及时处理。

非阻塞I/O有一个缺点，如果所有设备都一直没有数据到达，调用者需要反复查询做无用功，如果阻塞在那里，操作系统可以调度别的进程执行，就不会做无用功了。在使用非阻塞I/O时，通常不会在一个while循环中一直不停地查询（这称为Tight Loop），而是每延迟等待一会儿来查询一下，以免做太多无用功，在延迟等待的时候可以调度其它进程执行。

```c
while(1) {
    非阻塞read(设备1);
    if(设备1有数据到达)
    		处理数据;
    
  	非阻塞read(设备2);
    if(设备2有数据到达)
    		处理数据;
    ...
    sleep(n);
}
```

这样做的问题是，设备1有数据到达时可能不能及时处理，最长需延迟n秒才能处理，而且反复查询还是做了很多无用功。以后要学习的`select(2)`函数可以阻塞地同时监视多个设备，还可以设定阻塞等待的超时时间，从而圆满地解决了这个问题。

以下是一个非阻塞I/O的例子。目前我们学过的可能引起阻塞的设备只有终端，所以我们用终端来做这个实验。

程序开始执行时在0、1、2文件描述符上自动打开的文件就是终端，但是没有O_NONBLOCK标志。读标准输入是阻塞的。我们可以重新打开一遍设备文件/dev/tty（表示当前终端），在打开时指定O_NONBLOCK标志。

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

// 重试读取提示信息
#define MSG_TRY "try again read\n"

int main(void) {
    char buf[10]; // 定义缓冲区
    int n;        // 读取到的字节数量
    int stdfd;       // 重新打开的终端设备描述符
    // 打开终端设备，以非阻塞模式
    stdfd = open("/dev/tty",O_RDONLY | O_NONBLOCK);
    if(stdfd == -1){
        perror("open terminal failed: ");
        exit(1);
    }
    // 以非阻塞模式循环读取标准输入
    loop_read:
    n = read(stdfd,buf, sizeof(buf));
    if (n == -1){
        if(errno == EAGAIN){// 表示目前没有数据可读
            sleep(1); // 睡眠1秒
            write(STDOUT_FILENO,MSG_TRY, sizeof(MSG_TRY));
            goto loop_read; // 跳转回去继续尝试读取
        }
        perror("read stdin failed:");
        exit(1);
    }
    // 写回到标准输出
    write(STDOUT_FILENO,buf,n);
    close(stdfd);
    return 0;
}
```

执行结果:

```bash
$ ./a.out
try again read
try again read
try again read
try again read
try again read
try again read
try again read
try again read
try again read
...
```

程序并不会阻塞，而是会不断的输出`try again read`，直到往终端中输入数据;

### 6. lseek 系统调用

> `lseek`系统调用函数可以移动文件的读写位置(或者叫偏移量)。

每个打开的文件都记录着当前读写位置，打开文件时读写位置是0，表示文件开头，通常读写多少个字节就会将读写位置往后移多少个字节。但是有一个例外，如果以O_APPEND方式打开，每次写操作都会在文件末尾追加数据，然后将读写位置移到新的文件末尾。lseek和标准I/O库的fseek函数类似，可以移动当前读写位置（或者叫偏移量）。

```c
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
返回值为新的偏移量
```

参数offset和whence的含义和fseek函数完全相同。只不过第一个参数换成了文件描述符。和fseek一样，偏移量允许超过文件末尾，这种情况下对该文件的下一次写操作将延长文件，中间空洞的部分读出来都是0。

若lseek成功执行，则返回新的偏移量，因此可用以下方法确定一个打开文件的当前偏移量：

```c
off_t currpos;
currpos = lseek(fd, 0, SEEK_CUR);
```

这种方法也可用来确定文件或设备是否可以设置偏移量，常规文件都可以设置偏移量，而设备一般是不可以设置偏移量的。如果设备不支持lseek，则lseek返回-1，并将errno设置为ESPIPE。注意fseek和lseek在返回值上有细微的差别，fseek成功时返回0失败时返回-1，要返回当前偏移量需调用ftell，而lseek成功时返回当前偏移量失败时返回-1。

### 7. fcntl 系统调用

在刚才的非阻塞IO案例中，我们为什么不直接对**标准输入(STDIN_FILENO)**做非阻塞`read`操作呢？而是要从新`open`打开`/dev/tty`呢？

因为STDIN_FILENO在程序启动时已经被自动打开了，而我们需要在调用open时指定O_NONBLOCK标志。这里介绍另外一种办法，可以用fcntl函数改变一个已打开的文件的属性，可以重新设置读、写、追加、非阻塞等标志（这些标志称为File StatusFlag），而不必重新open文件。

```c
#include <unistd.h>
#include <fcntl.h>

int fcntl(int fd, int cmd);
int fcntl(int fd, int cmd, long arg);
int fcntl(int fd, int cmd, struct flock* lock);
```

`fd`: 要操作的文件描述符;

`cmd`: 要执行的操作，有如下选项:

- F_DUPFD: 创建一个新的文件描述符，同样指向`fd`所指向的文件;
- F_GETFD: 获取`fd`的文件描述符标记;
- F_SETFD: 对`fd`设置文件描述符标记;
- F_GETFL: 获取`fd`的文件状态标记;
- F_SETFL: 对`fd`设置文件状态;

`arg`: 可选参数，由`cmd`参数所决定;

如下案例： 使用`fcntl`将标准输入更改为非阻塞模式

```c
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#define MSG_TRY "try again read\n"

/**
 * 使用fcntl 系统调用，动态将标准输入更改为 非阻塞模式
 */
int main(void) {
    char buf[10]; // 定义缓冲区
    int n;        // 读取到的字节数量

    int flags;
    // 1. 获取标准输入的状态标记
    flags = fcntl(STDIN_FILENO,F_GETFL);
    // 2. 将状态标记 追加上 O_NONBLOCK
    flags |= O_NONBLOCK;
    // 3. 将新的状态值，更新回标准输入
    int res = fcntl(STDIN_FILENO,F_SETFL,flags);
    if(res == -1){
        perror("Modify stdin flags:");
        exit(1);
    }
    loop:
    n = read(STDIN_FILENO,buf, sizeof(buf));
    if (n == -1){
        if (errno == EAGAIN){
            write(STDOUT_FILENO,MSG_TRY, sizeof(MSG_TRY));
            sleep(1);
            goto loop;
        }
        perror("read stdin failed:");
        exit(1);
    }
    return 0;
}
```

### 8. ioctl 系统调用

ioctl用于向设备发控制和配置命令，有些命令也需要读写一些数据，但这些数据是不能用read/write读写的，称为Out-of-band数据。也就是说，read/write读写的数据是in-band数据，是I/O操作的主体，而ioctl命令传送的是控制信息，其中的数据是辅助的数据。例如，在串口线上收发数据通过read/write操作，而串口的波特率、校验位、停止位通过ioctl设置，A/D转换的结果通过read读取，而A/D转换的精度和工作频率通过ioctl设置。

```c
#include <sys/ioctl.h>

int ioctl(int fd, int request, ...);
```

`fd`: 某个设备的文件描述符;

`request`: 对射陪要发送的控制命令;

`...`: 可变参数，通常是一个指向变量或结构体的指针;

若出错则返回-1，若成功则返回其他值，返回值也是取决于request。

以下程序使用TIOCGWINSZ命令获得终端设备的窗口大小:

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[]) {
    // 终端窗口 大小信息结构体
    struct winsize windowSize;

    // 是否连接到终端
    if (isatty(STDOUT_FILENO) == 0){
        exit(1);
    }

    // 通过ioctl函数 对终端设备发送命令
    // 发送 TIOCGWINSZ 命令，获取终端窗口的大小信息
    if (ioctl(STDOUT_FILENO,TIOCGWINSZ,&windowSize) < 0){
        perror("ioctl TIOCGWINSZ error:");
        exit(1);
    }
    printf("高度: %d, 宽度: %d \n",windowSize.ws_row, windowSize.ws_col);
    return 0;
}

```

在图形界面的终端里多次改变终端窗口的大小并运行该程序，观察结果。

### 9. mmap 系统调用(重要)

> `mmap`全称**memory map(内存映射)**，简而言之就是将**内核空间**的一段内存区域映射到到**用户空间**的一段内存区域。映射成功后这两块内存区域将会产生关联，无论修改哪一块内存，都会反映到对方。对于内核空间与用户空间两者之间需要大量数据传输等操作的话效率是非常高的。当然，也可以将内核空间的一段内存区域同时映射到多个进程，这样还可以实现进程间的共享内存通信。

最常见的内存映射就是文件操作，将文件映射至内存(进程空间)，这样就可以将对文件的操作，转换为对内存的操作，一次避免更多的`lseek`、`read`、`write`调用，这点对于大文件或者频繁访问的文件而言尤其受益。

```c
#include <sys/mman.h>

void* mmap(void* addr, size_t len, int prot, int flag, int filedes, off_t offset);
```

- `addr`:

  如果`addr`参数为`NULL`，内核会自动在进程内存空间选择一块合适的内存建立映射。如果`addr`不为`NULL`，表示给内核一个提示，内核从`addr`地址之上的区域选择一块合适的内存建立映射。通常我们都是传`NULL`，让内核自己选择内存地址;

- `offset`:

  需要映射的内容在内核空间中偏移量，也就是从文件内容的什么位置开始映射。必须是`PAGE_SIZE`(在32位体系统结构上通常是4K)的整数倍;

- `len`:

  需要映射的那一段内容的长度大小。

- `filedes`:

  要映射的文件描述符，由`open`返回的值。

- `prot`:

  该参数用于指定映射区域的保护权限，它是一个掩码，可以通过位运算来设置多个权限，取值有如下几种:

  - PROT_EXEC: 表示这段映射区域可以被执行，例如映射动态库;
  - PROT_READ: 表示这段映射区域可以读取;
  - PROT_WRITE: 表示这段映射区域可以写入;
  - PROT_NONE: 表示这段映射区域不可访问;

- `flag`:

  该参数用于指定一些特性和行为，同样可以通过位运算来设置多个标志，常用的取值有:

  - MAP_SHARED: 多个进程对同一个文件的映射是共享的，一个进程对映射的内存做了修改，另一个进程也会看到这种变化。
  - MAP_PRIVATE: 多个进程对同一个文件的映射不是共享的，一个进程对映射的内存做了修改，另一个进程并不会看到这种变化，也不会真的写到文件中去。
  - MAP_ANONYMOUS: 创建一个匿名映射区域，不与任何文件关联。用于匿名内存的分配。

当映射建立完成后，会将映射的内存起始地址作为返回值返回。 

映射建立后，使用完毕后需要通过`munmap`来关闭内存映射，以此释放内存;

```c
#include <sys/mman.h>

int munmap(void* addr, size_t len);
```

- `addr`: 要关闭的内存映射起始地址，也就是`mmap`的返回值;
- `len`: 要关闭映射的内存长度;

#### mmap文件操作

下面通过一个案例，实现对文件内容的内存映射、读取、写入以及关闭等操作;

创建一个文本`hello.txt`

```tex
Hello,world
```

编写案例程序:

```c
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    // 打开一个文件
    int file_fd = open("hello.txt",O_RDWR);
    if(file_fd < 0){
        perror("open file");
        exit(1);
    }
    // 要映射的内存长度，只映射5个字节
    int len = 5;
    // 起始偏移量，0表示从文件内容的头部开始
    int offset = 0;

    // 通过mmap 对打开的文件建立内存映射
  	// 以可读可写权限,并且进程共享 建立映射
    char* content = mmap(NULL,len,PROT_READ | PROT_WRITE,MAP_SHARED, file_fd, offset);
    if (content == MAP_FAILED){
        // 内存映射失败
        perror("mmap failed");
        exit(1);
    }
    // 获取到了映射内容的起始内存地址，接下来就可以像操作内存一样来修改内容

    // 1. 读取映射到的内容
    char buf[len + 1];
    strncpy(buf,content,len);
    buf[len] = '\0';
    printf("%s\n",buf); // Hello

    // 2. 修改映射的内容
    strncpy(content,"HELLO",len);

    // 3. 再尝试读取映射内容
    printf("%s\n", strncpy(buf,content,len)); // HELLO

    // 4. 取消映射
    munmap(content,len);
    return 0;
}
```

接下来编译执行:

```bash
$ gcc main.c
$ ./a.out
Hello
HELLO
```



## 进程

### 1. 引言

我们知道，每个进程在内核中都有一个进程控制块（PCB）来维护进程相关的信息，Linux内核的进程控制块是`task_struct`结构体。

现在我们全面了解一下其中都有哪些信息:

- 进程id。系统中每个进程有唯一的id，在C语言中用pid_t类型表示，其实就是一个非负整数。
- 进程的状态，有运行、挂起、停止、僵尸等状态。
- 进程切换时需要保存和恢复的一些CPU寄存器。
- 描述虚拟地址空间的信息。
- 描述控制终端的信息。
- 当前工作目录(Current Working Dirctory)。
- umask掩码。
- 文件描述表，包含很多指向`file`结构体的指针。
- 和信号相关的信息。
- 用户id和用户组id。
- 控制终端、Session和进程组。
- 进程可以使用的资源上限(Resource Limit)。

这些东西稍后会进行详细一一介绍。

`fork`和`exec`是本章要介绍的两个重要的系统调用。`fork`的作用是根据一个现有的进程复制出一个新的进程，原来的进程称之为父进程(Parent Process)，新进程称为子进程(Child Process)。系统中同时运行着很多进程，这些进程都是从最初只有一个进程开始一个一个复制出来的。在Shell下输入命令可以运行一个程序，是因为Shell进程在读取用户输入的命令之后会调用fork复制出一个新的Shell进程，然后新的Shell进程调用exec执行新的程序。



例如: 我们打开一个终端程序`/bin/bash`，然后在终端执行 `ls` 可执行程序:

```bash
$ ls
...
```

这个如此简单的操作，会产生如下流程:

1. 首先`fork`创建一个子进程;
2. 这时的子进程仍然在执行`/bin/bash`程序，然后子进程调用`exec`执行新的程序`/bin/ls`;



在`open/close`中我们做过一个实验：用umask命令设置Shell进程的umask掩码，然后运行程序a.out，结果a.out进程的umask掩码也和Shell进程一样。现在可以解释了，因为a.out进程是Shell进程的子进程，子进程的PCB是根据父进程复制而来的，所以其中的umask掩码也和父进程一样。同样道理，子进程的当前工作目录也和父进程一样，所以我们可以用cd命令改变Shell进程的当前目录，然后用ls命令列出那个目录下的文件，ls进程其实是在列自己的当前目录，而不是Shell进程的当前目录，只不过ls进程的当前目录正好和Shell进程相同。

### 2. 环境变量

**环境变量(environment variables)**是操作系统提供的一种机制，用于在操作系统级别存储配置信息和对程序的影响。这些变量包含了影响程序运行行为的各种设置，比如搜索路径、临时文件位置、默认编辑器、语言设置等等。

进程的**环境变量**是指当前进程在运行时可用的特定环境信息，它的可用范围是针对当前进程的。每个进程都有自己的环境变量，当进程被创建时，它会继承父进程的环境变量信息，后续也可以修改、添加或删除自己的环境变量。例如通过`exec`系统调用执行新程序时会把**命令参数**和**环境变量表**传递给main函数。

在libc中定义的全局变量`environ`指针，指向环境变量表，它没有包含在任何头文件中，所以我们可以直接通过该变量来获取环境变量，例如:

```c
int main(void) {
    // 声明一个字符串指针变量，注意名称必须为`environ`
    // 因为该变量由 libc 库中所定义和提供
    extern char** environ;

    // 变量输出环境变量
    for (int i = 0; environ[i] != NULL; i++) {
        printf("%s\n",environ[i]);
    }
    return 0;
}
```

执行结果:

```bash
$ ./a.out
TERM=xterm-256color
SHELL=/bin/zsh
PAGER=less
...
```

由于父进程在调用fork创建子进程时会把自己的环境变量表也复制给子进程，所以a.out打印的环境变量和Shell进程的环境变量是相同的。

按照惯例，环境变量字符串都是name=value这样的形式，大多数name由大写字母加下划线组成，一般把name的部分叫做环境变量，value的部分则是环境变量的值。环境变量定义了进程的运行环境。

#### 查看环境变量

用environ指针可以查看所有环境变量字符串，但是不够方便，如果给出name要在环境变量表中
查找它对应的value，可以用getenv函数:

```c
#include <stdlib.c>

char* getenv(const chart* name);
```

getenv的返回值是指向value的指针，若未找到则为NULL。

```c
int main(int argc, char *argv[]) {
    const char* javaHome = getenv("HOME");
    printf("%s \n",javaHome); // /Users/zero

    const char* custom = getenv("CUSTOM");
    printf("%s \n",custom); // (null)
    return 0;
}
```

#### 设置环境变量

修改变量可以用如下函数:

```c
#include <stdlib.h>

int setenv(const char *name, const char *value, int rewrite);
int	 putenv(char* name)
```

putenv和setenv函数若成功则返回为0，若出错则返回非0。

setenv函数较为特殊，有如下规则:

- 若rewrite非0，则覆盖原来的定义；
- 若rewrite为0，则不覆盖原来的定义，也不返回错误。

```c
#include <stdio.h>
#include <stdlib.h>
#define KEY "ROOT_NAME"

int main() {
    int no = setenv(KEY,"张三",1);
    if (no != 0){
        perror("set env failed");
        exit(no);
    }
    const char* name = getenv(KEY);
    printf("%s",name); // 张三
    return 0;
}
```

#### 删除环境变量

unsetenv删除name的定义。即使name没有定义也不返回错误。

```c
#include <stdio.h>
#include <stdlib.h>

#define KEY "ROOT_NAME"

int main() {
    // 设置环境变量
    int no = setenv(KEY,"张三",1);
    if (no != 0){
        perror("set env failed");
        exit(no);
    }
    // 获取环境变量
    const char* name = getenv(KEY);
    printf("%s\n",name); // 张三
    // 删除环境变量
    no = unsetenv(KEY);
    if (no != 0){
        perror("remove env failed");
        exit(no);
    }
    printf("%s\n", getenv(KEY)); // (null)
    return 0;
}
```

注意： 这些操作都是针对于当前进程的环境变量，并不会影响父进程的环境变量。

父进程在创建子进程时会复制一份环境变量给子进程，但此后二者的环境变量互不影响。

### 3. 进程的控制

#### fork 系统调用

`fork`系统调用负责根据当前现有的进程，复制出来一个新的子进程。系统中同时运行着很多进程，这些进程都是从进程ID为1的进程，一个又一个复刻出来的子进程。

通过`fork`创建的子进程几乎和原来的进程完全相同，例如状态、寄存器、虚拟内存空间、umask、文件描述符表等等信息完全相同，只有一些细微的差别，例如进程id不一致，子进程会拥有自己的进程id;

```c
#include <sys/types.h>
#include <unistd.h>

pid_t fork(void);
```

使用`pid_t`类型作为进程的id，也就是该函数的返回值，返回值有如下情况:

- 如果返回的是-1，表示创建进程失败，可能是内存不足，也可能是进程数量到达系统上限;
- 如果返回值为0或其他正数，则表示创建进程成功，此时会产生两个视角，分别是父进程和子进程:
  - 返回值为0，表示为当前进程为子进程;
  - 返回值大于0，表示当前进程为父进，该值是子进程的id;

简单使用案例如下：

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {
    // 创建子进程
    pid_t cPid = fork();
    // cPid为-1，表示创建进程失败
    if (cPid == -1){
        perror("fork process failed");
        exit(1);
    }
    // 创建进程成功
    if (cPid > 0){
        // 父进程后续操作
        printf("进程创建完成，我是父进程.\n");
    } else if (cPid == 0){
        // 子进程后续操作
        printf("进程创建完成，我是子进程.\n");
    }
    return 0;
}
```

执行结果：

```bash
进程创建完成，我是父进程.
进程创建完成，我是子进程.
```



接下来这个案例使用`fork`来创建子进程，并且不同进程输出不同次数的消息内容:

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {
    char* message;
    int n;

    // 创建子进程
    pid_t pid = fork();
    if (pid == -1){
        perror("fork process failed");
        exit(1);
    }
    if (pid == 0){
        // 子进程
        message = "我是子进程~\n";
        n = 6;
    }else{
        // 主进程
        message = "我是主进程~\n";
        n = 3;
    }
    while (n > 0){
        printf("%s",message);
        sleep(1);
        n--;
    }
    return 0;
}

```

执行结果:

```bash
$ ./a.out
我是主进程~
我是子进程~
我是主进程~
我是子进程~
我是主进程~
我是子进程~
我是子进程~
$ 我是子进程~
我是子进程~
```

该案例的执行流程如下:

1. 父进程初始化。
2. 父进程调用fork，这是一个系统调用，因此进入内核。
3. 内核根据父进程复制出一个子进程，父进程和子进程的PCB信息相同，用户态代码和数据也相同。因此，子进程现在的状态看起来和父进程一样，做完了初始化，刚调用了fork进入内核，还没有从内核返回。
4. 现在有两个一模一样的进程看起来都调用了fork进入内核等待从内核返回（实际上fork只调用了一次），此外系统中还有很多别的进程也等待从内核返回。是父进程先返回还是子进程先返回，还是这两个进程都等待，先去调度执行别的进程，这都不一定，取决于内核的调度算法。
5. 如果某个时刻父进程被调度执行了，从内核返回后就从fork函数返回，保存在变量pid中的返回值是子进程的id，是一个大于0的整数，因此执下面的else分支，然后执行for循环，打印"我是主进程~\n"三次之后终止。
6. 如果某个时刻子进程被调度执行了，从内核返回后就从fork函数返回，保存在变量pid中的返回值是0，因此执行下面的if (pid == 0)分支，然后执行for循环，打印"我是子进程~\n"六次之后终止。fork调用把父进程的数据复制一份给子进程，但此后二者互不影响，在这个例子中，fork调用之后父进程和子进程的变量message和n被赋予不同的值，互不影响。
7. 父进程每打印一条消息就睡眠1秒，这时内核调度别的进程执行，在1秒这么长的间隙里（对于计算机来说1秒很长了）子进程很有可能被调度到。同样地，子进程每打印一条消息就睡眠1秒，在这1秒期间父进程也很有可能被调度到。所以程序运行的结果基本上是父子进程交替打印，但这也不是一定的，取决于系统中其它进程的运行情况和内核的调度算法，如果系统中其它进程非常繁忙则有可能观察到不同的结果。另外，读者也可以把sleep(1);去掉看程序的运行结果如何。
8. 这个程序是在Shell下运行的，因此Shell进程是父进程的父进程。父进程运行时Shell进程处于等待状态，当父进程终止时Shell进程认为命令执行结束了，于是打印Shell提示符，而事实上子进程这时还没结束，所以子进程的消息打印到了Shell提示符后面。最后光标停在This is the child的下一行，这时用户仍然可以敲命令，即使命令不是紧跟在提示符后面，Shell也能正确读取。

fork函数的特点概括起来就是“调用一次，返回两次”，在父进程中调用一次，在父进程和子进程中各返回一次。从上图可以看出，一开始是一个控制流程，调用fork之后发生了分叉，变成两个控制流程，这也就是“fork”（分叉）这个名字的由来了。子进程中fork的返回值是0，而父进程中fork的返回值则是子进程的id（从根本上说fork是从内核返回的，内核自有办法让父进程和子进程返回不同的值），这样当fork函数返回后，程序员可以根据返回值的不同让父进程和子进程执行不同的代码。

fork的返回值这样规定是有道理的。fork在子进程中返回0，子进程仍可以调用getpid函数得到自己的进程id，也可以调用getppid函数得到父进程的id。在父进程中用getpid可以得到自己的进程id，然而要想得到子进程的id，只有将fork的返回值记录下来，别无它法。

fork的另一个特性是所有由父进程打开的描述符都被复制到子进程中。父、子进程中相同编号的文件描述符在内核中指向同一个file结构体，也就是说，file结构体的引用计数要增加。

#### 孤儿进程

当一个父进程退出后，由该进程所创建出的所有子进程还在运行时，这些子进程就被称之为**孤儿进程**。孤儿进程将会被**init进程(pid为1的进程)**所管理，并且由init进程对它们完成状态收集工作。简单点说孤儿进程就是没有父进程的进程，然后就会认init进程作为父进程，完成孤儿进程的善后工作。
如下案例所示:

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * 简单点说孤儿进程就是没有父进程的进程，然后就会认init进程作为父进程，完成孤儿进程的善后工作。
 * 通过如下案例，来验证父进程结束后，孤儿进程的变化.
 */

int main(void) {
    int n;
    pid_t pid;
    if((pid = fork()) == -1){
        perror("fork process failed.");
        exit(1);
    }
    if (pid == 0){
        // 子进程
        n = 6;
        while (n > 0){
            printf("我是子进程，我的id为: %d，我的父进程id为: %d \n", getpid(),getppid());
            sleep(1);
            n--;
        }
    }else{
        // 父进程
        n = 3;
        while (n > 0){
            printf("我是父进程，我的id为: %d，我的父进程id为: %d, 我的子进程id为: %d \n",getpid(),getppid(),pid);
            sleep(1);
            n--;
        }
    }
    return 0;
}
```

执行结果:

```bash
$ ./a.out
我是父进程，我的id为: 29173，我的父进程id为: 28151, 我的子进程id为: 29174 
我是子进程，我的id为: 29174，我的父进程id为: 29173 
我是父进程，我的id为: 29173，我的父进程id为: 28151, 我的子进程id为: 29174 
我是子进程，我的id为: 29174，我的父进程id为: 29173 
我是父进程，我的id为: 29173，我的父进程id为: 28151, 我的子进程id为: 29174 
我是子进程，我的id为: 29174，我的父进程id为: 29173 
我是子进程，我的id为: 29174，我的父进程id为: 29173 
$ 我是子进程，我的id为: 29174，我的父进程id为: 1
我是子进程，我的id为: 29174，我的父进程id为: 1 
```

通过输出我们看到，程序刚运行时:

- 父进程id: 29173
- 子进程id: 29174
- bash进程id: 28151

3秒后，父进程输出完毕，父进程死亡:

- 子进程id: 29174
- 子进程的父进程id: 1

可以很明显的看到，在程序中通过`fork`创建出的子进程，在父进程死亡后，它的父进程id变为了 1，成为了**init进程**的子进程。

#### exec 系统调用

刚刚学到的`fork`用于创建进程，问题是创建好的进程是和父进程相同的程序，但是我们往往要使用子进程来执行其他程序，否则创建多进程的意义就不存在了。这个时候就可以通过`exec`系列函数来执行另一个程序。

当一个进程调用`exec`系列函数时，当前进程的用户空间代码和数据会完全被新程序替换掉，然后从新程序的启动例程开始执行。调用exec并不创建新进程，所以调用exec前后该进程的id并未改变。

`exec`并不是一个确定的系统函数，而是一堆以`exec`开头的函数，所以统称为`exec`函数。共有以下6种相关函数:

```c
#include <unistd.h>

l 系列函数
int execl(const char* path, const char* arg, ...);
int execlp(const char* file, const char* arg, ...);
int execle(const char *path, const char *arg, ..., char *const envp[]);

v 系列函数
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execve(const char *path, char *const argv[], char *const envp[]);

file | path: 要执行的程序
arg  | argv: 程序所需的命令行参数
envp: 要传递给新程序的环境变量
```

啊...头大有木有? 放眼望去一堆相似的函数，下面我们就针对这些函数的区别与功能进行详解。其实这些函数的功能是一致的，都是让当前进程执行另一个程序，只是由于参数传递方式不同，被拆分成多个函数。只有`execve`函数是真正的系统调用函数，其他函数都是针对于该`execve`的封装。

**这些函数整体分为两个系列:**

- 带有`l`字母系列(表示List):

  该系列函数要求将新程序的每个命令行参数都作为一个个的字符串传给它，命令行参数的个数是可变的，所以该系列函数的`arg`都是`...`，并且要求可变参数的最后一个参数必须为`NULL`,表示参数结束，起到哨兵的作用。如下使用`execl`执行新程序:

  ```c
  # 通过execl 运行 ls 程序(无参数)
  execl("/bin/ls","ls",NULL);
  
  # 通过execl 执行 ps 程序(多参数)
  execl("/bin/ps","ps","-o","pid,ppid",NULL);
  ```

- 带`v`字母系列(表示Vector):

  该系列函数要求将新程序的所有命令行参数构建成一个字符串数组，然后将数组作为参数传递给它，并且数组的最后一个参数必须为`NULL`。就像main函数的argv参数或者环境变量表一样。如下使用`execv`执行新程序:

  ```c
  # 构建 ps 程序所需的命令行参数
  char* const args[] = {"ps","-o","pid,ppid", NULL};
  # 执行 ps 程序
  execv("/bin/ps", args);
  ```

- 不以`p`为后缀的函数

  p表示path，函数不是以p结尾的，第一个参数必须是要执行的程序的**相对路径**或**绝对路径**，例如要执行`ls`程序，那么第一个参数必须为`ls`程序的所在路径，如下:

  ```c
  # 执行 ls 程序
  execl("/bin/ls", "ls", NULL);
  
  # 执行 ls 程序，以数组形式传参
  char* const args[] = {"ls", NULL};
  execv("/bin/ls",args);
  ```

- 以`p`为后缀的函数

  如果参数中包含/，则将其视为路径名，否则视为不带路径的程序名，在PATH环境变量的目录列表中搜索这个程序。

  ```c
  # 执行ls 程序，直接指定程序名称，通过path环境变量查找
  execlp("ls","ls",NULL);
  
  # 执行 ls 程序，以数组形式传参
  char* const args[] = {"ls", NULL};
  execvp("ls", args);
  ```

- 以`e`为后缀的函数

  e表示environment，表示将一份新的环境变量表传递给它，其他exec函数仍使用当前的环境变量表执行新程序。

  ```c
  # 创建一份新的环境变量
  char* const envs[] = {"PATH=/bin:/usr/bin","NAME=zhangsan",NULL};
  # 执行 ls 程序，并且将自定义的环境变量传递给它
  execle("/bin/ls","ls",NULL,envs);
  ```

由于exec函数只有错误返回值，只要返回了一定是出错了，所以不需要判断它的返回值，直接在后面调用perror即可。注意在调用execlp时传了两个"ps"参数，第一个"ps"是程序名，execlp函数要在PATH环境变量中找到这个程序并执行它，而第二个"ps"是第一个命令行参数，execlp函数并不关心它的值，只是简单地把它传给ps程序，ps程序可以通过main函数的argv[0]取到这个参数。

#### wait 进程同步

一个进程在终止时，会关闭进程内的所有的文件描述符，并且释放在用户空间分配的内存，但它的**进程控制块(PCB)**还保留着，内核在PCB中保留了一些信息: 如果该进程是正常终止，则保存着退出状态，如果是异常终止，则保留着导致异常终止的**信号**。已经终止的进程的父进程可以调用`wait`和`waitpid`获取这些保留信息，然后彻底清除掉这个进程。

我们知道在终端Shell中可以通过`$?`特殊变量来查看上个进程的退出状态， 因为Shell是它的父进程。当一个进程终止时，Shell会调用`wait`或`waitpid`得到它的退出状态同时清除掉这个进程。

如果一个进程已经终止，但是它的父进程尚未调用`wait`或`waitpid`对它进行清理时，这时的进程状态被称为**僵尸进程(Zombie)**。任何进程终止后都是僵尸进程。

正常情况下僵尸进程都立刻会被父进程清理掉，为了验证刚刚所说的，我们编写一个非正常程序：父进程fork出子进程，然后让子进程终止，而父进程既不终止，也不调用`wait`清理子进程:

```c
int main(void) {
    pid_t pid = fork();
    if(pid > 0){
       //父进程死循环
        while (1);
    }
  // 子进程直接终止
    return 0;
}
```

编译程序，然后后台运行该程序，通过ps查看进程信息:

```bash
$ ./a.out &
[1] 20894

$ ps u
USER   	PID  	%CPU 	%MEM   VSZ    RSS TT  		STAT 	STARTED TIME COMMAND
...
root     20894 99.0  0.0   4212   352 pts/0    RN   14:27   0:13 ./a.out
root     20895  0.0  0.0      0     0 pts/0    ZN   14:27   0:00 [a.out] <defunct>
```

`&` 表示后台运行该程序，Shell不会阻塞等待这个进程终止，并且后台进程是读不到终端输入的。

通过`ps` 程序可以看到 父进程的id为 20894，状态为`RN`表示为正在运行，而子进程的id为20895，并且状态为`ZN`表示为僵尸状态。

如果一个父进程终止，而它的子进程还存在（这些子进程或者仍在运行，或者已经是僵尸进程了），则这些子进程的父进程改为**init进程**。init是系统中的一个特殊进程，通常程序文件是/sbin/init，进程id是1，在系统启动时负责启动各种系统服务，之后就负责清理子进程，只要有子进程终止，init就会调用wait函数清理它。

> 注意: 僵尸进程是不能用kill命令清除掉的，因为kill命令只是用来终止进程的，而僵尸进程已经终止了。

介绍了这么多，下面正式开始介绍`wait`和`waitpid`函数:

```c
#include <sys/types.h>
#include <sys/wait.h>

pid_t wait(int* status);

pid_t waitpid(pid_t pid, int* status, int options);
# pid 为要等待的子进程ID
# - 如果 pid > 0，则等待具有指定PID的子进程。
# - 如果 pid = -1，则等待任意子进程。相当于wait
# - 如果 pid = 0，则等待与调用 waitpid 的进程属于同一进程组的任意子进程。
# - 则等待同一进程组中的任意子进程，其进程组ID等于 pid 的绝对值。

# options 行为选项
# 0 表示没有任何选项
# WNOHANG：如果没有任何已经结束的子进程则马上返回0, 不予以等待；
# WUNTRACED：如果子进程进入暂停执行情况则马上返回, 但结束状态不予以理会. 子进程的结束状态返回后存于status, 底下有几个宏可判别结束情况；
```

执行成功则返回被清理掉的子进程的id，并且将进程的退出状态设置给`status`参数。如果执行错误则返回-1，并且设置errno;

父进程这两个函数时，可能会产生如下几种情况:

- 如果当前进程没有任何子进程，则立刻返回-1。
- 如果当前进程的所有子进程都还在运行，也就是说没有处于僵尸状态的进程，那么就会产生阻塞，直到有一个子进程终止后，父进程会读取该终止信息，并且返回。

这两个函数的区别:

- `wait`会使父进程进入阻塞状态，直到有任意一个子进程终止后，获取其终止信息然后返回结束;
- `waitpid`可以通过指定的进程id，来决定阻塞等待哪个子进程终止，并且还可以在options参数中指定WNOHANG可以使父进程不阻塞而立即返回0。

可见，调用wait和waitpid不仅可以获得子进程的终止信息，还可以使父进程阻塞等待子进程终止，起到进程间同步的作用。如果参数status不是空指针，则子进程的终止信息通过这个参数传出，如果只是为了同步而不关心子进程的终止信息，可以将status参数指定为NULL。

子进程的终止信息在一个`int status`变量中，其中包含了多个字段，提供了许多宏定义可以取出其中的每个字段:

- `WIFEXITED`: 判断进程是否为正常终止，例如通过`exit`终止，如果是结果则为真;
- `WEXITSTATUS`: 取出status中的终止码，也就是子进程中通过`exit`指定的状态值;
- `WIFSIGNALED`: 判断进程是否是收到信号而异常终止的，如果是取出的结果则为真;
- `WTERMSIG`: 取出终止信号的编号;
- `WIFSTOPPED`: 判断子进程是否处于暂停状态，取出的值为真;
- `WSTOPSIG`: 取出暂停信号的编号;

如下案例:

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    // 创建子进程
    pid_t  pid = fork();
    if(pid == 0){
      	// 子进程 正常退出，设置值为3
        exit(3);
    }
    int status;
    // 父进程等待子进程结束
    waitpid(pid,&status,0);
    printf("子进程执行结束.\n");

    if (WIFEXITED(status)){// 判断程序是否正常结束，通过exit终止(无论值是多少)，如果是则返回非0
        // 获取exit的状态值
        int code = WEXITSTATUS(status);
        printf("子进程正常退出，状态码为: %d \n",code);
    }else if(WIFSIGNALED(status)){// 如果子进程是因为信号而结束则此宏值为真
        // 获取终止的信号
        int sig = WTERMSIG(status);
        printf("子进程因信号退出，信号为: %d \n",sig);
    }
    return 0;
}
```

执行结果：

```bash
$ ./a.out
子进程执行结束.
子进程正常退出，状态码为: 3 
```

### 4. 进程间通信

每个进程各自拥有自己的**虚拟地址空间**，而虚拟地址空间会被分为两大部分，分别是**内核空间**和**用户空间**。用户空间是每个进程独立拥有的，只有进程自己可以访问，而内核空间则是所有进程共享的。进程的**数据段**和**代码段**相关部分归属于用户空间，所以一个进程中的数据在另一个进程中是不可见的，如果进程之间如果想实现**数据共享**必须通过内核空间: 

1. 在内核空间开辟一块缓冲区;
2. 进程1将数据从自己的用户空间写入到内核空间;
3. 进程2从内核空间将数据读取到自己的用户空间;

内核提供的这种机制称之为**进程间通信(IPC InterProcess Communication)**。

#### Pipe

**Pipe(管道)**是一种最基本的进程通信机制,其原理为在**内核空间**开辟出一个缓冲区(称为管道)，然后进程之间就可以通过该缓冲区来进行通信。

管道由`pipe`函数创建:

```c
#include <unistd.h>

int pipe(int filedes[2]);
```

`pipe`在内核中开辟完成缓冲区后，会通过`filedes`参数传出两个文件描述符:

- `filedes[0]`表示管道的读端，可以通过`read(filedes[0])`读取管道中的数据;
- `filedes[1]`表示管道的写端，可以通过`write(filedes[1])`向管道中写入数据;

pipe函数调用成功返回0，调用失败返回-1。

使用案例如下，实现父子进程间的通信:

```c
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/**
 * 通过 管道 实现进程之间的通信
 */

#define MAX_LINE 80
#define MSG "hello world\n"

int main() {
    int pid;
    // 管道的读写端
    int chan[2];

    // 1. 创建管道
    if(pipe(chan) == -1){
        perror("create pipe failed:");
        exit(1);
    }
    // 2. 创建子进程
    if ((pid = fork()) < 0){
        perror("fork child process failed:");
        exit(1);
    }
    if (pid == 0){
        // 子进程操作
        // 关闭通道写端
        close(chan[1]);
        // 睡眠3s
        sleep(1);
        // 从通道中读取数据
        char line[MAX_LINE];
        int n = read(chan[0], line, MAX_LINE);
        // 打印到标准输出
        write(STDOUT_FILENO,line,n);
    } else{
        // 父进程操作
        // 关闭通道读端
        close(chan[0]);
        // 向通道内写入数据
        write(chan[1],MSG, sizeof(MSG));
        // 等待子进程终止
        wait(NULL);
    }
    return 0;
}
```

1. 父进程调用`pipe`开辟管道，得到两个文件描述符指向管道的两端。
2. 父进程调用`fork`创建子进程，那么子进程也有两个文件描述符指向同一管道。
3. 父进程关闭管道读端，子进程关闭管道写端。父进程可以往管道里写，子进程可以从管道里读，管道是用环形队列实现的，数据从写端流入从读端流出，这样就实现了进程间通信。

使用管道的一些限制:

- 两个进程通过一个管道只能实现单向通信，比如上面的例子，父进程写子进程读，如果有时候也需要子进程写父进程读，就必须另开一个管道。
- 需要通过`fork`来传递通道的文件描述符，使多个进程都能访问同一个管道，这样它们才能进行通信。

管道的一些特性(阻塞式管道):

- 如果管道的**写端**文件描述符全部都关闭了，而仍然有进程从**读端**读取数据，那么剩余的数据被读取完后再次读取会返回0，就像读到文件末尾一样。
- 如果管道的**读端**全都关闭了，这时有进程通过**写端**写入数据，则会收到`SIGPIPE`信号，从而导致进程异常终止。
- 如果管道的**写端**没有全部关闭时，**读端**读取数据发现没有数据可读时会**阻塞**，直到管道中有数据了再读取并且返回。
- 如果管道的**读端**没有完全关闭时，**写端**将通道写满(64k)时会发生**阻塞**，直到管道中有了空位置再写入数据并且返回。

管道的这四种特性具有普遍意义，在后续的网络编程中 TCP Socket也具有这些特性;

**popen/pclose函数**

我们到目前为止了解到了**进程**和**管道**，并且通过一些案例自己动手使用了一番，但是这样使用会比较繁琐，所以提供了如下一组函数,这一组函数自动将**创建进程**和**通道**结合在一起，使用起来更方便:

```c
#include <stdio.h>

FILE *popen(const char* command, const char* type);
# 成功返回文件指针，出错返回NULL

int pclose(FILE* stream);
# 成功返回 popen创建的进程的终止状态，出错返回-1
```

- `command`参数为要执行的命令;
- `type`则表示返回的文件指针是**标准输出**还是**标准输入**;

`popen`的工作流程:

1. 先通过`fork`创建出一个子进程;
2. 通过`pipe`创建出一个管道;
3. 如果`type`参数为`"r"`，管道的**读端**作为子进程的**标准输出**，并且返回的文件指针`FILE*`指向这个管道的读端;
4. 如果`type`参数为`"w"`，管道的**写端**作为子进程的**标准输入**，并且返回的文件指针`FILE*`指向这个管道的写端;
5. 最后子进程通过`exec`执行`command`命令;

`pclose`用于关闭`popen`打开的进程标准IO流，等待该进程执行结束，并且获取进程的终止状态。

如下案例:

1. 通过`popen`执行 `cat` 终端程序，并且读取其数据内容:

   ```c
   int main() {
       // 通过 popen 函数执行 cat 命令行程序，并且获取标准输出
       FILE* out = popen("cat hello.txt","r");
       if(out == NULL){
           perror("popen failed:");
           exit(1);
       }
   
       // 读取标准输出
       int ch;
       while ((ch = fgetc(out)) != EOF){
           putchar(ch);
       }
   
       // 关闭
       pclose(out);
       return 0;
   }
   
   ```

2. 通过`popen`执行 `cat >` 终端程序，向一个文件内部写入数据:

   ```c
   int main() {
       // 执行 cat  > hello.txt 命令
       FILE* in = popen("cat > hello.txt","w");
       if(in == NULL){
           perror("popen failed:");
           exit(1);
       }
       // 写入数据
       fputc('A',in);
       // 关闭
       pclose(in);
       return 0;
   }
   ```

#### FIFO

**FIFO(命名管道)**是Linux中提供了另一种ICP机制，它与**Pipe(无名管道)**不同，有如下特点:

- FIFO管道以文件形式存在于文件系统中，但是它不会占用物理磁盘空间;
- FIFO不需要通信的进程之间有父子关系，只要能访问FIFO管道的所在路径，就能够进行通信;

##### 1. 创建FIFO管道

在Linux中，FIFO(First In, Frist Out)管道是一种特殊类型的文件，可以通过 `mkfifo` 可执行命令创建出FIFO管道:

```bash
$ mkfifo temp
$ ll
prw-r--r--   1 root  root      0 Jan 18 20:55 temp
```

`prw-r--r--`中的`p`表示文件类型为管道。

FIFO文件在磁盘上没有数据块，仅用来标识内核中的一条通道，各个进程可以打开这个文件进行`read`或`write`，实际上是在读写内核数据;

也可以通过C语言中的函数创建:

```c
#include <sys/stat.h>

int mkfifo(const char* pathname, mode_t mode);
# 成功返回管道的文件描述符，失败返回-1;
```

##### 2. 打开FIFO管道

打开FIFO管道就像通过`open`系统调用打开一个普通文件一般，但是和普通文件有些区别:`open`打开一个普通文件不会产生阻塞，但是如果`open`打开一个FIFO管道文件则可能产生阻塞，取决于是否使用`O_NONBLOCK`标志;

阻塞模式:

- 如果以**只读**模式打开FIFO文件，那么会阻塞住，直到有另一个进程以**只写**模式打开同一个FIFO文件;
- 相反以**只写**模式打开也会阻塞，除非有另一个进程以**只读**模式打开了该文件;

非阻塞模式:

- 以**只读**模式打开，没有写进程也会成功返回，此时FIFO被读打开，而不会返回错误。
- 以**只写**模式打开，立即返回，如果此时没有其他进程以读的方式打开，open会失败打开，此时FIFO没有被打开，返回-1。

```c
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  	// 以只读方式打开管道(阻塞)
    int fifo_fd = open("temp",O_RDONLY);
    if(fifo_fd == -1){
        perror("open fifo");
        exit(1);
    }
  
    // 以只读方式打开管道(非阻塞)
    int fifo_fd = open("temp",O_RDONLY | O_NONBLOCK);
    if(fifo_fd == -1){
        perror("open fifo");
        exit(1);
    }
  
    // 以只写方式打开管道(阻塞)
    int fifo_fd = open("temp",O_WRONLY);
    if(fifo_fd == -1){
        perror("open fifo");
        exit(1);
    }
  
    
    // 以只写方式打开管道(非阻塞)
    int fifo_fd = open("temp",O_WRONLY | O_NONBLOCK);
    if(fifo_fd == -1){
        perror("open fifo");
        exit(1);
    }
}
```

##### 3. 读写FIFO管道

读取

```c
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // 打开管道
    int fifo_fd = open("temp", O_RDONLY);
    if (fifo_fd == -1){
        perror("open fifo");
        exit(1);
    }
    printf("read open fifo success.\n");
    // 读取数据
    char buf[20];
    size_t n = read(fifo_fd,buf, sizeof(buf));
    printf("read fifo success.\n");

    // 输出数据
    write(STDOUT_FILENO,buf,n-1);

    // 关闭管道
    close(fifo_fd);
    return 0;
}
```

写入

```c
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define msg "Hello World"

int main() {
    // 打开管道
    int fifo_fd = open("temp", O_WRONLY);
    if (fifo_fd == -1){
        perror("write open fifo");
        exit(1);
    }
    printf("write open fifo success.\n");

    // 写入数据
    write(fifo_fd,msg, sizeof(msg));
    printf("write fifo success.\n");

    // 关闭管道
    close(fifo_fd);
    return 0;
}
```

然后编译后同时执行两个程序:

```bash
$ ./write
write open fifo success.
write fifo success.
```

```bash
$ ./read
read open fifo success.
read fifo success.
Hello World
```

`open`函数调用中的参数标志`O_NONBLOCK`会影响FIFO的读写操作：

- 对一个空的**阻塞**的FIFO的read调用将等待，直到有数据可以读的时候才继续执行;
- 对一个空的**非阻塞**的FIFO的read调用立即返回0字节。
- 对一个**阻塞**的并且已经满了的FIFO进行write调用将会阻塞，直到数据可以被写入时才开始执行。
- 对一个**非阻塞**的并且已经满了的FIFO进行write调用，返回0，表示一个字节都没写入;

#### 共享内存

> **共享内存(Shared Memory)**指两个或多个进程共享一给定的存储区，进程可以将该段共享存储区连接到自己的用户空间中，如果某个进程向共享内存写入数据，所做的改动将立即影响到可以访问同一段共享内存的任何其他进程。

**共享内存的原理:**

进程之间也可以通过共享内存进行通信: 在物理内存上开辟一块空间，称为共享内存；不同进程将这块共享内存连接到自己的地址空间；不同进程以各自地址空间的虚拟地址通过页表找到共享内存，通过向共享内存中写数据和读数据实现进程间通信。

通过共享内存进行通信是最快的一种IPC机制，因为本质上共享内存就是多个进程之间共同使用同一块物理内存，也就是说多个进程都是直接对同一块内存进行读写操作，完全不需要经过多次的数据拷贝，所以效率非常高效。需要注意的是，在使用共享内存时，必须采取措施来确保数据的一致性和同步，以防止竞态条件等问题。通常，使用同步机制如信号量、互斥锁等来保护共享内存的访问。

**共享内存的操作步骤**

共享内存虽然非常高效，但是使用起来较为繁琐，大致有如下步骤:

1. 创建共享键值标识;

2. 创建共享内存;

3. 挂载共享内存;
4. 读写共享内存;
5. 卸载共享内存;

下面针对这些步骤进行详解。

##### 1.创建键值标识

```c
#include <sys/shm.h>

key_t ftok(const char* pathname, int proj_id);
```

`ftok`函数用于生成一个用于标识共享内存的唯一键值,当两次调用传入相同的参数时，返回值则相同;

- `pathname`: 一个已存在且拥有访问权限的文件路径(注意，只是利用了该文件的一些信息，例如时间戳、文件大小等等生成一些随机种子，然后和`proj_id`进行一些运算等等，从而生成一个键值);
- `proj_id`: 一个整型的数字标识，必须为非0;

最终返回的`key_t`可以作为创建共享内存时的标识，以及后续访问该共享内存所需要使用的，出错会返回-1，并且设置`errno`;



##### 2.创建/打开共享内存

```c
#include <sys/shm.h>

int shmget(key_t key, size_t size, int shmflg);
```

`shmget`函数用于调度内核创建或打开共享内存段，如果`key`标识的共享内存已存在则打开，返回共享内存的id，不存在则创建然后返回id;

- `key`: 共享内存的标识;
- `size`: 要申请的共享内存的大小，至少是`4k(PAGE_SIZE 一页)`，因为共享内存的分配是以页为单位，假设你申请1个字节，那么就会分配4096字节，如果你申请4097个字节，那么会分配8192字节。一定是`4096`的最小公倍数。如果是创建则必须指定该值，如果是访问那么指定为0即可;
- `shmflg`: 用于标识打开还是创建并打开共享内存，以及创建的共享内存的权限。可用多个选项进行 `|`运算，可选性有如下:
  - IPC_CREAT: 表示根据`key`创建出新的共享内存段，如果`key`标识的共享内存已存在，则不创建;
  - IPC_EXCL: 如果和IPC_CREATE一起使用，已存在的话就会出错;

成功后，将返回有效的共享内存标识符,失败返回-1并且设置`errno`。

创建共享内存案例如下:

```c
int main() {
    // 1.创建标识
    key_t key = ftok("hello.txt",9501);
    printf("create key: %d \n",key);
    // 2.创建共享内存段
  	// IPC_CREAT表示不存在则创建
  	// 0666 表示该共享内存的权限，所有用户可读可写
    int sm_id = shmget(key,20,IPC_CREAT | 0666);
    printf("create shared memory id: %d\n",sm_id);
    return 0;
}
```

执行结果:

```bash
$ ./a.out
create key: 0x1d010654
create shared memory id: 1
```

无论执行多少次，`key`都是一致的不会改变，我们还可以通过`ipcs -m`命令查看系统的共享内存信息:

```bash
$ ipcs -m

------------ Shared Memory Segments --------------
key				shmid      owner  	 perms     		bytes   nattch     status
0x1d010654 	1         root      666          20       0     
```

- `key`:使用`ftok`函数生成的键值;

- `shmid`: 共享内存的id,也就是`shmget`函数的返回值;
- `owner`: 该共享内存的所属者;
- `perms`: 该共享内存的访问权限;
- `bytes`: 该共享内存的大小;
- `nattch`: 该共享内存当前被多少进程共享;
- `status`: 状态;

##### 3.进程连接共享内存

当我们创建或者打开一个共享内存，得到其`shmid`后，就可以把该共享内存空间或进程空间连接到一起，通过`sgmat`函数:

```c
#include <sys/shm.h>

void* shmat(int shmid, const void* addr, int shmflg);
```

- `shmid`: 要挂载的共享内存段的id;
- `addr`: 通常指定为`NULL`，表示由内核自动选择进程中的一块可用的地址空间作为连接地址;
- `shmflg`: 如果`addr`设置为`NULL`，这个直接设置为0即可;

成功后返回进程内的连接起始地址，失败则会返回`(void*) -1`;

```c

int main() {
    // 1.创建标识
    key_t key = ftok("HELP.md",9501);
    printf("create key: 0x%x \n",key);
  
    // 2.创建共享内存段
    int sm_id = shmget(key,20,IPC_CREAT | 0666);
    printf("create shared memory id: %d\n",sm_id);

    // 3. 挂载共享内存
    void* shm_ptr = shmat(sm_id,NULL,0);
    if(shm_ptr == (void*)-1){
        perror("shmat failed");
        exit(1);
    }
    // 写入共享内存
    char msg[] = "Hello world";
    strncpy(shm_ptr,msg, sizeof(msg));
  
    // 读取共享内存
    char line[sizeof(msg)];
    strncpy(line,shm_ptr, sizeof(line));
    printf("shm content: %s \n",line);
    return 0;
}
```

##### 4.进程取消连接共享内存

共享内存使用完毕后，通过`shmde`函数取消连接，类似于`malloc`和`free`的关系。

```c
int shmdt(const void* shmaddr);
# 成功返回0，出错返回-1;
```

取消挂载很简单，只需要传入`shmat`函数的返回值地址即可;

```c
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>

/**
 * 通过 shmat 函数连接共享内存段
 */

int main() {
    // 1.创建标识
    key_t key = ftok("HELP.md",9501);
    printf("create key: 0x%x \n",key);
    // 2.创建共享内存段
    int sm_id = shmget(key,20,IPC_CREAT | 0666);
    printf("create shared memory id: %d\n",sm_id);

    // 3. 挂载共享内存
    void* shm_ptr = shmat(sm_id,NULL,0);
    if(shm_ptr == (void*)-1){
        perror("shmat failed");
        exit(1);
    }
    printf("shm address: %p\n",shm_ptr); // shm address: 0x10253c000

    // 写入共享内存
    char msg[] = "Hello world";
    strncpy(shm_ptr,msg, sizeof(msg));

    // 读取共享内存
    char line[sizeof(msg)];
    strncpy(line,shm_ptr, sizeof(line));
    printf("shm content: %s \n",line); // Hello world

    // 4. 取消挂载共享内存
    shmdt(shm_ptr);
    return 0;
}
```

##### 5.删除共享内存

```c
#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>

int main() {
    // 1. 获取共享内存的键值
    key_t key = ftok("HELP.md",9501);

    // 2. 获取共享内存的标识符
    int shm_id = shmget(key,20,IPC_CREAT | 0666);

    // 3. 根据共享内存标识符 删除共享内存段
    if(shmctl(shm_id,IPC_RMID,NULL) == -1){
        perror("remove shm failed");
        exit(1);
    }
    return 0;
}
```

或者直接通过终端程序`ipcrm -m [shmid]`来删除共享内存块;

#### 消息队列

## 信号



### 1.引用

> 在Linux中，**信号(Signal)**是一种软中断，用于进程之间传递信息。信号可以被一个进程发送给另一个进程,用于通知目标进程发生了某个事件。

为了理解信号，举一个我们经常使用的案例场景:

1. 执行`./a.out`程序，在Shell下启动一个前台进程;
2. 在进程运行过程中，按下`Ctrl-C`，这时键盘输入产生一个**硬件中断**;
3. 如果CPU正在执行该进程的代码，则该进程的用户空间代码暂停执行，CPU由用户空间切换到内核态处**理硬件中断**;
4. 终端驱动程序将Ctrl-C解释成一个`SIGINT`信号，记在该进程的PCB中(也可以认为是终端进程向该前台进程发送了一个SIGINT信号);
5. 当该进程某个时刻要从内核态切换回用户空间之前，首先会先处理PCB中记录的信号，发现有一个`SIGINT`信号存在，而这个信号的默认处理动作是终止进程，所以会直接结束程序，而不会继续执行用户空间的代码;

注意,`Ctrl-C`产生的`SIGIN`信号只能发给前台进程，如果是`./a.out &`后台进程是接收不到该信号的。

Shell可以同时运行一个前台进程和任意多个后台进程，前台进程在运行过程中随时都有可能收到`SIGIN`信号，所以**信号相对于进程的控制流程来说是异步(Asynchronous)的**。

### 2.信号的种类

Linux系统共定义了64种信号，分为两大类：**可靠信号**与**不可靠信号**，前32种信号为不可靠信号，后32种为可靠信号。

- **不可靠信号**: 也称为非实时信号，不支持排队，信号可能会丢失, 比如发送多次相同的信号, 进程只能收到一次. 信号值取值区间为1~31；

- **可靠信号**: 也称为实时信号，支持排队, 信号不会丢失, 发多少次, 就可以收到多少次. 信号值取值区间为32~64;

在终端可以通过`kill -l`列出所有信号，也可以通过`man 7 signal`查看所有信号的解释。每个信号都有一个编号和一个宏定义名称，这些宏定义可以在`sys/signal.h`头文件中找到

下面针对一些常见的信号进行详解:

| Name      | Value | Default Action | Description                                                  |
| --------- | ----- | -------------- | ------------------------------------------------------------ |
| SIGHUP    | 1     | Term           | 挂起                                                         |
| SIGINT    | 2     | Term           | 结束(中断)进程。前台运行时按下(Ctrl-C)时触发                 |
| SIGQUIT   | 3     | Core           | 结束(终止)进程并且生成核心转储文件。前台运行时按下(Ctrl-\\)时触发 |
| SIGILL    | 4     | Core           | 非法指令                                                     |
| SIGTRAP   | 5     | Core           | 浮点异常                                                     |
| SIGABRT   | 6     | Core           | 由`abort`函数生成的信号，通常用于强制终止程序的执行          |
| SIGBUS    | 7     | Core           | 由于对无效的内存地址进行访问而导致终止进程的信号。           |
| SIGFPE    | 8     | Core           | 由于浮点异常导致终止进程的信号。例如将`0`作为除数则会产生该信号。 |
| SIGKILL   | 9     | Term           | 杀死进程。用于强制杀掉一个进程。                             |
| SIGUSR1   | 10    | Term           | 用户自定义信号1。                                            |
| SIGSEGV   | 11    | Core           | 由于对有效的内存地址进行错误操作而导致终止进程的信号。很常见的错误信号，使用未初始化的指针、数组越界等都会产生该信号。 |
| SIGUSR2   | 12    | Term           | 用户自定义信号2。                                            |
| SIGPIPE   | 13    | Term           | 由于对没有读端的管道写入而终止进程的信号。                   |
| SIGALRM   | 14    | Term           | 由`alarm`函数通知内核发出的信号。                            |
| SIGTERM   | 15    | Term           | 正常终止进程。这是一种相对温和的终止请求，允许程序在退出之前完成必要的清理工作。 |
| SIGSTKFLT | 16    |                | 栈溢出                                                       |
| SIGCHLD   | 17    | Ign            | 子进程退出信号。当子进程结束后，会向父进程发送该信号。       |
| SIGCONT   | 18    | Ign            | 当处于暂停的进程开始继续执行。                               |
| SIGSTOP   | 19    | Stop           | 进程暂停执行。                                               |
| SIGTSTP   | 20    | Stop           | 进程暂停执行。                                               |
| SIGTTIN   | 21    | Stop           | 进程暂停，后台进程从终端读数据时                             |
| SIGTTOU   | 22    | Stop           | 进程暂停，后台进程向终端写数据时                             |
| SIGURG    | 23    | Ign            | I/O有紧急数据到达当前进程                                    |
| SIGXCPU   | 24    | Term           | 进程的CPU时间片到期                                          |
| SIGXFSZ   | 25    | Term           | 文件大小的超出上限                                           |
| SIGVTALRM | 26    | Term           | 虚拟时钟超时                                                 |
| SIGPROF   | 27    | Term           | profile时钟超时                                              |
| SIGWINCH  | 28    | Ign            | 窗口大小改变                                                 |
| SIGIO     | 29    | Ign            | I/O相关                                                      |
| SIGPWR    | 30    |                | 关机                                                         |
| SIGSYS    | 31    | Core           | 系统调用异常                                                 |

每个信号都有一个默认的**处理行为(Action)**，这个行为决定了进程接收到不同的信号后，会做出不同的行为。例如有些信号的行为就非常暴力和激进，一旦触发就会**杀掉进程**，而有些信号就会比较温和，是可以被忽略或者补救。

如下列出信号的默认行为配置:

- **Term**: 表示终止进程。
- **Ign**: 表示忽略该信号。
- **Core**: 表示终止该进程，并且生成Core Dump`文件。
- **Stop**: 表示停止(暂停)该进程。
- **Cont**: 表示继续执行该进程(如果该进程当前处于停止状态(Stop))。



**产生信号的条件主要有:**

- 用户在终端按下某些键时，终端驱动程序会发送信号给前台进程，例如Ctrl-C产生`SIGINT`信号，Ctrl-\产生`SIGQUIT`信号，Ctrl-Z产生`SIGTSTP`信号。
- 硬件异常产生信号，这些条件由硬件检测到并通知内核，然后内核向当前进程发送适当的信号,如下举例:
  - 例如当前进程执行了除以0的指令，CPU的运算单元会产生异常，内核将这个异常解释为`SIGFPE`信号发送给进程;
  - 比如当前进程访问了非法内存地址，，MMU会产生异常，内核将这个异常解释为`SIGSEGV`信号发送给进程;
- 一个进程调用kill(2)函数可以发送信号给另一个进程。
- 可以用kill(1)命令发送信号给某个进程，kill(1)命令也是调用kill(2)函数实现的，如果不明确指定信号则发送`SIGTERM`信号，该信号的默认处理动作是终止进程。
- 当内核检测到某种软件条件发生时也可以通过信号通知进程，例如闹钟超时产生`SIGALRM`信号，向读端已关闭的管道写数据时产生`SIGPIPE`信号。

如果不想按默认动作处理信号，用户程序可以调用sigaction(2)函数告诉内核如何处理某种信号（sigaction函数稍后详细介绍），可选的处理动作有以下三种：

1. 忽略此信号;

2. 执行该信号的默认处理动作;

3. 提供一个信号处理函数，要求内核在处理该信号时切换到用户态执行这个处理函数，这种方式称为捕捉（Catch）一个信号。

### 3.信号的产生

#### 3.1 终端输入信号

上一节讲过，`SIGINT`信号是终止进程，`SIGQUIT`是终止进程并且进程**Core Dump**，接下来我们来验证一下。

首先解释什么是Core Dump。当一个进程要异常终止时，可以选择把进程的用户空间内存数据全部保存到磁盘上，文件名通常是core，这叫做Core Dump。进程异常终止通常是因为
有Bug，比如非法内存访问导致段错误，事后可以用调试器检查core文件以查清错误原因，这叫做Post-mortem Debug。

一个进程允许产生多大的core文件取决于进程的Resource Limit(这个信息保存在PCB中)，我们可以通过`ulimit -a`来查看Shell进程的资源使用限制信息:

```bash
$ ulimit -a
-t: cpu time (seconds)              unlimited		# CPU时间片限制
-f: file size (blocks)              unlimited		# 可创建的文件大小限制
-d: data seg size (kbytes)          unlimited		# 数据段大小限制
-s: stack size (kbytes)             8192				# 进程的栈大小限制
-c: core file size (blocks)         0						# 可以生成的Core Dump文件大小限制
-m: resident set size (kbytes)      unlimited
-u: processes                       6942				# 最大用户进程数量限制
-n: file descriptors                65535				# 最多可以打开多少文件描述符
-l: locked-in-memory size (kbytes)  64
-v: address space (kbytes)          unlimited		# 虚拟内存大小限制
-x: file locks                      unlimited		# 文件锁
-i: pending signals                 6942				
-q: bytes in POSIX msg queues       819200
-e: max nice                        0
-r: max rt priority                 0
-N 15:                              unlimited
```

默认是不允许产生core文件的，因为core文件中可能包含用户密码等敏感信息，不安全。在开发调试阶段可以用ulimit命令改变这个限制，允许产生core文件。首先用ulimit命令改变Shell进程的Resource Limit，允许core文件最大为1024K:

```bash
$ ulimit -c 1024
```

然后写一个死循环程序：

```c
int main(){
  while(1);
  return 0;
}
```

然后以前台进程方式运行这个程序，然后在终端输入`Ctrl-C`或`Ctrl-\`:

```bash
$ gcc main.c

$ ./a.out # 按下Ctrl-C
^C

$ ./a.out # 按下Ctrl-\
^\[1]    16715 quit (core dumped)  ./a.out

$ ls
a.out  core.16715  main.c
```

按下`Ctrl-\`触发`SIGQUIT`信号，终止进程后产生了`core.16715`文件。ulimit命令改变了Shell进程的Resource Limit，a.out进程的PCB由Shell进程复制而来，所以也
具有和Shell进程相同的Resource Limit值，这样就可以产生Core Dump了。

#### 3.2 通过系统调用发送信号

以上一节的死循环程序为例，以后台进程方式运行该程序:

```bash
# 后台运行
$ ./a.out &
[1] 20292

# 通过kill 向20292进程发送 SIGSEGV信号
$ kill -SIGSEGV 20292
[1]  + 20292 segmentation fault (core dumped)  ./a.out

# 产生了core dump
$ ls
a.out  core.20292  main.c
```

通过`kill`发送信号的写法有多种，以该案例发送的信号为例，分别可以写为如下几种:

```bash
$ kill -SIGSEGV [pid]
$ kill -SEGV [pid]
$ kill -11 [pid]
```

`11`是`SIGSEGV`信号的编号取值，而`SEGV`则是它的简写，这三种方式完全等价;

**kill和raise函数**

`kill`命令是通过调用`kill`函数实现的，`kill`函数可以向一个指定的进程发送指定的信号。

`raise`函数可以给当前进程发送指定的信号，也就是当前进程给自己发送信号。

```c
#include <signal.h>

int kill(pid_t pid, int signo);
int raise(int signo);
```

这两个函数都是成功时返回0，错误返回-1;

`abort`函数使当前进程接收到SIGABRT信号而异常终止。

```c
#include <stdlib.h>
void abort(void);
```

就像`exit`函数一样，`abort`函数总是会成功的，所以没有返回值。

#### 3.3 由软件条件产生信号

本节主要介绍`alarm`函数和`SIGALRM`信号。

```c
#include <unistd.h>

unsigned int alarm(unsigned int seconds);
```

调用`alarm`函数需要传入一个无符号整型`seconds`参数，告诉内核在`seconds`秒后给当前进程发生一个`SIGALRM`信号，该信号的处理动作是**终止当前进程**。

返回值是`0`则表示成功，若返回值是其他数值，则表示是之前调用过该函数，所距离发送信号的剩余秒数。

如果`seconds`设置为0，表示取消之前的调用，返回值是剩余时间。

如下案例: 

该程序的作用为10秒内不停的数数，10秒后被`SIGALRM`信号终止:

```c
#include <unistd.h>
#include <stdio.h>

int main(){
  
  int counter = 0;
  
  // 10秒后给进程发送 SIGALRM信号
  alarm(10);
  
  // 循环计数输出
  while(1){
    printf("counter: %d \n",counter);
    counter++;
    sleep(1);
  }
  return 0;
}
```

```bash
$ ./a.out
counter: 0 
counter: 1 
counter: 2 
counter: 3 
counter: 4 
counter: 5 
counter: 6 
counter: 7 
counter: 8 
counter: 9 
[1]    8286 alarm      ./a.out
```

### 4.阻塞信号

进程接收到信号并且执行信号的处理动作这个过程被称之为信号**递达(Delivery)**，而信号在**递达**之前，称之为信号**未决(Pending)**。

进程可以选择**阻塞(屏蔽)**某些信号，被**阻塞**的信号产生后将会保持在**未决状态**，不会被进程处理，直到该进程解除了对这些信号的阻塞后，才会执行递达动作。

注意，**阻塞信号**和**忽略信号**是不同的，忽略信号是信号递达之后的一种可选的处理动作，而阻塞信号相当于暂时搁置信号的处理，待到解除阻塞后，才可以递达。

如下示意图:

![](../asstes/1.8信号在内核中的表示.png)

每个信号都有两个标志位分别表示阻塞和未决，还有一个函数指针表示处理动作。信号产生时，内核在进程控制块中设置该信号的未决标志，直到信号递达才清除该标志。在上图的例子中:

1. SIGHUP信号未阻塞也未产生过，当它递达时执行默认处理动作。
2. SIGINT信号产生过，但正在被阻塞，所以暂时不能递达。虽然它的处理动作是忽略，但在没有解除阻塞之前不能忽略这个信号，因为进程仍有机会改变处理动作之后再解除阻塞。
3. SIGQUIT信号未产生过，一旦产生SIGQUIT信号将被阻塞，它的处理动作是用户自定义函数sighandler。

未决和阻塞标志可以用相同的数据类型sigset_t来存储，sigset_t称为信号集，这个类型可以表示每个信号的“有效”或“无效”状态，在阻塞信号集中“有效”和“无效”的含义是该信号是否被阻塞，而在未决信号集中“有效”和“无效”的含义是该信号是否处于未决状态。下一节将详细介绍信号集的各种操作。阻塞信号集也叫做当前进程的信号屏蔽字（Signal Mask），这里的“屏蔽”应该理解为阻塞而不是忽略。 



## 线程

## Shell编程

### 1.Shell的历史

Shell的作用就是**解释并且执行用户的指令**，用户输入一条命令，Shell就解释执行一条命令，这种方式称为**交互式(Interactive)**。

Shell还有另一种执行命令的方式: 将多条命令写成一个脚本(Script)，里面包含了要执行的多条命令，让Shell一次性把这些命令执行完，而不是一条一条的输入执行，这种方式称为**批处理(Batcg)**。Shell脚本和编程语言很相似，也有变量和流程控制语句，但Shell脚本是解释执行的，不需要编译，Shell程序从脚本中一行一行读取并执行这些命令，相当于一个用户把脚本中的命令一行一行敲到Shell提示符下执行。

由于历史原因，UNIX系统上有多种Shell:

- **sh (Bourne Shell)**: 由Steve Bourne开发，各种UNIX系统都配有sh。
- **csh (C Shell)**: 由Bill Joy开发，随BSD UNIX发布，它的流程控制语句很像C语言，支持很多Bourne Shell所不支持的功能：作业控制，命令历史，命令行编辑。
- **ksh (Korn Shell)**: 由David Korn开发，向后兼容sh的功能，并且添加了csh引入的新功能，是目前很多UNIX系统标准配置的Shell，在这些系统上/bin/sh往往是指向/bin/ksh的符号链接。
- **tcsh (TENEX C Shell)**: 是csh的增强版本，引入了命令补全等功能，在FreeBSD、Mac OS X等系统上替代了csh。
- **bash (Bourne Again Shell)**: 由GNU开发的Shell，主要目标是与POSIX标准保持一致，同时兼顾对sh的兼容，bash从csh和ksh借鉴了很多功能，是各种Linux发行版标准配置的Shell，在Linux系统上/bin/sh往往是指向/bin/bash的符号链接[36]。虽然如此，bash和sh还是有很多不同的，一方面，bash扩展了一些命令和参数，另一方面，bash并不完全和sh兼容，有些行为并不一致，所以bash需要模拟sh的行为：当我们通过sh这个程序名启动bash时，bash可以假装自己是sh，不认扩展的命令，并且行为与sh保持一致。

文件/etc/shells给出了系统中所有已知（不一定已安装）的Shell，除了上面提到的Shell之外还有很多变种:

```bash
$ cat /etc/shells
# List of acceptable shells for chpass(1).
# Ftpd will not allow users to connect who are not using
# one of these shells.

/bin/bash
/bin/csh
/bin/dash
/bin/ksh
/bin/sh
/bin/tcsh
/bin/zsh
```

用户的默认Shell设置在/etc/passwd文件中，例如下面配置设置:

```bash
$ cat /etc/passwd
...
root:x:0:0:root:/root:/usr/local/bin/zsh
...
zero:x:1000:1000::/home/zero:/bin/bash
```

当`root`用户打开终端窗口时，就会自动执行`/usr/local/bin/zsh`Shell。

而`zero`用户打开终端窗口时，就会自动执行`/bin/bash`Shell。

### 2.Shell执行命令

#### 2.1 执行交互式命令

用户在命令行输入命令后，一般情况下Shell会fork并exec该命令，但是Shell的内建命令例外，执行内建命令相当于调用Shell进程中的一个函数，并不创建新的进程。以前学过的`cd`、`alias`、`umask`、`exit`等命令即是内建命令，凡是用`which`命令查不到程序文件所在位置的命令都是内建命令。

本节会介绍很多内建命令，如`export`、`shift`、`if`、`eval`、`[`、`for`、`while`等等。内建命令虽然不创建新的进程，但也会有Exit Status，通常也用0表示成功非零表示失败，虽然内建命令不创建新的进程，但执行结束后也会有一个状态码，也可以用特殊变量$?读出。

#### 2.2 执行Shell脚本

首先编写一个简单的脚本，保存为script.sh:

```sh
#! /bin/sh

cd ..
ls
```

Shell脚本中用#表示注释，相当于C语言的//注释。

但如果#位于第一行开头，并且是`#!`（称为Shebang）则例外，它表示该脚本使用后面指定的解释器`/bin/sh`解释执行。

接下来为该脚本赋予执行权限，并且执行该脚本:

```bash
$ chmod +x script.sh
$ ./script.sh
```

Shell会`fork`一个子进程并调用`exec`执行`./script.sh`这个程序，
exec系统调用应该把子进程的代码段替换成./script.sh程序的代码段，并从它_start开始执行。然而script.sh是个文本文件，根本没有代码段和_start函数，怎么办呢？其实exec还有另外一种机制，如果要执行的是一个文本文件，并且第一行用Shebang指定了解释器，则用解释器程序的代码段替换当前进程，并且从解释器的_start开始执行，而这个文本文件被当作命令行参数传给解释器。因此，执行上述脚本相当于如下操作:

```bash
$ /bin/sh ./script.sh
```

以这种方式执行不需要script.sh文件具有可执行权限。

两种执行Shell脚本的方法:

```bash
$ ./script.sh

$ sh ./script.sh
```

这两种方法本质上是一样的,另外还有一种方式:

```bash
$ source ./script.sh

或者

$ . ./script.sh
```

source或者.命令是Shell的内建命令，这种方式也不会创建子Shell，而是直接在交互式Shell下逐行执行脚本中的命令。也就是说脚本中所执行的命令会影响到Shell本身，例如在脚本中通过`cd`内建命令进入了其他工作目录，结束后Shell也会处于这个工作目录。

### 3.Shell基本语法

##### 变量

按照惯例，Shell变量由全大写字母加下划线组成，有两种类型的Shell变量:

- **本地变量**: 只存在于当前Shell进程，用set命令可以显示当前Shell进程中定义的所有变量（包括本地变量和环境变量）和函数。
- **环境变量**: 环境变量可以从父进程传给子进程，因此Shell进程的环境变量可以从当前Shell进程传给fork出来的子进程。用printenv命令可以显示当前Shell进程的环境变量。

环境变量是任何进程都有的概念，而本地变量是Shell特有的概念。在Shell中，环境变量和本地变量的定义和用法相似。

在Shell中定义或赋值一个变量：

```sh
$ UNAME=zhangsan
```

注意`=`两边不能有空格，否则会被Shell解释成命令和命令行参数。

本地变量定义后仅存在于当前Shell进程，使用`export`命令可以将本地变量导出为环境变量:

```sh
$ UNAME=zhangsan
$ export UNAME
```

也可以一步完成:

```sh
$ export UNAME=zhangsan
```

使用`unset`命令可以删除已定义的本地变量或环境变量:

```sh
$ unset UNAME
```

可以通过`$`或者`${}`来获取已存在的变量的值，如下所示:

```sh
$ echo $UNAME
zhangsan
$ echo ${UNAME}
zhangsan
```

Shell中的变量不需要明确指定类型，事实上Shell变量的值都是字符串，比如我们定义VAR=45，其实VAR的值是字符串45而非整数。Shell变量不需要先定义后使用，如果对一个没有定义的变量取值，则值为空字符串。

##### 文件名代换

这些用于匹配的字符称为通配符（Wildcard），具体如下:

| 通配符     | 说明                               |
| ---------- | ---------------------------------- |
| *          | 匹配0个或多个任意字符              |
| ?          | 匹配一个任意字符                   |
| [若干字符] | 匹配方括号中任意一个字符的一次出现 |

```sh
$ ls /dev/ttyS*
$ ls ch0?.doc
$ ls ch0[0-2].doc
$ ls ch[012][0-9].doc
```

注意，Globbing所匹配的文件名是由Shell展开的，也就是说在参数还没传给程序之前已经展开了，比如上述ls ch0[012].doc命令，如果当前目录下有ch00.doc和ch02.doc，则传给ls命令的参数实际上是这两个文件名，而不是一个匹配字符串。

##### 命令代换

用反引号或者`$()`括起来的内容也可以是一条命令，Shell会优先执行该命令，然后将命令的输出结果作为值替换到命令行中，如下:

```sh
$ DATE=`date`
$ echo $DATE
2024年 01月 28日 星期日 18:50:50 CST
```

也可以使用`$()`来代换:

```sh
$ DATE=$(date)
$ echo ${DATE}
2024年 01月 28日 星期日 18:50:50 CST
```

##### 算术代换

之前说到了，Shell中的变量都是字符串类型，那么如果想进行数字运算呢？这个时候就需要使用`$(())`来进行运算，`$(())`中的Shell变量取值将转换为整数，如下:

```sh
$ AGE=18
$ NEW_AGE=$(($AGE + 3))
$ echo $NEW_AGE
21
```

`$(())`中只能用 + - * / 和()运算符，并且只能做整数运算。

##### 单引号

Shell脚本中的单引号和双引号一样都是字符串的界定符。单引号保证引号内所有的字符的字面值，不存在特殊含义，所以不会进行解析,如下:

```sh
$ echo '$SHELL'
$SHELL
```

##### 双引号

双引号同样也是字符串边界符，但是会对特殊字符进行解析，如下:

```sh
$ echo "$SHELL"
/usr/local/bin/zsh

$ echo "`date`"
2024年 01月 28日 星期日 19:04:27 CST
```

### 4.Bash启动脚本

启动脚本是**bash**程序启动时自动执行的脚本。用户可以把一些环境变量的设置和alias、umask设置放在启动脚本中，这样每次启动Shell时这些设置都自动生效。

启动bash的方法不同，执行启动脚本的步骤也不相同，具体可分为以下几种情况:

#### - 登录交互式启动

交互Shell是指用户在提示符下输命令的Shell而非执行脚本的Shell，登录Shell就是在输入用户名和密码登录后得到的Shell，比如从字符终端登录或者用telnet/ssh从远程登录。

这样启动bash会自动执行以下脚本:

1. 首先执行`/etc/profile`脚本文件，系统中每个用户登录时都要执行这个脚本，如果系统管理员希望某个设置对所有用户都生效，可以写在这个脚本里。

2. 然后在当前用户主目录下依次寻找`~/.bash_profile`、`~/.bash_login`和`~/.profile`三个脚本文件，按照这个顺序找到第一个存在并且可读的脚步文件来执行。如果希望某些配置只对当前用户生效，那么可以配置在这里。并且由于该脚本是在`/etc/profile`之后执行，所以该脚本的优先级更高，重复的配置会覆盖掉`/etc/profile`文件中的配置。

   `~/.profile`这个启动脚本是sh规定的，bash规定首先查找以`~/.bash_`开头的启动脚本，如果没有则执行`~/.profile`，是为了和sh保持一致。

3. 顺便一提，在退出登录时会执行~/.bash_logout脚本（如果它存在的话）。

#### - 非登录交互式启动

比如在图形界面下打开一个终端窗口，或者在Shell中输入bash命令，就得到一个非登录式Shell。

这种Shell在启动时自动执行`~/.bashrc`脚本。

为了使登录式Shell也能执行`~/.bashrc`脚本，通常会在`~/.bash_profile`中调用执行`~/.bashrc`文件，在`~/.bash_profile`中追加以下内容:

```shell
if [ -f ~/.bashrc]; then
	. ~/.bashrc
fi
```

这段代码的意思是: 如果`~/.bashrc`文件存在，则以`source`方式执行它。

多数Linux发行版在创建帐户时会自动创建`~/.bash_profile`和`~/.bashrc`脚本，`~/.bash_profile`中通常都有上面这几行。所以，如果要在启动脚本中做某些设置，使它在图形终端窗口和字符终端的Shell中都起作用，最好就是在`~/.bashrc`中设置。

#### - 非交互式启动

为执行脚本而fork出来的子Shell是非交互Shell，启动时执行的脚本文件由环境变量BASH_ENV定义，相当于自动执行以下命令：

```shell
if [ -n "$BASH_ENV" ]; then
		. "$BASH_ENV";
fi
```

如果环境变量BASH_ENV的值不是空字符串，则把它的值当作启动脚本的文件名，source执行这个脚本。

#### - 以sh命令启动

如果以sh命令启动bash，bash将模拟sh的行为，以~/.bash_开头的那些启动脚本就不认了。所以，如果作为交互登录Shell启动，或者使用--login参数启动，则依次执行以下脚本：

1. `/etc/profile`
2. `~/.profile`

如果作为交互Shell启动，相当于自动执行以下命令：

```shell
if [ -n "$ENV" ]; then . "$ENV"; fi
```

如果作为非交互Shell启动，则不执行任何启动脚本。通常我们写的Shell脚本都以`#! /bin/sh`开头，都属于这种方式。

### 5.Shell脚本语法

#### 5.1 条件断言

通过`test`命令或者`[]`可以断言一个条件是否成立，如果断言结果为真则命令的Exit Status为0，反之为1(与C语言的逻辑表示整好相反)。

如下例子，断言两个数字的大小:

```bash
# 定义两个数字变量
❯ NUM1=18
❯ NUM2=20

# NUM1 小于 NUM2
$ test $NUM1 -lt $NUM2 
$ echo $? 
0 # 输出0，表示为真

# NUM1 大于 NUM2
$ test $NUM1 -gt $NUM2
$ echo $? 
1 # 输出1，表示为假

# NUM1 等于 NUM2
$ test $NUM1 -eq $NUM2
$ echo $?
1
```

`test`也可以替换为如下写法:

```bash
$ [ $NUM1 -eq 18 ]
$ echo $?
0
```

常见的断言命令参数有如下:

| 命令                   | 说明                                                         |
| ---------------------- | ------------------------------------------------------------ |
| [ -d `DIR` ]           | 如果`DIR`存在并且是一个目录则为真，反之为假;                 |
| [ -f `FILE` ]          | 如果`FILE`存在且是一个普通文件则为真                         |
| [ -z `STRING`]         | 如果`STRING`的长度为零则为真(用于判断是否为空字符串，或者变量是否未声明) |
| [ -n `STRING`]         | 如果`STRING`的长度非零则为真                                 |
| [ STRING1 = STRING2 ]  | 如果两个字符串相同则为真                                     |
| [ STRING1 != STRING2 ] | 如果字符串不相同则为真                                       |
| [ ARG1 OP ARG2 ]       | ARG1和ARG2应该是整数或者取值为整数的变量，OP的可选项有:<br>-eq(==)<br>-ne(!=)<br>-lt(<)<br>-le(<=)<br>-gt(>)<br>-ge(>=) |

使用案例如下：

```bash
$ ls
coder  hello.s  tools

# 判断 coder目录是否存在
$ test -d coder
$ echo $?
0

# 判断 hello.s文件是否存在
$ test -f hello.s
$ echo $?
0

# 断言字符长度是否为0
$ test -z ""
$ echo $?
0
# 断言一个变量是否未定义
$ test -z $CUSTOM
$ echo $?
0

# 断字符串长度为非0
$ test -n 'zhangsan'
$ echo $?
0
# 断言一个变量是否已存在
$ test -n $NUM1
$ echo $?
0

```

多个条件断言之间可以做与、或、非逻辑运算:

| 表达式             | 说明                                              |
| ------------------ | ------------------------------------------------- |
| [ ! EXPR ]         | EXPR可以是上表中的任意一种测试条件，!表示逻辑反。 |
| [ EXPR1 -a EXPR2 ] | -a 表示逻辑与(and)，两个表达式都为真时才为真。    |
| [ EXPR -o EXPR2 ]  | -o 表示逻辑或(or)，有一个表达式为真则为真。       |

案例如下:

```bash
$ [ $NUM1 -eq 18 -a $NUM2 -eq 20 ]
$ echo $?
0

$ test $NUM1 -eq 18 -o $NUM2 -eq 22
$ echo $?
0
```

#### 5.2 条件分支语句

和C语言类似，在Shell中用if、then、elif、else、fi这几条命令实现分支控制。这种流程控
制语句本质上也是由若干条Shell命令组成的，例如先前讲过的:

```shell
if [ -f ~/.bashrc ]; then
. ~/.bashrc
fi
```

其实是三条命令，if [ -f ~/.bashrc ]是第一条，then . ~/.bashrc是第二条，fi是第三条。如果两条命令写在同一行则需要用;号隔开，一行只写一条命令就不需要写;号了，另外，then后面有换行，但这条命令没写完，Shell会自动续行，把下一行接在then后面当作一条命令处理。和[命令一样，要注意命令和各参数之间必须用空格隔开。if命令的参数组成一条子命令，如果该子命令的Exit Status为0（表示真），则执行then后面的子命令，如果Exit Status非0（表示假），则执行elif、else或者fi后面的子命令。if后面的子命令通常是测试命令，但也可以是其它命令。Shell脚本没有{}括号，所以用fi表示if语句块的结束。见下例:

```shell
#! /bin/sh

if [ -f /bin/bash ]
        then echo "/bin/bash is a file"
        else echo "/bin/bash is not a file"
fi

if :;
        then echo "always true"; fi

```

`:`是一个特殊的命令，称为空命令，该命令不做任何事，但Exit Status总是真。此外，也可以执
行`/bin/true`或`/bin/false`得到真或假的Exit Status。再看一个例子：

```shell
#! /bin/sh

echo "Is it morning? Please answer yes or no."

# 等待用户输入一行字符串
read YES_OR_NO

# 根据输入的不同，输出不同的信息
if [ "$YES_OR_NO" = "yes" ]; then
        echo "Good morining"
elif [ "$YES_OR_NO" = "no" ]; then
        echo "Good afternoon!"
else
        echo "Sorry, $YES_OR_NO not recognized.  Enter yes or no."
        exit 1
fi
exit 0
```

`read`命令的作用是等待获取一行标准输入字符串，将该字符串内容保存在一个`YES_OR_NO`变量中。



执行结果:

```bash
$ ./script.sh
Is it morning? Please answer yes or no.
yes
Good morining


$ ./script.sh
Is it morning? Please answer yes or no.
no
Good afternoon!


$ ./script.sh
Is it morning? Please answer yes or no.
hhh
Sorry, hhh not recognized.  Enter yes or no.
```

此外，Shell还提供了&&和||语法，和C语言类似，具有Short-circuit特性，很多Shell脚本喜欢写
成这样：

```bash
$ test "$(whoami)" != 'root' && (echo you are using a non-privileged account; exit 1)
```

&&相当于“if...then...”，而||相当于“if not...then...”。&&和||用于连接两个命令，而上面讲的-a和-o仅用于在测试表达式中连接两个测试条件，要注意它们的区别，例如:

```bash
$ test "$VAR" -ge 1 -a "$VAR" -lt 3
```

和以下写法完全等价:

```bash
$ test "$VAR" -gt 1 && test "$VAR" -lt 3
```

#### 5.3 多条件分支语句

case命令可类比C语言的switch/case语句，esac表示case语句块的结束。C语言的case只能匹配整型或字符型常量表达式，而Shell脚本的case可以匹配字符串和Wildcard，每个匹配分支可以有若干条命令，末尾必须以;;结束，执行时找到第一个匹配的分支并执行相应的命令，然后直接跳到esac之后，不需要像C语言一样用break跳出。

```shell
#!/bin/zsh

echo "Is it morning? Please answer yes or no."

# 获取输入
read LINE

case "$LINE" in

# 第一条分支匹配
yes | y | Yes | YES )
  echo "Good Morning!" ;;

# 第二条分支匹配
[nN]*)
  echo "Good Afternoon!" ;;

# 其他分支处理
*)
  echo "Sorry, $YES_OR_NO not recognized. Enter yes or no."
  exit 1
  ;;
esac
exit 0
```

#### 5.4 循环语句

##### for

Shell脚本的for循环结构和C语言很不一样，它类似于某些编程语言的foreach循环。例如:

```shell
#! /bin/sh

for ITEM in apple banana pear; do
	echo "I like $ITEM"
done
```

`ITEM`是循环迭代的临时变量，第一次循环取值为`apple`，第二次取值是`banana`，第三次取值是`pear`。

举例有需求: 将当前目录下的chap0、chap1、chap2等文件名改为chap0~、chap1~、chap2~等（按惯例，末尾有~字符的文件名表示临时文件），该程序可以这样写:

```shell
#! /bin/sh

for FILE_NAME in chap?; do
	mv $FILENAME $FILE_NAME~
done
```

##### while

while的用法和C语言类似。

```shell
#! /bin/zsh
COUNT=3

while [ ! $COUNT -eq 0 ]; do
	echo "Hello World $COUNT"
  COUNT=$((COUNT-1))
done
```

练习需求：输入一个密码，如果超过5次不正确，则结束程序.

```shell
#! /bin/sh

echo "Enter password:"
read PASSWD

TRY_COUNT=1

while [ "$PASSWD" != "root" ]; do
  if [ "$TRY_COUNT" = 5 ]; then
    echo "Sorry, no retries available"
    exit 1
  fi
	echo "Sorry, try again"
	TRY_COUNT=$((TRY_COUNT+1))
	read PASSWD
done
```

### 6.特殊变量

在Shell中有许多变量都是被自动赋值的，如下几种:

| 特殊变量        | 代表含义                                                     |
| --------------- | ------------------------------------------------------------ |
| `$0、$1、$2...` | 启动进程的命令行参数，相当于C语言main函数的`argv[0]、argv[1]、argv[2]...` |
| `$#`            | 命令行参数的数量(不包括第一个参数)，相当于C语言main函数的argc - 1 |
| `$@`            | 命令行参数列表(不包含第一个参数)                             |
| `$?`            | 上一条命令的Exit Status                                      |
| `$$`            | 当前Shell的进程号                                            |

```shell
#!/bin/zsh

echo "共 $# 个命令行参数"

# 根据索引访问命令行参数
echo "第一个参数: $0"
echo "第一个参数: $1"
echo "第一个参数: $2"
```

遍历参数列表

```shell
# 遍历命令行参数 第一种方式
INDEX=1
while [ "$#" -gt 0 ]; do
    echo "第 $INDEX 个参数: $1"
    shift
done
```

```shell
# 遍历命令行参数 第二种方式
for ITEM in "$@" ; do
    echo "$ITEM"
done
```

### 7.可执行命令

针对一些非常常用的终端程序进行逐一详解和学习.

#### echo

`echo`译为回音，该程序接收多个参数，然后在**标准输出**中以一个字符串形式输出。

基础用法如下:

```bash
$ echo hello world
hello world
```

`echo`程序也可以结合`$`符来使用，`$`用来获取变量的值:

```bash
$ nickname=张三
$ echo hello world $nickname
hello world 张三

$ echo $?
0
```

`$?`表示获取Shell的上一个程序的终止状态，0表示正常终止，非0表示异常终止.

参数使用`''`括起来表示为纯文本，`$`符将不会进行语义解析:

```bash
$ echo '$nickname 你好 哈哈哈'
$nickname 你好 哈哈哈
```

参数使用`""`括起来表示为一个整体字符串，这样就可以识别`\`反斜杠转义字符和空格等特殊字符:

```bash
$ echo "hello\nworld"
hello
world

$ echo "hello             world"
hello             world
```

如果不使用`“”`，那么结果会有何不同呢?

可选参数有如下:

| 参数 | 作用                         |
| ---- | ---------------------------- |
| -n   | 输出内容不追加`\n`换行符     |
| -e   | 启用`\`反斜杠的转义解释      |
| -E   | 关闭\`反斜杠的转义解释(默认) |

 



## 网络编程





