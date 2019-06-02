#!/bin/sh
PARA=$1
DIRNAME=`dirname $0`
cd $DIRNAME
FULL_PATH=`pwd`
ROOT_PATH=$FULL_PATH/../

#compile muduo
echo "=========start to build muduo======="
MUDUO_DIR=$ROOT_PATH/muduo
cd $MUDUO_DIR && sh build.sh $PARA
if [ "$?" != "0" ]; then
  echo "muduo build failed, exit"
  exit 1
fi

#compile public
echo "=========start to build public======="
PUBLIC_DIR=$ROOT_PATH/public
cd $MUDUO_DIR && sh build.sh $PARA
if [ "$?" != "0" ]; then
  echo "public build failed, exit"
  exit 1
fi

#compile sender
echo "=========start to build sender======="
SENDER_DIR=$ROOT_PATH/sender
cd $SENDER_DIR && sh build.sh $PARA
if [ "$?" != "0" ]; then
  echo "sender build failed, exit"
  exit 1
fi

