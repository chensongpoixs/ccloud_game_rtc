#!/bin/bash
# project
#bin_names=( cpgcommmserver cpggatewayserver cpgloginserver cpgcentralserver cpgmatchserver cpgwsgatewayserver cpglogserver)

dst_dir=linux

bin=build

dst_bin=`pwd`

DIFF_EXE=$dst_bin/../protocol/Tools/diff



version_filename=last_build_version

md5_filename=md5list.txt

src_bin=$dst_bin

#echo $src_bin

src_sd=$src_bin/sd


#sum_cpu=1
#sum_cpu=1


option_make=0

function show_useage()
{
	echo Usage: [options]	
	echo
	echo options:
	echo "-h ,         help"
	echo "-m ,         1 make, 0 not make, default 0"
	echo "-c ,         make clean "
}



function copy_run_bin()
{
	for name in ${bin_names[@]}
	do
		cp $src_bin/$name $src_sd/$name
		
	done
}


#function t_system_info()
#{
#	#$sum_cpu=/proc/cpuinfo| grep "processor"| wc -l
#}


function t_make_clean()
{
	echo $dst_bin
	
	if [ -d $dst_bin/../build ]; then
		cd $dst_bin/../build  
		make clean
	fi
	echo "make clean  $dst_bin"/../build
}

function parse_args
{
	for src_arg in "$@"
	do
		local arg=$src_arg
		
		local arr=($(echo $src_arg|tr '=' ' '));
	
		if [ ${#arr[@]} == 2 ]; then
			arg=${arr[0]}
		fi
		arg=${arg:1}
		case $arg in
			m)
				option_make=1	
				;;
			h)
				show_useage
				exit 0
				;;
			c)	
				t_make_clean
				exit 0
				;;
			?)  #当有不认识的选项的时候arg为?
				echo "unkonw argument"
				exit 1
				;;
		esac
	done
}



function f_make
{
	echo $option_make
	if [ $option_make == 0 ]; then
		return
	fi
	
	
	# version
	#local new_version=`git log --oneline | wc -l`
	
	
	#if [ ! -f $version_filename ]; then
	#	old_version=0
	#else
	#	old_version=`cat $version_filename`
	#fi
	#
	#
	#if [ "$old_version" == "$new_version" ]; then
	#    return
	#fi
	
	#dir

	cd ..
	
	if [ ! -d $bin ]; then
        	mkdir $bin
	fi

	
	cd $bin 
	
	# cmake .
	cmake ..


	# make 
	make   -j  4

#	echo $src_sd
#	if [ ! -e $src_sd ]; then
#		mkdir $src_sd
#	fi

#	cd $src_bin
#	:>$md5_filename
#	for name in ${bin_names[@]}
#	do
#		objcopy --strip-debug $src_bin/$name $src_sd/$name
#		md5sum ${name}>>$src_bin/$md5_filename
#	done
	
}


parse_args "$@"

#t_system_info

f_make

