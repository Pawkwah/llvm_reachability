#!/bin/bash

PROJECTHOME=$(pwd)
LLVMHome="llvm-10.0.0.obj"
install_path=/home/SVF-tools
export LLVM_DIR=$install_path/SVF/$LLVMHome
export PATH=$LLVM_DIR/bin:$PATH
export PATH=$PROJECTHOME/bin:$PATH
export SVF_DIR=$install_path/SVF/

echo "LLVM_DIR="$LLVM_DIR
echo "SVF_DIR="$SVF_DIR
