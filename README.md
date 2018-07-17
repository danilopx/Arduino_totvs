# Arduino_totvs

Projeto com o objetivo de integrar o ERP Protheus da Totvs com IHM desenvolvido com arduino para contabilizar o número de peças produzidas e fazer o apontamento de produção e emissão de etiqueta de identificação da caixa de forma automática.

1) Requisitos Protheus

  - Criação de Webserver REST em ADVPL para integração de informação com o IHM desenvolvido com arduino.
  - Buscar infomação da ordem de produção (SC2010) atraves do código de barras.
  - Buscar informação do Produto (SB1010) atrelado a ordem de produção, principalmente a unidade de medida e o multiplo da caixa para apontamento de produção.
  - Apontar produção conforme multiplo na tabela SD3010 atraves do autoexec mata250.
  - Criar tabela auxiliar para armazenar as peças contabilizadas (S5010)
  - Emitir etiqueta de identificação da caixa com código, descrição do produto, quantidade da caixa, lote, data e turno.
  
  
  2) Requisitos IHM desenvolvido com arduino
  
  - Shield UsbHost Adk 2.0 para conectar leitor de código de barras.
  - Shield rs232 para conexão com a impressora Zebra GC420t.
  - Shield Internet enc28j60 para comunicação com o Webserver REST.
  - Shield display lcd 20x4 para mostrar informação da ordem de produção e o núnero de peças contabilizada.
  - Conexão atraves de pulso digital com o equipamento de teste de fuga de corrente (Hipot) localizado no final da linha de produção para fazer a contabilização das peças aprovadas.



