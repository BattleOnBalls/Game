# Game
## 项目选题：<br>
Agar.io<br>
<br>
## 组员:<br>
HYH 1753255:100%<br>
CSD 1651708:100%<br>
CTT 1751062:100%<br>
<br>
## 项目基础功能：<br>
支持图形显示`√`<br>
支持鼠标键盘操作`√`<br>
支持agar.io中的吞噬操作`√`<br>
支持agar.io中的分裂操作`√`<br>
支持动画`√`<br>
实现服务端：支持局域网联机对战（自由模式）<br>
(尝试过，但是没有成功...参见文件夹class（no）,里面有详细注解)<br>
<br>
## 拓展功能：<br>
单机版有AI出没`√`<br>
支持吐孢子功能`√`<br>
不定时产生刺`√`<br>
支持换肤功能`√`<br>
支持自由输入昵称（英文，大小写不限）`√`<br>
<br>
## 已使用的C++特性：<br>
STL容器，如std::vector,std::map等`√`<br>
类和多态`√`<br>
函数重载`√`<br>
类型推断`√`<br>
迭代器`√`<br>
<br>
## 代码规范：<br>
函数采用小驼峰命名法，参数采用下划线命名法<br>
不同代码块之间空行间隔开<br>
适时附上注释便于理解<br>
尽可能使用按引用传递<br>
使用#define声明常量<br>
<br>
## 游戏运行说明：<br>
### 开始界面  <br> 
![image text](https://github.com/BattleOnBalls/Game/blob/master/enter.png)<br><br>
点击框框输入昵称，再点击“修改”按钮确认修改（一定要点击哦，不然无法开始游戏）<br>
![image text](https://github.com/BattleOnBalls/Game/blob/master/skin.jpg)<br><br>
开始界面右上角有选择“皮肤”的按钮，选择你喜欢的皮肤，不要裸奔喔（裸奔是蜜汁绿色：）嘿嘿）<br>
点击“开始比赛”进入游戏界面<br>
### 游戏界面<br>
![image text](https://github.com/BattleOnBalls/Game/blob/master/game.png)<br><br>
进入游戏后，通过虚拟摇杆控制球球的移动（点击界面摇杆出现，拖动鼠标球球会朝相同方向前进）<br>
点击右下方“闪电”球球会吐孢子（达到一定分值才可吐孢子）<br>
点击右下方“-”球球会分身（只有达到一定分值才能分身，分身数达到一定限额则不能继续分身）<br>
游戏界面会不定时出现绿刺。球球足够大时，碰撞绿刺吃掉绿刺的同时还会分裂（达到分身上限时，仅吃掉不分身）<br>
![image text](https://github.com/BattleOnBalls/Game/blob/master/ai.png)<br><br>
由于服务器存在一个de不出来的bug目前还没有实现联网功能...为了排遣玩家寂寞空虚冷的情绪，我们加入了一个AI...这个AI有点智障，只会吃最近的豆子和在玩家比它大时逃跑，玩家比它小时过来吃玩家（偶尔皮一下自己还会分个身）...但是它速度快！也不是那么好吃到的！ <br>
### 结束界面<br>
![image text](https://github.com/BattleOnBalls/Game/blob/master/over.png)<br><br>
游戏结束后会显示你的最终得分<br>
可以选择“退出”或者“返回大厅”继续游戏<br>
 

   


