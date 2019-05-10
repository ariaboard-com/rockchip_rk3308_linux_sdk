# Download SDK #
The SDK is about 2.2 GiB. You can clone from Gitlab directly. (It may be fail due to network flow limit).

We have provided a 7z ball of the .git directory at here :

* [Firefly-RK3308_Linux_SDK](http://www.t-firefly.com/doc/download/55.html#other_165)

Please check the md5 checksum before proceeding:
```
#!shell
$ md5sum Firefly-RK3308_Linux_SDK_git_20181008.7z
3800278cc016d117eec8c0381ba66769  Firefly-RK3308_Linux_SDK_git_20181008.7z
```

If it is correct, uncompress it:
```
#!shell
$ mkdir -p ~/prj/Firefly-RK3308
$ cd ~/prj/Firefly-RK3308
$ 7zr x Firefly-RK3308_Linux_SDK_git_20181008.7z
$ git reset --hard
```

Remote repository has been added by default
```
#!shell
$ git remote -v
gitlab	https://gitlab.com/TeeFirefly/rk3308-linux.git (fetch)
gitlab	https://gitlab.com/TeeFirefly/rk3308-linux.git (push)
```

From now on, you can pull updates from gitlab:
```
#!shell
$ git pull gitlab firefly:firefly
```

Please visit the following website for more details of our ROC-RK3308-CC development board. Thank you.

  [https://gitlab.com/TeeFirefly/rk3308-linux](https://gitlab.com/TeeFirefly/rk3308-linux) 

## 下载 Linux SDK

由于 SDK 比较大，我们提供了一个打包好的.git以供下载：

> * [Firefly-RK3308_Linux_SDK](http://www.t-firefly.com/doc/download/55.html#other_165)

下载完成后先验证一下 MD5 码：   
```shell
$ md5sum Firefly-RK3308_Linux_SDK_git_20181008.7z
3800278cc016d117eec8c0381ba66769  Firefly-RK3308_Linux_SDK_git_20181008.7z
```

确认无误后，就可以解压：   
```shell
$ mkdir -p ~/prj/Firefly-RK3308
$ cd ~/prj/Firefly-RK3308
$ 7zr x Firefly-RK3308_Linux_SDK_git_20181008.7z
$ git reset --hard
```

解压出来的SDK，已经默认添加远程仓库
```shell
$ git remote -v
gitlab	https://gitlab.com/TeeFirefly/rk3308-linux.git (fetch)
gitlab	https://gitlab.com/TeeFirefly/rk3308-linux.git (push)
```

以后就可以直接从 gitlab 处更新：   
```shell
$ git pull gitlab firefly:firefly
```

也可以到 [https://gitlab.com/TeeFirefly/rk3308-linux](https://gitlab.com/TeeFirefly/rk3308-linux) 在线浏览源码
