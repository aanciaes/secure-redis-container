#!/bin/bash

package='Record Docker Stats'

# Variables
container_id=;
output_file=;
interval=5
samples=1
command='docker stats --no-stream --format "{{.ID}},{{.Name}},{{.CPUPerc}},{{.MemPerc}},{{.MemUsage}},{{.NetIO}},{{.BlockIO}},{{.PIDs}}"'

run_init_assertions() {
	if [ -z "$container_id" ]; then
		print_usage ;
	else
		command+=" $container_id"
	fi 

	if [ -z "$output_file" ]; then
		output_file="./$container_id-stats.csv"
	fi

	if [ ! -f "${output_file}" ] || [ ! -s "${output_file}" ]; then 
		echo "container_id,container_name,cpu_perc,mem_perc,mem_usage,net_io,block_io,pids;timestamp" > "$output_file"
	fi

	if [ $samples == -1 ]; then 
		echo "--- Recording infinite samples with $interval seconds between them ---"
	else
		echo "--- Recording $samples samples with $interval seconds between them ---"
	fi
}

print_usage() {
  echo "$package"
  echo " "
  echo "usage: $ docker-stats-recorder -c <container_id> [options]"
  echo " "
  echo "options:"
  echo "-h                      Show help (this screen)"
  echo "-c <container_id>       Indicate container to record"
  echo "-o <output_name>        Indicate where to store stats"
  echo "-f <csv>     	        Indicate the format of stored stats"
  echo "-i <interval seconds>   Interval between samples"
  echo "-s <number of samples>  Number of samples to recover. Set -1 to infinite"

  exit 0
}

while getopts c:o:i:s: option; do
	case "${option}" in
		h) print_usage ;;
		c) container_id=${OPTARG};;
		o) output_file=${OPTARG};;
		i) interval=${OPTARG};;
		s) samples=${OPTARG};;
		*) print_usage ;;
	esac
done

run_init_assertions ;

index=0;
while [ $index -lt $samples ] || [ $samples == -1 ]; do
	echo "Recording a sample..."

	timestamp=$(date +%s%N)
	output=$(eval $command)
	output+=",$timestamp"
	
	echo "$output" >> "$output_file"

	((index++))

	if [ $index -lt $samples ] || [ $samples == -1 ]; then
		sleep "$interval"
	fi
done
