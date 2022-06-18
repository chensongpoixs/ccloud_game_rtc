#!/bin/bash

PROJECT=`pwd`

PROTC_EXE=/usr/local/bin/protoc

NEW_PROTO_DIR=$PROJECT/../Shared/protoc/proto
NEW_PROTO_OUT_DIR=$PROJECT/../Shared/protoc
NEW_PROTO_CLINET_DIR=$PROJECT/../Shared/protoc/proto/client_msg
NEW_PROTO_SERVER_DIR=$PROJECT/../Shared/protoc/proto/server_msg

# 1 client 2. server 3 . 
msg_url=$1
msg_file_name=$2
test_error=0

function compile_proto()
{
	proto_file=$1
	proto_out_dir=$2
	
	
	if [ ! -d $proto_file ]; then
		echo "not find dir $proto_file";
        	return;
	fi
	
	if [ ! -d $proto_out_dir ]; then
		echo "not find dir $proto_out_dir";
        	return;
	fi
	
	#echo $proto_file
	#echo $proto_out_dir
	cd $proto_file
	
	#for fileproto in $(ls *.proto);
	#do
		echo "compile ${msg_file_name} " #copy_cc
		${PROTC_EXE} --cpp_out=${proto_out_dir}  ${msg_file_name}
		#t_diff $(basename $fileproto .proto).pb.h    
		#t_diff $(basename $fileproto .proto).pb.cc
	#done
}




function check_argv()
{
	if [ ! $msg_url ]; then
		echo "proto url= $msg_url";
		return;
	fi
	
	if [ ! $msg_file_name ]; then
		echo "proto name ";
		return;
	fi
	
	
	
	if [ $msg_url -eq 1 ]; then
		#echo "url = $msg_url";
		compile_proto  $NEW_PROTO_CLINET_DIR $NEW_PROTO_OUT_DIR  $msg_file_name
	elif  [ $msg_url -eq 2 ]; then 
		#echo "url = $msg_url";
		compile_proto  $NEW_PROTO_SERVER_DIR $NEW_PROTO_OUT_DIR  $msg_file_name
	elif  [ $msg_url -eq 3 ]; then
		compile_proto  $NEW_PROTO_DIR $NEW_PROTO_OUT_DIR  $msg_file_name
	else
		echo "proto url error ${msg_url}";
	fi 
	
	
	
}

check_argv


