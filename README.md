 
用C语言实现只使用CPU进行计算的3D渲染，不用OpenGL，不用GPU。

代码来自于学习pikuma.com的课程“3D Graphics From Scratch”。

## Back-face culling

## Clipping
### 直观理解计算线和平面相交点的算法

![](./assets/images/intersection_of_line_and_plane.png)

### 裁剪三角形的算法

![](./assets/images/clipping_triangle_by_right_and_bottom_plane.png)

设在平面以内的点的数组是array。

三角形abc，先被右侧平面裁剪。

线段ca和右侧平面相交于点d，把点d添加到array。

点a在右侧平面内，把点a添加到array。

线段ab和右侧平面不相交，不添加相交点。

点b在右侧平面内，把点b添加到数组array。

线段bc和右侧平面相交于点e，把点e添加到array。

点c在右侧平面外，不添加点c。

此时数组array中按顺序有点d,a,b,e，组成多边形dabe。

按同样的算法，用下侧平面依次裁剪线段ed，da，ab，be，得到多边形dfgbe。

然后把多边形dfgbe中每三个点构成一个三角形，图中的三角形应该是以g为起点构造的，得到3个三角形gbe，ged，gdf。

