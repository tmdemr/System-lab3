#!/bin/bash
rm -f /root/test.log
for i in {1..100}
do
  NOW=`date +%Y-%m-%d\ %H:%M:%S`
  echo "[$NOW] i = $i" >> /root/test.log
  sleep 2
done
echo done...
