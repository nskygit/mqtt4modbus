# mqtt4modbus
支持通过mqtt对modbus tcp/rtu从机, 进行管理(get/put/save value, get/set/toggle switch)
 ![image](https://github.com/nskygit/mqtt4modbus/raw/master/mqtt4modbus.png)
* 1 支持通过mqtt对modbus从机进行管理
* 2 modbus连接从机支持tcp和rtu
* 3 配置文件支持csv的加载和保存
* 4 支持对modbus的mqtt操作（value: get/put/save/   switch: set/get/toggle）
* 5 其他

## Requirements
* [**Mosquitto**](https://mosquitto.org/) - MQTT broker
* [**libmosquitto**](https://mosquitto.org/man/libmosquitto-3.html) - MQTT client library
* [**libmodbus**](http://libmodbus.org/) - Modbus library
* **GCC + Make** - GNU C Compiler and build automation tool

## Usage
**1 start mosquitto**

    server# mosquitto -d

**2 run mqtt4modbus**

    server# mqtt4modbus &  

**3 run mqtt.fx and subscribe and publish**

**3.1 plc/mqtt4modbus/oper/get**

    [payload] {"identifier":"y410"}  

    [Reply  ] plc/mqtt4modbus/oper/get/ok  
    [payload] {"code":0,"identifier":"y410","address":"0x2089","label":"SV 0NP KOMORA STR","slaveid":17}  

**3.2 plc/mqtt4modbus/oper/get**

    [payload] {"identifier":"all"}  

    [Reply  ] plc/mqtt4modbus/oper/get/ok  
    [payload] {"code":0,"all":[{"identifier":"y402","address":"0x2081","label":"SV 1NP SCHODY","slaveid":17},{"identifier":"y401","address":"0x2080","label":"SV 2NP KUCHYN LINKA","slaveid":17},{"identifier":"y403","address":"0x2082","label":"SV 1NP OBYVAK KRB STR","slaveid":17},{"identifier":"y404","address":"0x2083","label":"SV 1NP OBYVAK KRB KRJ","slaveid":17},{"identifier":"y405","address":"0x2084","label":"SV 2NP LOZNICE KRJ","slaveid":17},{"identifier":"y406","address":"0x2085","label":"SV 2NP LOZNICE STR","slaveid":17},{"identifier":"y407","address":"0x2086","label":"SV 1NP WC","slaveid":17},{"identifier":"y409","address":"0x2088","label":"SV 0NP KOMORA KRJ","slaveid":17},{"identifier":"y410","address":"0x2089","label":"SV {"identifier":"c203","address":"0x40ca","label":"MB ERROR","slaveid":17}]}

**3.3 plc/mqtt4modbus/oper/put**

    [payload] {"identifier":"y410","address":"0x2189","label":"test","slaveid":1}

    [Reply  ] plc/mqtt4modbus/oper/put/ok  
    [payload] {"code":0,"identifier":"y410","address":"0x2189","label":"test","slaveid":1}

**3.4 plc/mqtt4modbus/oper/save**

    [Reply  ] plc/mqtt4modbus/oper/save/ok

**3.5 plc/mqtt4modbus/y410/get**

    [Reply  ] plc/mqtt4modbus/y410/get/ok  
    [payload] false

**3.6 plc/mqtt4modbus/y410/set**

    [payload] true  
    [Reply  ] plc/mqtt4modbus/y410/set/ok

**3.7 plc/mqtt4modbus/y410/toggle**

    [Reply  ] plc/mqtt4modbus/y410/toggle/ok

## Refer
    https://github.com/hubpav/mqtt2modbus ([**Pavel Hübner**]).
