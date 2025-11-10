# DIREÇÕES SOBRE A ENTREGA DO INTERPRETADOR MyBC

| Item | Instrução | Responsável | Concluída |
| --- | --- | --- | --- |
| 0 | identificar os membros do grupo, bem como o número tanto no preâmbulo do código principal, quanto no nome do pacote *.tar.gz; | Vitor F. | Sim |
| 1 | modificar a função match para que, em caso de erro, acuse o número da linha e a coluna; | Gustavo |  |
| 2 | a mensagem de erro de sintaxe, ainda no match, deve ser reportando o que era esperado e o que foi encontrado; | Gustavo |  |
| 3 | não é para imprimir o número associado ao token e sim o nome do mesmo, à exceção dos códigos ASCII, como ';' '.' etc; |  |  |
| 4 | ainda no tratamento de erro no match, resolver o problema do exit para não quebrar a execução do ineterpretador de comando; | Vitor F. |  |
| 5 | a documentação dos códigos devem ser feitas na forma de comentários elucidativos, que facilitem a compreensão dos métodos envolvidos no contexto de compiladores; | Vitor |  |
| 6* | uma documentação suplementar, não necessária, seria uma redação mais didática sobre os métodos adotados e detalhes de implementação; |  |  |
| 7* | filtrar as teclas <- e -> para evitar corrupção do buffer stdin; |  |  |
| 8* | capturar o sinal de interrupção (ctrl C) para imprimir uma quebra de linha |  |  |
