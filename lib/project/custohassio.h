// Function to insert customized HASSIO Configuration actions

void custo_hassio_disc(){
    for (size_t i = 0; i < carro_max; i++) {
        hassio_device(carro[i].car_id, String(carro[i].car_id + ChipID));
        config_entity("binary_sensor","Presence", "Presence", carro[i].car_id, mqtt_pathCars[i]);
        config_entity("sensor","Battery", "Battery", carro[i].car_id, mqtt_pathCars[i]);
        config_entity("sensor","signal_strength", "RSSI", carro[i].car_id, mqtt_pathCars[i]);
    }
}

void custo_hassio_del(){
    for (size_t i = 0; i < carro_max; i++) {
        hassio_device(carro[i].car_id, String(carro[i].car_id + ChipID));
        delete_entity("binary_sensor","Presence", "Presence", carro[i].car_id);
        delete_entity("sensor","Battery", "Battery", carro[i].car_id);
        delete_entity("sensor","signal_strength", "RSSI", carro[i].car_id);
    }
}

void custo_hassio_attr(){
//    send_switch_attributes("Switch");         // NOT Used!!
}
