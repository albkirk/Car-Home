// Function to insert customized MQTT Configuration actions

void custom_mqtt(String command, String cmd_value) {

  if ( command == "Info" && bool(cmd_value.toInt())) {
    //status_report();
    if (BattPowered) mqtt_publish(mqtt_pathtele, "Battery", String(getBattLevel(),0));
    mqtt_publish(mqtt_pathtele, "NTP_Sync", String(NTP_Sync));
    mqtt_publish(mqtt_pathtele, "DateTime", String(curDateTime()));
    hassio_attributes();
    update_presence();
    //ambient_data();
  }
  if ( command == "Config" && bool(cmd_value.toInt())) {
    mqtt_publish(mqtt_pathtele, "OTA", String(config.OTA));
    mqtt_publish(mqtt_pathtele, "TELNET", String(config.TELNET));
    mqtt_publish(mqtt_pathtele, "WEB", String(config.WEB));
    mqtt_publish(mqtt_pathtele, "Interval", String(config.SLEEPTime));   
    mqtt_publish(mqtt_pathtele, "Temp_Corr", String(config.Temp_Corr));
    mqtt_publish(mqtt_pathtele, "LDO_Corr", String(config.LDO_Corr));
  }

}

void custom_connect(){
//    mqtt_dump_data(mqtt_pathtele, "Telemetry");
    update_presence();
}
