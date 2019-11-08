#This is a bash script used as a cron job to fetch the data and append it to a file for later access.
echo "\"`date +%s`\",`wget -qO- http://192.168.86.32/`" >> /var/services/homes/cjtrowbridge/tempLog.txt
