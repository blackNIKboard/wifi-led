export WIFI_SSID="YOUR_SSID"
export WIFI_PASS="YOUR_PASSWORD"

echo -e "{\n\t\"ssid\": \"$WIFI_SSID\",\n\t\"password\": \"$WIFI_PASS\"\n}" > data/config/wifi.json