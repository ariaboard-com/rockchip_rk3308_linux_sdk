wpa_supplicant -B -i wlan0 -c /data/cfg/wpa_supplicant.conf

amixer cset name='DAC HPOUT Left Volume' 18
amixer cset name='DAC HPOUT Right Volume' 18
amixer cset name='Master Playback Volume' 40

if [ ! -d "/oem/application-necessities" ];then
    mkdir /oem/application-necessities
fi
# WIFI 
# softapServer Rockchip-Echo-123

# Run Demo
# SampleApp /oem/AlexaClientSDKConfig.json /oem/resources/ 
