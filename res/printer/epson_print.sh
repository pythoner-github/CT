#!/bin/sh
# 脚本使用格式
# epson_print.sh Tool_path Print_device PPD_file Image_name [Copies] [Options] 

#TOOLPATH=/mnt/harddisk/emp

if [ "$#" -lt "4" ]
then
    echo "Error: Missing Parameter!"
    echo "use : epson_print.sh Tool_path Print_device PPD_file Image_name [Copies] [Options]"
    echo "exit"
    exit
fi

if [ ! -e "$1/imagetoraster" -o ! -e "$1/rastertoescpx" ]
then 
    echo "Error: Missing Dependence tools named imagetoraster and rastertopclx"
    echo "exit"
    exit
else
    TOOLPATH=$1
fi

if [ ! -e "$2" ]
then
    echo "Error: Printer device does not exist."
    echo "exit"
    exit
fi

if [ -e "$TOOLPATH/ppd/$3" ]
then
    export PPD=$TOOLPATH/ppd/$3
else
    echo "PPD file does not exist."
    exit
fi

if [ ! -e "$4" ]
then
    echo "Error: Image file does not exist."
    echo "exit"
    exit
fi

if [ -n "$5" ]
then
    COPIES=$5
else
    COPIES="1"
fi

if [ -n "$6" ]
then
    OPTIONS=$6
else
    OPTIONS="NULL"
fi

if [ $USERNAME != "root" ]
then
    echo "Must be root user!"
    exit
fi

# cups version 1.2.7
# cupsddk version 1.2.3

# imagetoraster job-id user title copies options [file]
# rastertopclx  job-id user title copies options [file]
# option format: "name0=value1 name1=value1 ..."

# option 具体参数说明
# gamma(integer(1:10000)) gamma校正,默认值为1000,不进行校正.
# brightness(integer(0:200)) 默认值为100,正常情况下为100,200为双倍亮度,50为正常亮度的一半
# ppi (integer(1:MAX)) 指定图片文件的分辨率,以每英尺的像素点来指定,即pixel per inch,
#                      默认值包含在图片文件中的ppi属性,如果图片文件中没有包含ppi,默认值为128
# scaling(integer(1~1000)) 设置一个图片的缩放比例.
#                           没有指定默认值,100指定图片大小以100%打印在页面上.
#                           如果设定了scaling的值,它将会覆盖掉ppi的设置
# position(center,top,left,right,top-left,top-right,bottom,bottom-left,bottom-right)
#          指定图片在打印纸上的位置
# saturation(integer(0~200)) 打印图片的时候指定颜色的饱和度,默认值为100
# hue(integer(-180~180)) 打印图片的时候指定颜色的色调,默认值为0

$TOOLPATH/imagetoraster 1 Emperor image $COPIES "$OPTIONS" $4 | $TOOLPATH/rastertoescpx 1 Emperor image 1 NULL | cat > $2
