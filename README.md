# ♜ TORRES SENTINELA 
As Torres Sentinela são um protótipo de sistema de monitoramento e coleta de dados de precipitação (chuvas).

Desenvolvido em torno da placa ESP32, o projeto tem como principal propósito alimentar com dados o Projeto Sentinela, uma iniciativa maior para a qual você pode encontrar 
informações detalhadas e o contexto de aplicação em (link do google sites de projetos).

</br>

*ATENÇÃO: Este repositório contém apenas a parte de coleta de dados (hardware e firmware). Para visualizar o sistema em funcionamento completo e acompanhar os dados em 
tempo real, você acesse o repositorio da interface de usuario em: [https://github.com/SofiaValadares/TorreSentinela-Front](https://github.com/SofiaValadares/TorreSentinela-Front)*
</br>

### Desenvolverores
<table>
  <tr>
    <td align="center">
      <a href="https://github.com/SofiaValadares">
        <img src="https://avatars.githubusercontent.com/u/113111708?v=4" width="100px;" alt="Foto Sofia Valadares"/><br>
        <sub>
          <b>Sofia Valadares</b>
        </sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/SofiaValadares">
        <img src="https://avatars.githubusercontent.com/u/113111708?v=4" width="100px;" alt="Foto Sofia Valadares"/><br>
        <sub>
          <b>Sofia Valadares</b>
        </sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/SofiaValadares">
        <img src="https://avatars.githubusercontent.com/u/113111708?v=4" width="100px;" alt="Foto Sofia Valadares"/><br>
        <sub>
          <b>Sofia Valadares</b>
        </sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/SofiaValadares">
        <img src="https://avatars.githubusercontent.com/u/113111708?v=4" width="100px;" alt="Foto Sofia Valadares"/><br>
        <sub>
          <b>Sofia Valadares</b>
        </sub>
      </a>
    </td>
</table>


# MONTANDO O PROJETO
Para montar o projeto é necessario garantir uma serie de requisitos de hardware e software

__Requisitos de Hardware__
1. Módulo WiFi ESP32 Bluetooth 30 pinos
2. Sensor de Precipitação

__Requisitos de Software__
1. Node-RED
2. Visual Studio Code com extenção do PlataformIO

## Montagem do Circuito

## Clonando o Projeto

## Node-RED
Para instalar o Node-RED, siga as instruções oficiais na página [Running Node-RED locally](https://nodered.org/docs/getting-started/local).
Lá você encontrará o passo a passo necessário para realizar a instalação usando **npm**.

Após concluir a instalação, execute o comando abaixo no terminal para iniciar o Node-RED localmente:

```bash
node-red --port 1880
```

Quando o serviço estiver em execução, a interface ficará disponível em:
**[http://127.0.0.1:1880/](http://127.0.0.1:1880/)**

Ao acessar o Node-RED, você verá o painel principal. No canto superior esquerdo há um menu “sanduíche”, onde existe a opção **Importar**.

Ao selecionar **Importar**, cole o conteúdo do JSON fornecido no projeto.
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
Após colar o conteúdo, o fluxo será importado para o Node-RED. Em seguida, basta clicar em Implementar para aplicá-lo.

## Configurando o Ambiente

## Rodando o Projeto


