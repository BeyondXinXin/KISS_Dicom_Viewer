
&emsp;&emsp;开发中......  

详细介绍：[KISS Dicom Viewer : 一个简单的 DICOM 浏览器](https://beondxin.blog.csdn.net/article/details/108678403)  
项目地址：[GitHub](https://github.com/BeyondXinXin/KISS-Dicom-Viewer)

---

# kissDicomViewer
## Demo展示
![](https://raw.githubusercontent.com/BeyondXinXin/BeyondXinXIn/main/PixX/1.5rf17p9kj0o0.gif#pic_center)
![](https://img-blog.csdnimg.cn/20210207220052527.gif#pic_center)
![](https://img-blog.csdnimg.cn/20210207215731176.gif#pic_center)
![](https://img-blog.csdnimg.cn/20210207215730987.gif#pic_center)
![](https://img-blog.csdnimg.cn/20210207215730938.gif#pic_center)
![](https://img-blog.csdnimg.cn/20210207215730626.gif#pic_center)
![](https://img-blog.csdnimg.cn/20210207215730494.gif#pic_center)
![](https://img-blog.csdnimg.cn/2021020721574924.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2ExNTAwNTc4NDMyMA==,size_16,color_FFFFFF,t_70#pic_center)
![](https://img-blog.csdnimg.cn/20210207215756662.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2ExNTAwNTc4NDMyMA==,size_16,color_FFFFFF,t_70#pic_center)
![](https://img-blog.csdnimg.cn/2021020721580197.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2ExNTAwNTc4NDMyMA==,size_16,color_FFFFFF,t_70#pic_center)
![](https://img-blog.csdnimg.cn/20210207215815761.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2ExNTAwNTc4NDMyMA==,size_16,color_FFFFFF,t_70#pic_center)
![](https://img-blog.csdnimg.cn/20210207215819503.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2ExNTAwNTc4NDMyMA==,size_16,color_FFFFFF,t_70#pic_center)

---
## 开发计划

* **打开数据**  
* [x] 打开 dcm 文件 文件夹  
* [x] 打开 dcmzip 文件
* **导出数据**  
* [x] 导出当前显示图像/当前选择序列 jpeg bmp dicom tiff pnm png   
* [x] 复制当前图片到剪贴板  
* **显示ui**
* [x] 屏幕分割显示布局  序列预览条
* [x] 显示注释 鼠标位置和灰度
* [x] 显示窗位窗宽 比例尺 字体可修改
* [x] 显示所有dicom标签
* **图片浏览器交互**
* [x] 浏览序列 播放模式 旋转 翻转
* [x] 调整窗位窗宽 框选ROI自适应窗位窗宽
* [x] 移动图像 缩放图像 局部放大图像 自适应放大图像 指定倍率放大图像 
* [x] 测量长度/角度/矩形面积/椭圆面积/添加描述文字
* **插件** 
* [x] 多平面重建(MPR)
* [x] 三维重建(VR)
* [x] 曲面重建(CPR)
* [x] 图像前处理（锐化、平滑、提取边缘、emboss滤波）
* **数据库**
* [x] 一个小型单机本地数据库（使用不舒服，后续重做）
* [x] 数据库浏览器（使用不舒服，后续重做）
* **PACS** 
* [x] 提供一个小型的pacs scp
