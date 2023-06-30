# infrared_3D_reconstruction
## 光激励红外三维重构目标检测
此项目通过AGV搭载机械臂以及光激励红外设备，激光导航AGV靠近试件，根据RGBD相机重构三维场景，确定检测试件及其三维模型，根据三维模型划分红外光激励检测设备的运动位姿，控制机械臂运动到指定位姿完成扫查试件；融合高清视觉与红外检测结果，并投影到三维模型中增强显示。<br>
<img src="https://user-images.githubusercontent.com/54426524/163543969-5eef0645-95ee-4053-a975-aaa4c35f2fe9.PNG" height="290px">
<img src="https://user-images.githubusercontent.com/54426524/163543973-1bf3b3b7-81a2-4e2a-b0b4-ae56588ca05e.PNG" height="290px"><br>
## 代码环境:
+ mechanical arm rokae SDK<br>
+ depth camera astra SDK<br>
+ Open3D_v0.10.0<br>
+ OpenCV455<br>

## 代码流程：
+ 标定红外系统、深度相机和机器人的变换关系<br>
+ 采集实验数据，包含红外图像、深度图像、机械臂及AGV当前姿态<br>
+ 人机交互、三维重构、RGBD、HD RGB、Thermal RGB数据融合<br>

## 实验结果
<img src="https://user-images.githubusercontent.com/54426524/163544509-78d6ee3d-0d8c-4aca-bc2f-cca8226b957e.PNG" width="320">
<img src="https://user-images.githubusercontent.com/54426524/163544515-513c394e-4ecb-44a9-9a5b-124f0d90bd2a.PNG" width="320">
<img src="https://user-images.githubusercontent.com/54426524/163544521-eaaed732-175a-4793-acda-e93f60a25552.PNG" width="320"><br>
