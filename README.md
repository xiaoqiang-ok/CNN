# CNN

Lenet: a convolutional neural network program for handwritting digit recognition
including both model train(multithread Back-Propagation) and test logic, model saving and loading logic, visual-studio-MFC dialog  based net visualing.

minist(hand-writting digit pictures) data can be archieved from: https://pan.baidu.com/s/1pJsOMqn

this project is built via visual-studio-2012 on PC windows.

after start the project you will get a dialog:
click"查看卷积"
![DialogInterface](https://github.com/xiaoqiang-ok/CNN/blob/master/dialog.jpg)

then click "训练网络"
![DialogInterface](https://github.com/xiaoqiang-ok/CNN/blob/master/how-does-it-looks-like.gif)

after several trainning iterations，you can click“保存网络”
the trained model will be saved in CNN\CNN2\net.txt
you can click"读入网络”to restore a Lenet model.
you can click "识别测试" to check the accuracy of the model trained with 10000 test pics。

