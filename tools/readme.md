# 工具使用说明


这里使用的python都是使用的python2.7， 没有测试过在3.x版本下兼容的情况


## export.sh

使用shell脚本调用glfw里接口， glfw里使用tiny_obj_loader解析模型文件(.obj), 这里进一步解析，使生成的文件直接对应到OpenGL的EBO,VAO, 进而提高渲染效率

<br>

## hash.py

引擎使用的资源在ios下都打到main bundle里， 而且main bundle不区分路径，所有路径的文件最后都在一个沙盒里， 所以xcode不允许不同文件夹下同名文件放到一个bundle里。

这个根据文件路径生成hash值，使用hash值重命名资源，这样就不会重复了。而且使用hash，也可以不直接把文件格式给外部，也是为了加密。

为了python里计算hash的算法和c++里算法是一样的，hash.py回调libc.c的接口。

<br>


## icon.py

iOS生成不同分辨率的icon和json配置


<br>


## mesh.py

一些通用的网格导出， 比如说quad, cube, plane...

引擎会引用这些网格，而不用需要在代码里声明一个个很大的数组。

<br>

## texture.sh

用来生成mac和iOS平台支持的压缩纹理，这里调用了xcode内置的texturetool工具，根据自己需求转换不同格式的纹理

此脚本还会调用solution.py，来转换图片的分辨率， 因为诸如PVRTC压缩格式的Texture必须是2的次幂才可以转换成功。

关于更多纹理压缩的相关的介绍，点击[查看][i1]

<br>

## solution.py

PIL转换图片分辨率

<br>

## sample.sh

使用cubemap采样env, 生成球谐参数


<br><br>
penghuailiang<br>
2019-05-10


[i1]: https://huailiang.github.io/blog/2019/texture/
