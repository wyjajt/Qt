##ReadMe
#
#
#此文件是global.pri文件的demo版本
#
#global.pri文件用于定义 各用户工程的绝对路径
#
#每个用户使用自己的global.pri文件, 不要向SVN库上传
#
#使用时, 需要将global.pri文件中的
#	 ProPATH = D:/SVN/MachineVision 修改为当前用户的工程路径
#	 例如 ：ProPATH = F:/PROG_All_Project/project
#
#引用方法：
#在各个工程的PRO文件中加入
#         include(../../global.pri)
#
#
#################################################################

###新软件目录需要写到绝对路径trunk目录为止

ProPATH = $${PWD}/../../svn/MachineVision_3.1/trunk
MS_VERSION = VS2015
