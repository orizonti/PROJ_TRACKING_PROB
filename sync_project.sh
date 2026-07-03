#!/bin/bash
#SYNC FROM REMOTE TO WORK_DIR
SOURCE_DIR=$(pwd)
DEST_DIR=$1

echo "=================================="
echo "SYNCRONIZE SOURCE:$SOURCE_DIR  DESTINATION:$DEST_DIR"
echo "PARAM: -uv -pugD --recursive"
echo "=================================="

#rsync -uv --exclude main.cpp\
#          --exclude controller_process_class.cpp\
#          --exclude controller_process_class.h\
#          --exclude SETTINGS_PATH.h\
#          --exclude /AIMING_MODULES/\
#          --exclude /build/\
#          --exclude /dump_dir/\
#          --exclude /.git/\
#          --recursive $SOURCE_DIR $DEST_DIR; 

rsync -uv -p -u -g -D --recursive $SOURCE_DIR --exclude SETTINGS_PATH.h\
                                  --exclude CMakeLists.txt\
                                  --exclude dump_dir\
                                  --exclude build\
                                  --exclude .git\
                                  --exclude .cache\
                                  --exclude CMakeFiles\
                                  --recursive $SOURCE_DIR $DEST_DIR; 

echo "=================================="
echo "END"

