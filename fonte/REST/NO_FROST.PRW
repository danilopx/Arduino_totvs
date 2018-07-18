#INCLUDE "TOTVS.CH"
#INCLUDE "RESTFUL.CH"  
#include "topconn.ch"   
#include "protheus.ch"

WSRESTFUL nofrost DESCRIPTION "Exemplo de servi√Åo REST"
†
WSDATA count† AS INTEGER
WSDATA startIndex AS INTEGER
†
WSMETHOD GET DESCRIPTION "Exemplo de retorno de entidade(s)" WSSYNTAX "/nofrost/op/{id}"
//WSMETHOD POST DESCRIPTION "Exemplo de inclusao de entidade" WSSYNTAX "/nofrost/op/{id}"
//WSMETHOD PUT DESCRIPTION "Exemplo de altera√Å‚Äûo de entidade" WSSYNTAX "/nofrost/op/{id}"
//WSMETHOD DELETE DESCRIPTION "Exemplo de exclus‚Äûo de entidade" WSSYNTAX "/nofrost/op/{id}"

END WSRESTFUL

// O metodo GET nao precisa necessariamente receber parametros de querystring, por exemplo:
// WSMETHOD GET WSSERVICE clientes‚Ä†

WSMETHOD GET WSRECEIVE startIndex, count WSSERVICE nofrost
Local i
Local cRet:="" 
Local cArea   := getNextAlias()
Local cOP2 := ::aURLParms[1] 
Local cEmp := "0301"


// define o tipo de retorno do m√àtodo
::SetContentType("application/json")

DEFAULT ::startIndex := 1, ::count := 5
i:=::startIndex

If Len(::aURLParms) <= 0

		::SetResponse('{"erro":"OP invalido"}')

Else  

////////////////////////////////////////Busca de Dados Op

   if(!EMPTY(cOP2) .and. len(cOP2) == 11)      

       cOP2 := SubStr(cOP2, 1, 6)


       BEGINSQL ALIAS cArea     

           select C2_NUM, C2_QUANT, C2_PRODUTO, C2_QUJE, C2_QUANT - C2_QUJE as C2_SALDO, B1_QE , B1_CONV ,C2_UM,  
           case when B1_QE > 0  then (C2_QUJE / B1_QE) else 0 end AS  C2_ETQ,C2_DATRF
           from SC2010 C2 INNER JOIN SB1010 B1 ON B1_COD = C2_PRODUTO AND B1.%notdel% 
           where C2_NUM = %exp:cOP2% AND C2_FILIAL = %exp:cEmp% and C2.%notdel%
       ENDSQL
       	IF ((cArea)->(!eof()))
    	 
            IF ( Empty ((cArea)->C2_DATRF)  )
            	::SetResponse('{') 
            	::SetResponse('"op":"'+(cArea)->C2_NUM+'"')  
            	::SetResponse(',')
            	::SetResponse('"qt":"'+cValToChar((cArea)->C2_QUANT)+'"')  
             	::SetResponse(',')  
                ::SetResponse('"pr":"'+Alltrim((cArea)->C2_PRODUTO)+'"')  
            	::SetResponse(',') 
       			::SetResponse('"qp":"'+cValToChar((cArea)->C2_QUJE)+'"')  
       			::SetResponse(',') 
     		  	::SetResponse('"sl":"'+cValToChar((cArea)->C2_SALDO)+'"') 
      		 	::SetResponse(',')
     		  	::SetResponse('"cx":"'+cValToChar((cArea)->B1_QE)+'"')  
     		  	::SetResponse(',')
      		 	::SetResponse('"cv":"'+cValToChar((cArea)->B1_CONV)+'"')  
      		 	::SetResponse(',')
      		 	::SetResponse('"um":"'+(cArea)->C2_UM+'"') 
      		 	::SetResponse(',') 
     		  	::SetResponse('"et":"'+cValToChar((cArea)->C2_ETQ)+'"')  
     		 	::SetResponse('}')   
	
              else    
                ::SetResponse('{"erro":"OP Encerrada"}')  
			
              Endif 
        Endif
        
        (cArea)->(dbCloseArea())
        
    Else
		::SetResponse('{"erro":"OP Nao Localizada"}')  
			
	EndIf
EndIf	

//::SetResponse(']')
Return .T.
/*
// O metodo POST pode receber parametros por querystring, por exemplo:
// WSMETHOD POST WSRECEIVE startIndex, count WSSERVICE clientes
WSMETHOD POST WSSERVICE clientes
Local lPost := .T.
Local cJson
Local oJson	

// Exemplo de retorno de erro
If Len(::aURLParms) == 0
†SetRestFault(400, "id parameter is mandatory")
†lPost := .F.
Else
†// recupera o body da requisi√Å‚Äûo
cJson := ::GetContent()
conout('cBody>> '+cJson)
fwjsondeseialize(cJson,@oJson)

If Len(oJson) > 0
	SX3->(DbSetOrder(1))
	SX3->(DbGoTop())
	SX3->(DbSeek('SA1'))
	While SX3->(!EOF()) .And. SX3->X3_ARQUIVO=="SA1"
		If !(EMPTY(SX3->X3_OBRIGAT))
	    	//If (oJson[Len(oJson)]:&(SX3->X3_CAMPO)== Nil) .Or. Empty(oJson[Len(oJson)]:&(SX3->X3_CAMPO))
	    	//	SetRestFault(400, "id " + SX3->X3_CAMPO + " is mandatory")
	    	//EndIf
	    EndIf 
    SX3->(DbSkip())
    EndDo
    printJson(jsonfields, "| ")
Else
	ConOut("##### [JSON][ERR] " + "Parser 1 com erro" + " MSG len: " + AllTrim(Str(lenStrJson)) + " bytes lidos: " + AllTrim(Str(nRetParser)))
   	ConOut("Erro a partir: " + SubStr(strJson, (nRetParser+1)))
EndIf

†// insira aqui o c√õdigo para opera√Å‚Äûo inser√Å‚Äûo
†// exemplo de retorno de um objeto JSON
†::SetResponse('{"id":' + ::aURLParms[1] + ', "name":"clientes"}')
EndIf
Return lPost
†
// O metodo PUT pode receber parametros por querystring, por exemplo:
// WSMETHOD PUT WSRECEIVE startIndex, count WSSERVICE clientes
WSMETHOD PUT WSSERVICE clientes
Local lPut := .T.
†
// Exemplo de retorno de erro
If Len(::aURLParms) == 0
† SetRestFault(400, "id parameter is mandatory")
† lPut := .F.
Else
† // recupera o body da requisi√Å‚Äûo
† cBody := ::GetContent()
† // insira aqui o c√õdigo para opera√Å‚Äûo de atualiza√Å‚Äûo
† // exemplo de retorno de um objeto JSON
† ::SetResponse('{"id":' + ::aURLParms[1] + ', "name":"clientes"}')
EndIf
Return lPut
†
// O metodo DELETE pode receber parametros por querystring, por exemplo:
// WSMETHOD DELETE WSRECEIVE startIndex, count WSSERVICE clientes
WSMETHOD DELETE WSSERVICE clientes
Local lDelete := .T.
†
// Exemplo de retorno de erro
If Len(::aURLParms) == 0
† SetRestFault(400, "id parameter is mandatory")
† lDelete := .F.
†
Else
† // insira aqui o c√õdigo para opera√Å‚Äûo exclus‚Äûo
 // exemplo de retorno de um objeto JSON
† ::SetResponse('{"id":' + ::aURLParms[1] + ', "name":"clientes"}')
EndIf
Return lDelete    

 /*








