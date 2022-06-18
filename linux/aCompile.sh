#!/bin/bash

PROJECT=`pwd`

PROTC_EXE=/usr/local/bin/protoc

OLD_PROTO_DIR=$PROJECT/../CPGGatewayServer/ClientServer/protocol
OLD_PROTO_OUT_DIR=$PROJECT/../CPGGatewayServer/ClientServer/protocol


NEW_PROTO_DIR=$PROJECT/../Shared/protoc/proto
NEW_PROTO_OUT_DIR=$PROJECT/../Shared/protoc
NEW_PROTO_CLINET_DIR=$PROJECT/../Shared/protoc/proto/client_msg
NEW_PROTO_SERVER_DIR=$PROJECT/../Shared/protoc/proto/server_msg


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
	
	echo $proto_file
	
	cd $proto_file
	
	for fileproto in $(ls *.proto);
	do
		echo "compile ${fileproto} " #copy_cc
		${PROTC_EXE} --cpp_out=${proto_out_dir} ${fileproto}
		#t_diff $(basename $fileproto .proto).pb.h    
		#t_diff $(basename $fileproto .proto).pb.cc
	done
}




compile_proto  $OLD_PROTO_DIR $OLD_PROTO_OUT_DIR

compile_proto  $NEW_PROTO_DIR $NEW_PROTO_OUT_DIR


compile_proto  $NEW_PROTO_CLINET_DIR $NEW_PROTO_OUT_DIR

compile_proto  $NEW_PROTO_SERVER_DIR $NEW_PROTO_OUT_DIR





