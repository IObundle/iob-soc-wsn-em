#!/bin/bash
set -e
source /opt/ic_tools/init/init-xcelium20-09-hf001
xmvlog -messages $CFLAGS $VSRC
xmelab -messages $EFLAGS worklib.system_tb:module
xmsim  $SFLAGS worklib.system_tb:module 
