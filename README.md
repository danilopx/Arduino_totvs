# Arduino_totvs

Projeto tem como objetivo, integrar o Sistema ERP Protheus da Totvs com um coletor de dados desenvolvido em arduino que será responsavel por contabilizar o número de peças produzidas e fazer o apontamento de produção e emissão de etiqueta de identificação da caixa de forma automática.

1) Requisitos Protheus

  - Criação de Webserver REST em ADVPL para a troca de informação da ondem de produção do Protheus com o coletor de dados.
  - Buscar infomação da ordem de produção (SC2010) atraves do código de barras.
  - Buscar informação do Produto (SB1010) atrelado a ordem de produção, principalmente a unidade de medida e o multiplo da       caixa para apontamento de produção.
  - Apontar produção conforme multiplo na tabela SD3010 atraves do autoexec mata250.
  - Criar tabela auxiliar para armazenar as peças contabilizadas (SZ5010)
  - Emitir etiqueta de identificação da caixa com código, descrição do produto, quantidade da caixa, lote, data e turno.
  
  
  2) Requisitos coletor de dados desenvolvido com arduino
  
  - Shield UsbHost Adk 2.0 para conectar leitor de código de barras.
  - Shield rs232 para conexão com a impressora Zebra GC420t.
  - Shield Internet enc28j60 para comunicação com o Webserver REST.
  - Shield display lcd 20x4 para mostrar informação da ordem de produção e o núnero de peças contabilizada.
  - Conexão atraves de pulso digital com o equipamento de teste de fuga de corrente (Hipot) localizado no final da linha de produção para fazer a contabilização das peças aprovadas.
  
  
Fase 1 - Esbolso do IHM com arduino.
  
Nesta fase foi feita a ligação dos componentes(Display LCD, Enc28j60, UsbHost) e teste com o webservice do protheus.
  
![img](https://github.com/danilopx/Arduino_totvs/blob/master/img/img01.jpeg)


Fase 2 - Montagem do Prototipo
 
Apos testes iniciais na fase 1, foi montado o prototipo do IHM. Para a montagem foi utilizado uma caixa em plastica dom as medidas 
16cm x 10cm x 7 cm.
Na parte frontal da caixa ficou instalado o Display LCD, dois push botton e dois leds, um verde e outro vermelho para sinais    indicativos.

Na lateral direira estão a entrada USB padrão do arduino, entrada USB do shield UsbHost para conexão do leitor de código de barras e a entrada de fonte de energia.

Na lateral esquerda estão a entrada de rede RJ45 e a entrada serial RS232 para comunicação com a impressora Zebra.

  ![img](https://github.com/danilopx/Arduino_totvs/blob/master/img/img2.jpg)
  
Fase 3 - Comunicação com a impressora Zebra GC420t

A comunicação do coletor de dados com a impressora foi feita pela comunicação serial RS232, o cabo utilizado tem nas duas pontas conector DB9 macho, conforme esquema na imagem abaixo os pinos ligados são 2 com 3, 3 com 2 e 5 com 5.

![img](https://github.com/danilopx/Arduino_totvs/blob/master/img/cabo-serial.jpg)
 
Foi utilizado a linguagem de programação ZPL para a montagem da etiqueta e envio para a impressora. 



