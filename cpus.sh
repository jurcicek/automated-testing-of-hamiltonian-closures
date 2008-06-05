
stat_comp()
{
  echo "Host $1" >> cpus.log
  echo "--------------------------------------------------------------------" >> cpus.log
  echo "  PID USER     %CPU %MEM   VSZ     TIME COMMAND" >> cpus.log
  rsh $1 -l filip ps -u filip -wo pid,user,%cpu,%mem,vsize,time,args | grep FJDipl | grep -v grep >> cpus.log 2> /dev/null
  echo "" >> cpus.log
}

echo -e "Lyra CPUs statistic for user filip \n" > cpus.log
stat_comp yacek1
stat_comp yacek2
stat_comp yacek3
stat_comp yacek4
stat_comp yacek5
stat_comp yacek6
stat_comp yacek7
stat_comp yacek8

echo 
cat cpus.log
