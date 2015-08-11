#!/bin/bash

function usage() {
	echo "Usage: $0 min_x max_x min_y max_y"
	exit 1
}

#validate ranges
if [ -z "$4" ]; then
	usage
fi
min_x=$(($1))
max_x=$(($2))
min_y=$(($3))
max_y=$(($4))
if [ $min_x -gt $max_x ] || [ $min_x -lt -180 ] || [ $max_x -gt 180 ]; then
	usage
fi
if [ $min_y -gt $max_y ] || [ $min_y -lt -90 ] || [ $max_y -gt 90 ]; then
	usage
fi

#get awscli if we dont have it
aws --version &> /dev/null
if [ $? -ne 0 ]; then
	sudo apt-get install awscli
fi

#pull down the bounding box of tiles you want
for x in $(seq $min_x 1 $max_x); do
	for y in $(seq $min_y 1 $max_y); do
		file=$(python -c "print '%s%02d%s%03d.hgt.gz' % ('S' if $y < 0 else 'N', abs($y), 'W' if $x < 0 else 'E', abs($x))")
		dir=$(echo $file | sed "s/^\([NS][0-9]\{2\}\).*/\1/g")
		aws --region us-east-1 s3 sync s3://mapzen.valhalla/elevation/${dir}/ ./elevation/${dir}/ --exclude "*" --include ${file}
		gunzip ./elevation/${dir}/${file}
	done
done