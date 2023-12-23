
/**
 * C语言规定了signed和unsigned两个关键字，unsigned char型表示无符号数，signed char型表示有符号数。
 * 这两个关键字仅能用于修饰整型: char、 short、 int、 long等类型;
 *
 *  - signed(有符号): 可以表示为正数、负数和零;
 *  - unsigned(无符号): 仅能表示正数和零;
 *
 * 那么以前我们常用的不带signed或unsigned关键字的char型是无符号数还是有符号数呢？
 * C标准规定这是Implementation Defined，编译器可以定义char型是无符号的，也可以定义char型是有符号的，
 * 在该编译器所对应的体系结构上哪种实现效率高就可以采用哪种，x86平台的gcc定义char型是有符号的。
 * 这也是C标准的Rationale之一：优先考虑效率，而可移植性尚在其次。
 *
 * 注意，ASCII码的取值范围是0～127，所以不管char型是有符号的还是无符号的，保存一个ASCII码都没有问题，
 * 一般来说，如果用char型保存ASCII码字符，就不必明确写是signed还是unsigned；
 * 如果用char型表示8位的整数，为了可移植性就必须写明是signed还是unsigned。
 *
 */
