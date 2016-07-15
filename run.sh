#!/bin/bash

# File    : run.sh
# Author  : lihan
# Version : 1.0
# Company : xiaojukeji
# Contact : lihan@diditaxi.com.cn
# Date    : 2015-07-23 13:31:46

[ -f output/bin/logger_lib ] && output/bin/logger_lib $@
[ -f output/bin/logger_lib-service ] && output/bin/logger_lib-service $@
