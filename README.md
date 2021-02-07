
&emsp;&emsp;开发中......  

项目地址：
[CodeChina kissDicomViewer](https://codechina.csdn.net/a15005784320/kiss-dicom-viewer)  
详细介绍：
[CSDN 一个简单的 DICOM 浏览器](https://beondxin.blog.csdn.net/article/details/108678403)  

---


@[TOC](kissDicomViewer)
# kissDicomViewer
## 1 Demo展示

![在这里插入图片描述](https://img-blog.csdnimg.cn/20210207220052527.gif#pic_center)


![在这里插入图片描述](https://img-blog.csdnimg.cn/20210207215731176.gif)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210207215730987.gif)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210207215730938.gif)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210207215730626.gif)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210207215730494.gif)

![在这里插入图片描述](https://img-blog.csdnimg.cn/2021020721574924.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2ExNTAwNTc4NDMyMA==,size_16,color_FFFFFF,t_70)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20210207215756662.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2ExNTAwNTc4NDMyMA==,size_16,color_FFFFFF,t_70)

![在这里插入图片描述](https://img-blog.csdnimg.cn/2021020721580197.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2ExNTAwNTc4NDMyMA==,size_16,color_FFFFFF,t_70)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20210207215815761.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2ExNTAwNTc4NDMyMA==,size_16,color_FFFFFF,t_70)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20210207215819503.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2ExNTAwNTc4NDMyMA==,size_16,color_FFFFFF,t_70)




---
## 2 前言
&emsp;&emsp;一直没有找到一个合适的`Dicom`浏览器。`Windos`下的小蚂蚁(`RadiAnt Dicom Viewer`)很好用但可惜不是跨平台的，只有`Windos`版本。平时工作系统是`Ubuntu/Deepin`，虽然用`deepin-wine5`也可以安装`RadiAnt Dicom Viewer`但是跟`Windos`下比起来很多功能不能用，比如`pacs scu`，数据库（`rdvdb`）用`deepin-wine5`装的也有`bug`，小蚂蚁中文版还有很多乱码。  
&emsp;&emsp;全平台都支持并且跟小蚂蚁一样又小又快的我找到两个`Papaya`和`DWV`，该有的功能很齐全，虽然开源但是一个是`JavaScript`一个是`lua`，如果想大改自己改也无从下手。  
&emsp;&emsp;`ItkSnap`和`3DSlicer`也都是全平台的，都很强大而且都是开源的。架构都很好，可以自己任意开发插件添加进去。如果把这两个只当成`DICOM`浏览器实在有点大材小用。    

&emsp;&emsp;现成软件下载：  
[RadiAnt Dicom Viewer](https://www.radiantviewer.com/)  
[Papaya](https://github.com/rii-mango/Papaya)  
[DWV](https://github.com/ivmartel/dwv)  
[ItkSnap](https://github.com/pyushkevich/itksnap)   
[3DSlicer](https://www.slicer.org/slicer3-downloads/)   


> `DICOM`浏览器就是一个图片浏览器，只需要把`DICOM`文件解析成图片然后显示就可以了，成熟的有一大把。工作需要的话直接



---
## 3 软件介绍
### 3.1 软件命名
&emsp;&emsp;**KISS Dicom Viewer**   
&emsp;&emsp;
&emsp;&emsp;&emsp;&emsp;**KISS**指：**Keep it Simple and Stupid**。
### 3.2 第三方库
- 界面 [Qt](https://www.qt.io/)  
- DICOM协议相关用&emsp;[DCMTK](https://github.com/DCMTK/dcmtk)  
- 数据库用&emsp;[Sqlite3](https://www.sqlite.org/index.html)  
- 数据库浏览打算改一下&emsp;[sqlitebrowser](https://github.com/sqlitebrowser/sqlitebrowser)  
- PACS用&emsp;[DICOM storage (C-STORE) SCU](https://support.dcmtk.org/docs/storescu.html)  
- 2D图片浏览用 &emsp;[QGraphicsView](https://doc.qt.io/qt-5/qgraphicsview.html)  
- 多平面重建-MPR用 &emsp;  [QVTKOpenGLNativeWidget](https://vtk.org/doc/nightly/html/classQVTKOpenGLNativeWidget.html)  
- 三维重建-体渲染用&emsp;   [QVTKOpenGLNativeWidget](https://vtk.org/doc/nightly/html/classQVTKOpenGLNativeWidget.html) + [ITK]()  
- 图像融合和图像处理用&emsp; [Opencv](https://github.com/opencv/opencv)  

### 3.3 程序架构
&emsp;&emsp;想了下好像也没啥功能，只是个图片浏览器。简单的事件驱动就够了。

### 3.4 代码规范
&emsp;&emsp;目前还在开发阶段，而且是空余时间，没做代码优化逻辑很乱也不是很规范，到处抄的代码，命名也一塌糊涂，等想做的功能都实现了在`Reviewer`。
### 3.5 Modality支持
&emsp;&emsp;目前适配的**Modality**包括**CT、XA、IVUS、OCT、US**，其余的可以浏览影像，但是注释等设置和交互参数只能为默认值。这个已经封装成`XML`配置，后续慢慢加。


---
## 4 开发计划

* **打开数据**  
* [x] 打开 dcm 文件  
* [x] 打开 dcm 文件夹  
* [ ] 打开 dcmzip 文件  
* **导出数据**  
* [x] 导出当前图像 jpeg bmp ...
* [x] 导出图片到剪贴板
* [ ] 导出当前序列 pngs
* [ ] 导出当前序列 mp4
* **显示ui**
* [x] 屏幕分割显示布局
* [x] 序列预览条
* [x] 全屏
* [x] 显示注释
* [x] 显示鼠标位置和灰度
* [x] 显示窗位窗宽
* [x] 显示比例尺
* [x] 设置显示字体
* [x] 显示所有dicom标签
* **图片浏览器交互**
* [x] 浏览序列
* [x] 调整窗位窗宽
* [x] 移动图像
* [x] 缩放图像
* [x] 放大图像
* [x] 局部放大图像
* [x] 框选ROI 自适应窗位窗宽
* [x] 自适应放大图像
* [x] 100/200/400放大图像
* [x] 测量长度/角度/矩形面积/椭圆面积/添加描述文字
* [x] 播放模式
* [x] 旋转
* [x] 翻转
* [ ] 差值测量 
* [ ] 脊柱侧弯测量
* **插件** 
* [ ] 多平面重建(MPR)
* [ ] 三维重建(VR 体渲染)
* [x] 图像前处理（锐化、平滑、提取边缘、emboss滤波）
* [ ] 图像融合
* **数据库**
* [x] 一个小型单机本地数据库（使用不舒服，后续重做）
* [x] 数据库浏览器（使用不舒服，后续重做）
* **PACS** 
* [x] 提供一个小型的pacs scp
* [ ] 提供一个小型的pacs scu（未做）

## 4 实现方法
### 4.1 显示ui --- 屏幕分割显示布局
&emsp;&emsp;[Qt 实现 屏幕分割显示布局，可以任意拖拽显示](https://beondxin.blog.csdn.net/article/details/108679798)

### 4.2 Dicom数据结构 ---- DicomImage --> QSharedData
&emsp;&emsp;[把DicomImage封装成 QSharedData 使用 （显式共享）](https://beondxin.blog.csdn.net/article/details/108680479)

&emsp;&emsp; 把**DicomImage**封装成 **QSharedData** ，让后利用**Qt**实现可视化。   
&emsp;&emsp; &emsp;&emsp; ├── PATIENT （病人）    
&emsp;&emsp; &emsp;&emsp; │ │ └── STUDY （检查）    
&emsp;&emsp; &emsp;&emsp; │ │ │ │ └── SERIES （序列）  
&emsp;&emsp; &emsp;&emsp; │ │ │ │ │ │ └── IMAGE （影像 高度为未知）    
&emsp;&emsp; &emsp;&emsp; │ │ │ │ └── SERIES （序列）    
&emsp;&emsp; &emsp;&emsp; │ │ │ │ │ │ └── IMAGE （影像 高度为1）  
&emsp;&emsp; &emsp;&emsp; │ │ │ │ │ │ └── IMAGE （影像 高度为1）  
&emsp;&emsp; &emsp;&emsp; │ │ └── STUDY （检查）  
&emsp;&emsp; &emsp;&emsp; │ │ │ │ └── SERIES （序列）  
&emsp;&emsp; &emsp;&emsp; │ │ │ │ │ │ └── IMAGE （影像）  

### 4.3 Dicom数据结构 ---- DicomImage --> Series
&emsp;&emsp;[Dicom数据结构 ---- DicomImage --＞ SeriesInstance](https://beondxin.blog.csdn.net/article/details/108681041)

&emsp;&emsp; 根据协议可以知道每个**Series**中可以有单帧或多帧。  
&emsp;&emsp; 多帧时 每帧影像尺寸高度是1  
&emsp;&emsp; 单帧时 每帧影像尺寸高度未知   
- 单帧模式高度代表当前时间 !
- 多帧模式高度代表空间位置 !

```cpp
    enum  SeriesPattern {
        Empty_Frame,  //
        Single_Frame, // 单帧
        Multi_Frame,  // 多帧
    };
```
&emsp;&emsp; 涉及到Series，2D可视化肯定会有方向
&emsp;&emsp;（多帧模式下）区分平面
&emsp;&emsp;（单帧模式下）只有XY平面显示模式，另外两个平面表示**时间密度曲线**，与其相关打算作为插件用**opencv**做，所以这里封装的**Series**其余两个平面均指多帧模式。
### 4.4 打开数据 ---- 打开 dcm 文件、打开 dcm 文件夹
&emsp;&emsp;[Qt 实现 打开 文件/文件夹 同一个接口](https://beondxin.blog.csdn.net/article/details/108681757)  
&emsp;&emsp;[Qt 实现一个批量加载线程（单线程）](https://beondxin.blog.csdn.net/article/details/108681908)  
&emsp;&emsp;[Qt 实现一个 文件监听线程](https://beondxin.blog.csdn.net/article/details/108682045)  
如何打开一个**dcm影像**：  
&emsp;&emsp;[把DicomImage封装成 QSharedData 使用 （显式共享）](https://beondxin.blog.csdn.net/article/details/108680479) **2.4 DcmFileFormat 获取 常用标签/图片**  
### 4.5 图片浏览器交互 ---- 框选ROI 自适应窗位窗宽  
&emsp;&emsp;[Qt 实现 RubberBandDrag 框选](https://beondxin.blog.csdn.net/article/details/108682306)  
### 4.6 图片浏览器交互 ---- 浏览序列
&emsp;&emsp;[QGraphicsView 按住鼠标实现帧数切换](https://beondxin.blog.csdn.net/article/details/108682720)  
### 4.7 Dcmtk Pacs 开发：小型dicom数据库（sqlite）
&emsp;&emsp;[Dcmtk Pacs 开发：小型dicom数据库（sqlite）](https://blog.csdn.net/a15005784320/article/details/109058579)  
### 4.8 Dcmtk Pacs 开发：StoreScp 实现
&emsp;&emsp; [Dcmtk Pacs 开发：StoreScp 实现](https://blog.csdn.net/a15005784320/article/details/109058249)  
### 4.9 Dcmtk Pacs 开发：Echo 实现
&emsp;&emsp; [Dcmtk Pacs 开发：Echo 实现](https://blog.csdn.net/a15005784320/article/details/109057800)  
### 4.10 DCMTK 遍历 DICOM 标签/Tags 并 可视化
&emsp;&emsp; [DCMTK 遍历 DICOM 标签/Tags 并 可视化](https://beondxin.blog.csdn.net/article/details/113005123)
### 4.11 利用QTimer实现 Dicom 播放功能
&emsp;&emsp; [利用QTimer实现 Dicom 播放功能](https://beondxin.blog.csdn.net/article/details/113063185)
### 4.12 KISS Dicom Viewer：插件实现方法
&emsp;&emsp; [KISS Dicom Viewer：插件实现方法](https://beondxin.blog.csdn.net/article/details/113620696)
### 4.13 仿照小蚂蚁实现 sharpen smooth Edge Emboss
&emsp;&emsp; [仿照小蚂蚁实现 sharpen smooth Edge Emboss](https://beondxin.blog.csdn.net/article/details/113620255)

### 4.x 慢慢完善...

1. 优化前处理方法