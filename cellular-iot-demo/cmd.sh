#!/usr/bin/env bash

cmd=${1}
val=${2}
if [[ -z $val ]]; then
    val=0
fi

if [[ $cmd = "post" ]]; then
    json_data="{ \"cmd\": \"${cmd}\", \"data\": \"${val}\" }"
elif [[ $cmd = "flash" || $cmd = "at" ]]; then
    json_data="{ \"cmd\": \"${cmd}\", \"code\": \"${val}\" }"
else
    json_data="{ \"cmd\": \"${cmd}\", \"val\": ${val} }"
fi

echo $json_data
send_data=$(echo $json_data | base64)
echo $send_data
r=$(curl -s -X POST https://supersim.twilio.com/v1/SmsCommands --data-urlencode "Sim=<YOUR_SUPER_SIM_ID>" --data-urlencode "Payload=${send_data}" -u "<YOUR_ACCOUNT_ID>:<YOUR_AUTH_TOKEN>")

# Exit cleanly
exit 0
