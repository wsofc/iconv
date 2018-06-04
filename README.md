
# 跨平台的字符转码库
1. pc 平台需要定义相关导出函数 编译出dll

2. android 需要编写.mk编译出.so

3. ios 有现成的.tbd(.framework),直接引用即可

4. 附带一个c++版转码类(其中iconv和stl11转码两种实现方式)
