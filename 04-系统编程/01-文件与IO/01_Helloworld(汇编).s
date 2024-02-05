;; 定义数据段
;.data
;	; 定义 msg .ascii字符串
;	msg: .ascii "Hello，world!\n"
;	; 计算 msg 的字符长度
;	len = . - msg
;
;; 代码段
;.text
;	; 指定程序入口点
;	.global _start
;
;; 程序入口
;_start:
;	movl		$len, %edx	; 将msg字符串长度移动到 edx 寄存器
;	movl		$msg, %ecx	; 将msg字符串首地址移动到 ecx 寄存器
;	movl		$1, %ebx		; 将$1(标准输出的文件描述符)移动到 ebx 寄存器
;	movl		$4, %eax		; 将$4(`sys_write`系统函数)移动到 eax 寄存器，表示要进行IO写操作
;	int			$0x80				; 触发系统调用，执行sys_write写操作，将 ecx 指向的字符串输出到标准输出设备
;	movl		$0, %ebx		; 将 0 移动到 ebx 寄存器，作为程序退出状态码
;	movl		$1, %eax		; 将$1(sys_exit 系统函数，用于退出程序)移动到 eax 寄存器.
;	int			$0x80				; 触发系统调用，执行sys_exit退出程序