

# DESAFIO - VAGA DESENVOLVEDOR DE SOFTWARE EMBARCADO
## Leonardo Arruda

### Observa��es Gerais
Os firmwares foram feitos usando a IDE Atollic e o micro STM32F071CB. 
Os firmwares n�o est�o 'rodando', pois faltam algumas fun��es secund�rias que n�o foram implementadas.


### Quest�o 1
Nessa quest�o, foi calculado a tens�o esperada quando cada bot�o era pressionado. Em seguida, configurou-se um ADC para fazer a leitura.


### Quest�o 2
Nessa quest�o, foi desenvolvido um protocolo para transmiss�o dos dados. Como premissa, foi adotado que a mensagem pode ter dados e status de v�rios sensores diferentes, ou seja, pode ter 5, 10, 15... bytes.
Para confibialidade da transmiss�o, foi adicionado um byte de in�cio (SOF) e um de fim (EOF), um byte de tamanho dos dados. Por fim, o pen�ltimo byte e o antepen�ltimo cont�m o CRC de 16 bits dos bytes anteriores (menos o SOF).

| SOF    | TAMANHO | DADOS    | CRC     | EOF    |
|--------|---------|----------|---------|--------|
| 1 byte | 1 byte  | vari�vel | 2 bytes | 1 byte |

Caso tenha algum dos caracteres especiais na mensagem, � adicionado um byte stuffing onde, adiciona-se um byte de escape antes desse byte e soma-se um valor fixo nele.

### Quest�o 3
Como a quest�o era mais curta, foi adicionada no projeto da quest�o 2. Foi demarcado com um coment�rio.