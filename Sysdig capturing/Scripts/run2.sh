rm -rf *.out
rm -rf ../output

input=("youtube480"
	   "youtube720"
	   "youtube1080"
	   "smplayer-cctv-480"
	   "smplayer-cctv-720"
	   "smplayer-cctv-1080"
	   "web-utd"
	   "web-vbaidu-163"
	   "gimp"
	   "pixlr-online"
	   "librewriter")
procname=("firefox"
		  "firefox"
		  "firefox"
		  "mpv"
		  "mpv"
		  "mpv"
		  "firefox"
		  "firefox"
		  "gimp"
		  "firefox"
		  "soffice.bin")
command=("fdbytes_by proc.name"
	     "fdbytes_by_new_read proc.name"
		 "fdbytes_by_new_write proc.name"
		 "iobytes_new"
		 "iobytes_file_new"
		 "iobytes_net_new"
		 "topconns"
		 "topfiles_bytes"
		 "topfiles_bytes_read"
		 "topfiles_bytes_write"
		 "topports_server"
		 "topprocs_cpu" 
		 "topprocs_file"
		 "topprocs_net"
		 "topscalls")



len1=${#input[*]}
len2=${#command[*]}

#:<<!
for((i=0;i<len2;i++))
do
	output="${command[i]}.out"
    output=`echo "$output" | sed s/[[:space:]]//g`	
	echo -e "${command[i]}:\n" >> $output
	
	for((j=0;j<len1;j++))
	do
		input_scap="${input[j]}.scap"
		
		echo "${input[j]}:" >> $output
	
		sudo sysdig -r $input_scap -c ${command[i]} >> $output	

		echo -e "-------------------------\n" >> $output	
	done
done
#!

output="${command[len2-1]}_proc.out"
output=`echo "$output" | sed s/[[:space:]]//g`
echo -e "${command[len2-1]}_proc:\n" >> $output
	
for((j=0;j<len1;j++))
do
	input_scap="${input[j]}.scap"

	echo "${input[j]} proc.name=${procname[j]}:" >> $output

	sudo sysdig -r $input_scap -c ${command[len2-1]} proc.name=${procname[j]}>> $output	

	echo -e "-------------------------\n" >> $output	
done


mkdir ../output
mv *.out ../output/
