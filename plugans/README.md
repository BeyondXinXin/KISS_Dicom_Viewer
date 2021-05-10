
# CBCT 模块

## 说明

1. 暂不开源

## 开发进度

* [x] 1.统一开源库版本
* [x] 2.新建工程
* [x] 3.打开
* [x] 4.显示多个视图
* [x] 5.三视图再体渲染中显示，响应滚轮切换
* [x] 6.调整对比度、亮度
* [x] 7.2D、3D测量
* [x] 8.体渲染6种模式、骨密度调整、一个可以拖动点的图表
* [x] 9.视图 正、左、右
* [x] 10.完整标签  
* [x] 11.清晰度调整
* [x] 12.滚动条
* [x] 13.输出图片  
* [x] 14.内存泄漏、闪退
* [x] 15.线密度
* [x] 16.矩形骨密度
* [x] 17.vr包围盒剪裁交互
* [x] 18.上传编译后第三方库
* [x] 19.CPR 选点
* [ ] 20.CPR 可视化
* [ ] 21.vs2019


## 开源库版本和配置选项

* CMake-3.17.2  
* Qt-5.15.2   
&emsp;&emsp;&emsp;msvc2015 64
* VTK-9.0.1  
&emsp;&emsp;&emsp;VTK_GROUP_ENABLE_Qt  ON  
&emsp;&emsp;&emsp;VTK_MODULE_ENABLE_VTK_GUISupportQt  ON  
&emsp;&emsp;&emsp;VTK_MODULE_ENABLE_VTK_GUISupportQtSQL  ON  
&emsp;&emsp;&emsp;VTK_MODULE_ENABLE_VTK_RenderingQt  ON  
&emsp;&emsp;&emsp;VTK_MODULE_ENABLE_VTK_ViewsQt  ON  
&emsp;&emsp;&emsp;Qt5_DIR C:/Qt/5.15.2/msvc2015_64/lib/cmake/  
* ITK-5.1.2  
&emsp;&emsp;&emsp;INSTALL_GTEST OFF  
&emsp;&emsp;&emsp;BULID_TESTING OFF  
&emsp;&emsp;&emsp;Module_ITKVtkGlue ON  
&emsp;&emsp;&emsp;VTK_DIR  D:/lib/VTK-9.1.0/bulid  
* DCMTK-3.6.5  
&emsp;&emsp;&emsp;DCMTK_ENABLE_STL_STRING  ON  
&emsp;&emsp;&emsp;BULID_SHARED_LIBS  ON  
