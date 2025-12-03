# ♜ TORRES SENTINELA

As **Torres Sentinela** são um protótipo de sistema de monitoramento voltado para a coleta de dados de precipitação (chuvas).

Desenvolvido com base na placa **ESP32**, o projeto tem como principal objetivo alimentar o *Projeto Sentinela* — uma iniciativa maior de monitoramento ambiental. Você pode encontrar informações detalhadas e todo o contexto de aplicação no site oficial do projeto (link do Google Sites).

</br>

> ⚠️ **Atenção:**
> Este repositório contém **apenas a parte de coleta de dados** (hardware + firmware).
> Para visualizar a interface completa em funcionamento e acompanhar os dados em tempo real, acesse o repositório da interface do usuário:
> 👉 **[https://github.com/SofiaValadares/TorreSentinela-Front](https://github.com/SofiaValadares/TorreSentinela-Front)**

</br>

## 👥 Desenvolvedores

<table>
  <tr>
    <td align="center">
      <a href="https://github.com/SofiaValadares">
        <img src="https://avatars.githubusercontent.com/u/113111708?v=4" width="100px;" alt="Foto Sofia Valadares"/><br>
        <sub><b>Sofia Valadares</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/SofiaValadares">
        <img src="https://avatars.githubusercontent.com/u/113111708?v=4" width="100px;" alt="Foto Sofia Valadares"/><br>
        <sub><b>Sofia Valadares</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/guiga-sa">
        <img src="https://avatars.githubusercontent.com/u/123979639?v=4" width="100px;" alt="Foto Guilherme Silveira"/><br>
        <sub><b>Guilherme Silveira</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/SofiaValadares">
        <img src="https://avatars.githubusercontent.com/u/113111708?v=4" width="100px;" alt="Foto Sofia Valadares"/><br>
        <sub><b>Sofia Valadares</b></sub>
      </a>
    </td>
</table>

---

# 🛠️ MONTANDO O PROJETO

Para montar o projeto, é necessário garantir uma série de requisitos de hardware e software.

### **Requisitos de Hardware**

1. Módulo **ESP32** (Wi-Fi + Bluetooth, 30 pinos)
2. Sensor de precipitação

### **Requisitos de Software**

1. **Node-RED**
2. **Visual Studio Code** com a extensão **PlatformIO**

---

## 🔌 Montagem do Circuito

*(Adicione aqui imagens ou diagramas quando possível.)*

---

## 📥 Clonando o Projeto

Crie uma pasta, abra o terminal nela e execute:

```bash
git clone https://github.com/SofiaValadares/torre-sentinela.git
```

---

# 🔴 Node-RED

Para instalar o Node-RED, siga as instruções oficiais em:
👉 [https://nodered.org/docs/getting-started/local](https://nodered.org/docs/getting-started/local)

O guia explica como instalar usando **npm**.

Após concluir a instalação, inicie o Node-RED com:

```bash
node-red --port 1880
```

Assim que o serviço iniciar, a interface estará disponível em:
**[http://127.0.0.1:1880/](http://127.0.0.1:1880/)**

No painel principal, clique no menu “sanduíche” (canto superior esquerdo) e selecione **Importar**.
Em seguida, cole o conteúdo do JSON abaixo:

```json
[
    {
        "id": "fad4f935c49154a2",
        "type": "tab",
        "label": "Fluxo 1",
        "disabled": false,
        "info": "",
        "env": []
    },
    {
        "id": "57ec799374289e18",
        "type": "mqtt in",
        "z": "fad4f935c49154a2",
        "name": "",
        "topic": "/chuva/dados",
        "qos": "0",
        "datatype": "auto-detect",
        "broker": "8035d64024d5cdb0",
        "nl": false,
        "rap": true,
        "rh": 0,
        "inputs": 0,
        "x": 310,
        "y": 220,
        "wires": [
            [
                "c54388cad92ee27d"
            ]
        ]
    },
    {
        "id": "c54388cad92ee27d",
        "type": "json",
        "z": "fad4f935c49154a2",
        "name": "",
        "property": "payload",
        "action": "",
        "pretty": false,
        "x": 590,
        "y": 220,
        "wires": [
            [
                "3147e167e0e41014"
            ]
        ]
    },
    {
        "id": "3147e167e0e41014",
        "type": "function",
        "z": "fad4f935c49154a2",
        "name": "Guardar último dado",
        "func": "if (!msg.payload) {\n    node.warn(\"[GUARDAR] Sem payload, não vou salvar\");\n    return null;\n}\n\nlet data = msg.payload;\n\n// Se ainda for string, tenta converter para objeto JSON\nif (typeof data === \"string\") {\n    node.warn(\"[GUARDAR] Payload é string, tentando JSON.parse: \" + data);\n    try {\n        data = JSON.parse(data);\n    } catch (e) {\n        node.warn(\"[GUARDAR] Falha ao fazer JSON.parse: \" + e.message);\n        return null;\n    }\n}\n\nnode.warn(\"[GUARDAR] Payload após parse: \" + JSON.stringify(data));\n\n// Lê o id\nconst rawId = data.id;\nif (!rawId) {\n    node.warn(\"[GUARDAR] Payload sem id, não vou salvar\");\n    return null;\n}\n\nconst id = String(rawId).trim();\n\nnode.warn(\"[GUARDAR] Salvando dados na chave: chuva_\" + id);\n\n// Salva esse objeto (já parseado) na memória global\nglobal.set(\"chuva_\" + id, data);\n\nreturn null;\n",
        "outputs": 1,
        "timeout": 0,
        "noerr": 0,
        "initialize": "",
        "finalize": "",
        "libs": [],
        "x": 880,
        "y": 220,
        "wires": [
            [
                "4a2ad9e4a1b1d5cf"
            ]
        ]
    },
    {
        "id": "aa97346de0de5815",
        "type": "http in",
        "z": "fad4f935c49154a2",
        "name": "",
        "url": "/chuva/:id",
        "method": "get",
        "upload": false,
        "skipBodyParsing": false,
        "swaggerDoc": "",
        "x": 220,
        "y": 480,
        "wires": [
            [
                "eca71629d3caa645"
            ]
        ]
    },
    {
        "id": "eca71629d3caa645",
        "type": "function",
        "z": "fad4f935c49154a2",
        "name": "Responder /chuva",
        "func": "const rawId = msg.req.params.id || \"\";\nconst id = String(rawId).trim();\n\nif (!id) {\n  msg.statusCode = 400;\n  msg.payload = { error: \"Informe o ID na URL: /chuva/:id\" };\n  return msg;\n}\n\nconst dado = global.get(\"chuva_\" + id);\n\nnode.warn(\"[RESPOSTA] Buscando chave: chuva_\" + id + \" -> \" + JSON.stringify(dado));\n\nmsg.headers = {\n  \"Access-Control-Allow-Origin\": \"*\",\n  \"Access-Control-Allow-Methods\": \"GET, OPTIONS\",\n  \"Access-Control-Allow-Headers\": \"Content-Type, Authorization\"\n};\n\nif (!dado) {\n  msg.statusCode = 404;\n  msg.payload = { error: \"Nenhum dado encontrado para este id\", id };\n} else {\n  msg.payload = dado;\n}\n\nreturn msg;\n",
        "outputs": 1,
        "timeout": 0,
        "noerr": 0,
        "initialize": "",
        "finalize": "",
        "libs": [],
        "x": 490,
        "y": 480,
        "wires": [
            [
                "d1a4be30a1e21872"
            ]
        ]
    },
    {
        "id": "d1a4be30a1e21872",
        "type": "http response",
        "z": "fad4f935c49154a2",
        "name": "http response",
        "statusCode": "",
        "headers": {},
        "x": 800,
        "y": 480,
        "wires": []
    },
    {
        "id": "4a2ad9e4a1b1d5cf",
        "type": "debug",
        "z": "fad4f935c49154a2",
        "name": "debug 1",
        "active": true,
        "tosidebar": true,
        "console": false,
        "tostatus": false,
        "complete": "payload",
        "targetType": "msg",
        "statusVal": "",
        "statusType": "auto",
        "x": 1020,
        "y": 260,
        "wires": []
    },
    {
        "id": "8035d64024d5cdb0",
        "type": "mqtt-broker",
        "name": "meubroker",
        "broker": "localhost",
        "port": 1883,
        "clientid": "",
        "autoConnect": true,
        "usetls": false,
        "protocolVersion": 4,
        "keepalive": 60,
        "cleansession": true,
        "autoUnsubscribe": true,
        "birthTopic": "",
        "birthQos": "0",
        "birthRetain": "false",
        "birthPayload": "",
        "birthMsg": {},
        "closeTopic": "",
        "closeQos": "0",
        "closeRetain": "false",
        "closePayload": "",
        "closeMsg": {},
        "willTopic": "",
        "willQos": "0",
        "willRetain": "false",
        "willPayload": "",
        "willMsg": {},
        "userProps": "",
        "sessionExpiry": ""
    }
]
```

Após colar o conteúdo, o fluxo será importado.
Para concluir, clique em **Implementar**.

---

# ⚙️ Configurando o Ambiente e Rodando o Projeto

Abra no **PlatformIO** a pasta `torre-sentinela`, criada ao clonar o repositório.

Em seguida, edite o arquivo **wifi_config.h**, inserindo as credenciais da rede Wi-Fi que será utilizada:

```c
#ifndef CONFIG_WIFI_H
#define CONFIG_WIFI_H

#define WIFI_SSID "seu-wifi"
#define WIFI_PASSWORD "sua-senha"

void conectaWiFi();

#endif
```

Com a placa **ESP32** conectada ao computador:

1. Execute o **Build**
2. Execute o **Upload**
3. Abra o **Monitor Serial** para identificar o *ID* da placa, que será usado pelo sistema



