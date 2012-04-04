portal 
======
本程序利用了 cURL 库，所以需要安装这个库。
方法为::

   $ sudo apt-get install libcurl4-openssl-dev

以上是在debian中安装方法，对于其他系统可以尝试::

   $ apt-cache search libcurl

查找合适的包再重复第一步以安装相应的库。

当然也可以直接下载源代码编译安装这个库。

本归档文件中附带了两个可执行文件，
分别是32位版本和64位版本。


编译安装
--------
首先解压归档文件到一个目录，并进入这个目录。
在终端上输入::

   $ make
   $ make install

使用说明
--------
上线方法::

   $ portal l _帐号 _密码

其中的 _帐号 和 _密码 要换成相应的帐号密码。
另一个上线方法::

   $ portal l

这需要在 $HOME 目录创建一个名为 .portal 的文件，
写上用户名密码，中间用空格隔开。
比如::

   $ echo "b091180000 0000000"  > ~/.portal

下线方法::
   
   $ portal d

其中的 d 可以省略。

卸载方法::
 
   $ make uninstall

作者
----
weibo::

   @Fanchy_Lee
