

# DESAFIO - VAGA DESENVOLVEDOR DE SOFTWARE EMBARCADO
## Leonardo Arruda

### Observações Gerais
Os firmwares foram feitos usando a IDE Atollic e o micro STM32F071CB. 
Os firmwares não estão 'rodando', pois faltam algumas funções secundárias que não foram implementadas.


### Questão 1
Nessa questão, foi calculado a tensão esperada quando cada botão era pressionado. Em seguida, configurou-se um ADC para fazer a leitura.


### Questão 2
Nessa questão, foi desenvolvido um protocolo para transmissão dos dados. Como premissa, foi adotado que a mensagem pode ter dados e status de vários sensores diferentes, ou seja, pode ter 5, 10, 15... bytes.
Para confibialidade da transmissão, foi adicionado um byte de início (SOF) e um de fim (EOF), um byte de tamanho dos dados. Por fim, o penúltimo byte e o antepenúltimo contém o CRC de 16 bits dos bytes anteriores (menos o SOF).

| SOF    | TAMANHO | DADOS    | CRC     | EOF    |
|--------|---------|----------|---------|--------|
| 1 byte | 1 byte  | variável | 2 bytes | 1 byte |

Caso tenha algum dos caracteres especiais na mensagem, é adicionado um byte stuffing onde, adiciona-se um byte de escape antes desse byte e soma-se um valor fixo nele.

### Questão 3
Como a questão era mais curta, foi adicionada no projeto da questão 2. Foi demarcado com um comentário.