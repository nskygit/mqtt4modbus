# mqtt4modbus
支持通过mqtt对modbus tcp/rtu从机,进行管理(show/save/get/set/toggle)
* 1 支持通过mqtt对modbus从机进行管理
* 2 modbus连接从机支持tcp和rtu
* 3 配置文件支持csv的加载和保存
* 4 支持对modbus的mqtt操作（show/save/set/get/toggle）
* 5 其他

## Requirements
* [**Mosquitto**](https://mosquitto.org/) - MQTT broker
* [**libmosquitto**](https://mosquitto.org/man/libmosquitto-3.html) - MQTT client library
* [**libmodbus**](http://libmodbus.org/) - Modbus library
* **GCC + Make** - GNU C Compiler and build automation tool

## Usage：
* 1 server# mosquitto -d
* 2 server# make; mqtt2modbus &

* 3 client# mqtt.fx 进行mqtt subscribe and publish
* 3.1 plc/mqtt4modbus/oper/get
*     {"identifier":"y410"}
* 3.2 plc/mqtt4modbus/oper/get/ok
*     {"code":0,"identifier":"y410","address":"0x2089","label":"SV 0NP KOMORA STR","slaveid":17}
* 3.3 plc/mqtt4modbus/oper/set
* 3.4 plc/mqtt4modbus/oper/show
* 3.5 plc/mqtt4modbus/oper/save
* 3.6 plc/mqtt4modbus/y410/set
* 3.7 plc/mqtt4modbus/y410/get
* 3.8 plc/mqtt4modbus/y410/toggle

## Thanks
基于原项目https://github.com/hubpav/mqtt2modbus ([**Pavel Hübner**]).
