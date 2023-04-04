# StardrewAutoFishing
A Stardrew Valley fish bot, under development.

At the current stage, detection of fish position and fishing bar position is finished (there is a bug when treasure is present) 
as well as the action part (i.e. click to move the fishing bar).

A primitive controller is implemented. It is able to catch some easy fish perfectly and some middle hard fishes.

You can already try it out.

Next step is to fix the treasure bug.

# Usage
Prerequisites:
1. Python >= 3.6 (only tested on 3.10.7 should work for all versions after 3.6)
2. Windows10

Steps:
1. Download 'autoFishing.zip' at 'https://github.com/Lixian-Zhang/StardrewAutoFishing/releases/tag/v0.1.0'
2. Unzip it
3. Open a windows terminal (cmd or powershell)
4. Change to the directory where you unzip the files using `cd <your directory>`, e.g.
`cd C:\Users\some_user_name\Desktop\autoFishing`
5. Verify that you are successful with step 4 using `dir`, and you should see some outputs corresponding to files unzipped.
6. Run: `pip install -e .` (be careful not to miss the period at the end)
What this command does is to install a python package from the current directory. You can run `pip uninstall autoFishing` to undo this step.
7. Run the game.
8. **Change both two zoom levels to the minimum (75%) and change the display mode to windowed then resize the game window to the minimum**.
9. Run: `python tests\test_Agent.py` you should see 'Game window detected.' being printed out.
10. Catch some fish!

# 使用说明
系统要求：
1. Python版本>=3.6
2. Windows10
操作步骤：
1. 在‘https://github.com/Lixian-Zhang/StardrewAutoFishing/releases/tag/v0.1.0’下载文件‘autoFishing.zip’。
2. 解压缩到你喜欢的地方。
3. 打开一个终端（cmd或者powershell）
4. 用`cd <你的目录>`命令切换到你解压文件的目录，例如`cd C:\Users\some_user_name\Desktop\autoFishing`。
5. 用`dir`命令验证第4步正确执行。你会看到刚刚解压出来的文件名被打印出来。
6. 运行`pip install -e .`命令（小心不要漏掉末尾的点）。这一步的作用是把当前目录当作一个python包安装到你电脑里。运行`pip uninstall autoFishing`命令可以把它卸载掉。
7. 运行游戏。
8. **把两种缩放级别都调到最小，并且切换到窗口化模式。**
9. 运行`python tests\test_Agent.py`命令，成功的话会看到'Game window detected.'被打印出来。
10. 开始捕鱼！