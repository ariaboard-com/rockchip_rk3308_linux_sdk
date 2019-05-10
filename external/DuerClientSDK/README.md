# DevKit使用方法：

- DevKit建议编译环境为Ubuntu64 16.04、cmake 3.1以上版本。
- 在build目录下有xxx.build.conf文件，修改其中有关编译器选项的配置为自己对应环境配置。
- 修改xxx.build.conf文件后，回到上一级目录，运行sh build.sh build/xxx.build.conf即可产生对应的demo程序。



注：

1、目前SDK仍然在持续迭代中，部分头文件位置可能会有调整，接口API也可能会有小幅变化，部分SO库存在增删可能

2、third目录中的lib库使用请参考使用，编译脚本参见build_third.sh


