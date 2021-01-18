# kiss-dicom-viewer

&emsp;&emsp;开源代码托管还有这么多平台，github、gitee、coding.net、codechina，大家不应该是一家嘛。打算试一下CodeChina，接下来1个月用CodeChina作为个人代码管理体验下，好用了接着用。CodeChina界面看了下比较喜欢，而且目前没有一点广告。


---

https://beondxin.blog.csdn.net/article/details/108678403  

&emsp;&emsp;东拼西凑到处copy后kiss-dicom-viewer的Demo9月份做好了，勉强能用。  


## 代码规范
&emsp;&emsp;目前还在开发阶段，而且是空余时间，没做代码优化逻辑很乱也不是很规范，到处抄的代码，命名也一塌糊涂，等都开发稳定了再Review。  
> 如果需要借鉴的话请勿愤～～～

## 已知严重BUG
&emsp;&emsp;公司PACS开发一直没招，让我临时顶替下。这几个月直接把完全没测试过的kiss-dicom-viewer中pacs功能投入到医院临床使用......
&emsp;&emsp;强烈要求下，公司终于同意招人正经的做自己的PACS软件了。我可以回归本意，继续自己业余折腾kiss-dicom-viewer了。  

&emsp;&emsp;临床使用中遇到些尴尬的问题，统一改：  
* 内存泄露，反复打开同一文件夹，线程发送数据存在内存泄露。  
* 不完善标签的DICOM影像在显示时候会有些问题，尤其是SIMPITK自己生成DICOM。  

## 开发进度
&emsp;&emsp;9月做好就没再改过了。代码整理和好多功能还没搞，一放就拖到了年底了。最近刚提出离职，没有什么进度需要赶了，再加上马上过年，由于疫情就留在杭州不回家了。计划最近这段时间和过年期间完善一下kiss-dicom-viewer。计划是这样，要是过年给自己找个别的事估计就继续搁浅了......  

## 平台
&emsp;&emsp;打包后全平台用没问题，开发我使用的：  
win：Qt5.12 vs2017   
ubuntu16：Qt5.11 gcc7.5   
第三方库目前就用了DCMTK3.6.5，做体渲染功能时会增加vtk8.20

win 下效果

![](Doc/Images/001.png)

