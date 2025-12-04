[
    {
        "id": "44ee11ba7421fe1a",
        "type": "tab",
        "label": "ESP32 MQTT Dashboard (Wireless)",
        "disabled": false,
        "info": "Flow updated for MQTT wireless communication.",
        "env": []
    },
    {
        "id": "3ac2ca61d9d30743",
        "type": "mqtt in",
        "z": "44ee11ba7421fe1a",
        "name": "MQTT Sensor Data",
        "topic": "farm/esp32/data",
        "qos": "0",
        "datatype": "utf8",
        "broker": "2ae84f97d8b19242",
        "nl": false,
        "rap": true,
        "rh": 0,
        "inputs": 0,
        "x": 240,
        "y": 280,
        "wires": [
            [
                "cf5e697c7fe8d130"
            ]
        ]
    },
    {
        "id": "cf5e697c7fe8d130",
        "type": "json",
        "z": "44ee11ba7421fe1a",
        "name": "Parse JSON",
        "property": "payload",
        "action": "",
        "pretty": false,
        "x": 420,
        "y": 280,
        "wires": [
            [
                "1e43af24b10bca34",
                "2988860228d3fa69",
                "aa8ccec8ec9db850",
                "b80f3e4b4f5b3c0c",
                "0228510791522aa3"
            ]
        ]
    },
    {
        "id": "0228510791522aa3",
        "type": "ui_text",
        "z": "44ee11ba7421fe1a",
        "group": "e0b57174e92b8d00",
        "order": 5,
        "width": 4,
        "height": 2,
        "name": "Alert Status",
        "label": "Active Alerts",
        "format": "Count: {{payload.alerts}}<br>Safety: {{payload.gas_safe ? '✅ OK' : '🚨 DANGER'}}",
        "x": 620,
        "y": 420,
        "wires": []
    },
    {
        "id": "1e43af24b10bca34",
        "type": "ui_gauge",
        "z": "44ee11ba7421fe1a",
        "name": "Temperature",
        "group": "e0b57174e92b8d00",
        "order": 1,
        "width": 4,
        "height": 4,
        "title": "Temperature (°C)",
        "format": "{{payload.t}}",
        "min": 0,
        "max": 40,
        "colors": [
            "#0000ff",
            "#ffff00",
            "#ff0000"
        ],
        "x": 620,
        "y": 120,
        "wires": []
    },
    {
        "id": "2988860228d3fa69",
        "type": "ui_gauge",
        "z": "44ee11ba7421fe1a",
        "name": "Humidity",
        "group": "e0b57174e92b8d00",
        "order": 2,
        "width": 4,
        "height": 4,
        "title": "Humidity (%RH)",
        "format": "{{payload.h}}",
        "min": 0,
        "max": 100,
        "colors": [
            "#0080ff",
            "#7fff00",
            "#ff0000"
        ],
        "x": 610,
        "y": 180,
        "wires": []
    },
    {
        "id": "aa8ccec8ec9db850",
        "type": "ui_text",
        "z": "44ee11ba7421fe1a",
        "group": "e0b57174e92b8d00",
        "order": 3,
        "width": 4,
        "height": 2,
        "name": "Distance",
        "label": "Distance (cm)",
        "format": "{{payload.d}} cm",
        "x": 610,
        "y": 240,
        "wires": []
    },
    {
        "id": "b80f3e4b4f5b3c0c",
        "type": "ui_text",
        "z": "44ee11ba7421fe1a",
        "group": "e0b57174e92b8d00",
        "order": 4,
        "width": 4,
        "height": 2,
        "name": "Raw Gas Reading",
        "label": "Gas (Raw AO)",
        "format": "{{payload.gasA}}",
        "x": 620,
        "y": 300,
        "wires": []
    },
    {
        "id": "2ae84f97d8b19242",
        "type": "mqtt-broker",
        "name": "HiveMQ Public Broker",
        "broker": "broker.hivemq.com",
        "port": "1883",
        "tls": "",
        "clientid": "NodeRED_Farm_Client",
        "autoConnect": true,
        "usetls": false,
        "protocolVersion": 4,
        "keepalive": "60",
        "cleansession": true,
        "autoUnsubscribe": true,
        "birthQos": "0",
        "willQos": "0",
        "sessionExpiry": ""
    },
    {
        "id": "e0b57174e92b8d00",
        "type": "ui_group",
        "name": "Current Sensor Readings",
        "tab": "c76b4a6d0a794715",
        "order": 1,
        "disp": true,
        "width": "12",
        "collapse": false,
        "className": ""
    },
    {
        "id": "c76b4a6d0a794715",
        "type": "ui_tab",
        "name": "Home Monitor",
        "icon": "dashboard",
        "order": 1,
        "disabled": false,
        "hidden": false
    },
    {
        "id": "d7f993c22abc96a1",
        "type": "global-config",
        "env": [],
        "modules": {
            "node-red-dashboard": "3.6.6"
        }
    }
]
