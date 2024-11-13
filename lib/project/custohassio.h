// Function to insert customized HASSIO Configuration actions

void custo_hassio_disc(){
    for (size_t i = 0; i < carro_max; i++) {
        hassio_device("car_" + carro[i].car_id, String(carro[i].car_id + ChipID));
        config_entity("binary_sensor","Presence", "Presence", carro[i].car_id, mqtt_pathCars[i]);
        config_entity("sensor","Battery", "Battery", carro[i].car_id, mqtt_pathCars[i]);
        config_entity("sensor","signal_strength", "RSSI", carro[i].car_id, mqtt_pathCars[i]);
    }
    config_trigger("Button_1");
}

void custo_hassio_del(){
    for (size_t i = 0; i < carro_max; i++) {
        hassio_device("car_" + carro[i].car_id, String(carro[i].car_id + ChipID));
        delete_entity("binary_sensor","Presence", "Presence", carro[i].car_id);
        delete_entity("sensor","Battery", "Battery", carro[i].car_id);
        delete_entity("sensor","signal_strength", "RSSI", carro[i].car_id);
    }
    delete_trigger("Button_1");
}

void custo_hassio_attr(){
    yield();
//    send_switch_attributes("Switch");         // NOT Used!!
}
void trigger_syncme() {
    yield();
/*
    String mqtt_pathhassio = String(discovery_prefix) + "/";

    attributes_doc.clear();
    attributes_doc["ChipID"]                = ChipID;                               // Chip identifier
    attributes_doc["DeviceName"]            = config.DeviceName;                    // Device friendly name
    attributes_doc["src_Param"]             = "LEVEL";                              // Source Parameter
    attributes_doc["Component"]             = "cover";                              // component class
    attributes_doc["Attribute"]             = "current_position";                   // entity attribute
    attributes_doc["dst_Param"]             = "Position";                           // Destination Parameter

    serializeJson(attributes_doc, attributes_jsonString);                           //Serialize JSON data to string
    //telnet_println("Trigger SyncMe: " + String(attributes_jsonString));
    mqtt_publish(mqtt_pathhassio, "SyncMe", String(attributes_jsonString));
*/
}

void config_backup() {
    config_doc.clear();
    config_doc["Location"]  = String(config.Location);
/*
    config_doc["LOWER_Pos"] = config.LOWER_LEVEL;
    config_doc["UPPER_Pos"] = config.UPPER_LEVEL;
*/
    serializeJson(config_doc, config_jsonString);                           //Serialize JSON data to string
    //telnet_println("Backup string: " + String(config_jsonString));
    mqtt_publish(mqtt_pathconf, "BckpRstr", String(config_jsonString), true);
}

