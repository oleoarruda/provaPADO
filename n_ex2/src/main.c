#include "stm32f0xx.h"




#define TAM_MAX_REC 256
#define NUM_MAX_PACOTES 30


#define SOF 0x01
#define EOF 0x05
#define ESC 0x50


typedef struct Struct_Pacote
{
	float valor;
	uint8_t modelo;
	uint8_t status;
} pacote;

typedef struct Struct_Mensagem
{
    pacote pacotes[NUM_MAX_PACOTES];
    uint8_t tam;
} mensagem;

typedef union {
    float valorFloat;
    uint8_t vet[sizeof(float)];
} floatByte;


uint8_t rx_msg[TAM_MAX_REC];
uint8_t len=0;

mensagem recebida;
uint8_t flag_rec_msg=0;

uint16_t CRC16(uint8_t * msg, uint8_t tam); //Funcao que calcula o CRC com 16 bits

uint8_t Ler_Buffer(uint8_t * dado_lido); //Lê um buffer circular

void Escrever_Serial(uint8_t dado); //Escreve um byte na serial

uint8_t Tratar_Msg_Rx(uint8_t * msg, uint8_t len)
{
    uint16_t crc_rx = 0;
    uint16_t crc_calc = 0;
    uint8_t i=0;
    floatByte valor_lido;


    crc_rx = msg[len - 3]; //Pega o CRC da mensagem
    crc_rx <<= 8;
    crc_rx |= msg[len - 2];


    crc_calc = CRC16(&msg[1], (len - 4)); //Calcula o CRC

    if (crc_rx == crc_calc) // Verifica se o CRC calculado é igual ao recebido
    {

        uint8_t rx_len = msg[1];

        if ((len - 5) == rx_len) // Verifica se o tamanho recebido é igual ao tamanho no corpo da mensagem
        {
            recebida.tam=rx_len/5; //Calcula a quantidade de pacotes de dados recebidos dividindo o tamanho por 5

            for (i=0;i<(recebida.tam);i++) //Iteraçao nos pacotes de dados
            {

                valor_lido.vet[0]=msg[2+(5*i)]; //Salva os 4 primeiros bytes
                valor_lido.vet[1]=msg[3+(5*i)];
                valor_lido.vet[2]=msg[4+(5*i)];
                valor_lido.vet[3]=msg[5+(5*i)];

                recebida.pacotes[i].valor=valor_lido.valorFloat; // Transforma em float

                recebida.pacotes[i].modelo=msg[6+(5*i)]>>4; // Salva o modelo
                recebida.pacotes[i].status=msg[6+(5*i)]&0x0F; //Salva o Status

            }

            flag_rec_msg=1;



        }

    }

    return 0;
}

void Protocol_Rx(void)
{
    static uint8_t flag_rx_on = 0;
    static uint8_t flag_rx_esc = 0;

    uint8_t status_buffer = 0;
    uint8_t dado_lido = 0;


    status_buffer = Ler_Buffer(&dado_lido); // Lê o buffer circular que contem os dados recebidos na serial

    if (status_buffer)
    {
        if (dado_lido == SOF) //Se o dado lido é o SOF, inici o recebimento da mensagem
        {
            rx_msg[0] = dado_lido;
            len = 1;
            flag_rx_on = 1;
        }
        else if (flag_rx_on == 1)
        {
            if (flag_rx_esc == 1) //Caso o byte de escape tenha sido lido por ultimo, o proximo byte devera ser descontado 0x50
            {
                flag_rx_esc = 0;
                rx_msg[len] = (uint8_t)(dado_lido - 0x50);
                len++;
            }
            else if (ESC == dado_lido) //Caso o byte lido seja o de escape, ativa a flag
            {
                flag_rx_esc = 1;
            }
            else //Se nao estiver lendo byte de escape, salva o byte normalmente
            {
                rx_msg[len] = dado_lido;
                len++;
                if (EOF == dado_lido) // Caso tenha lido o EOF, chama a funcao para interpretar
                {
                    Tratar_Msg_Rx(rx_msg, len);
                    flag_rx_on = 0;
                    flag_rx_esc = 0;
                }

            }

        }

    }

}

void Protocol_Tx(uint8_t * frame, uint8_t len)
{

    uint8_t i;

    Escrever_Serial(SOF);
    for (i = 1; i < len-1; i++) //Percorre o vetor de dados procurando um dos bytes especiais, caso encontre, adiciona um byte de escape e altera o byte somando 0x50
    {
        if ((frame[i] == SOF) || (frame[i] == EOF) || (frame[i] == ESC))
        {
            Escrever_Serial(ESC);
            Escrever_Serial(frame[i] + ESC);
        }
        else
        {
           Escrever_Serial(frame[i]);
        }
    }
    Escrever_Serial(EOF);
}



void Tratar_Msg_Tx(mensagem tx)
{
    uint8_t frame[256];
    uint8_t len=0,i;

    floatByte valorFB;

    frame[len++]=SOF;
    frame[len++]=(tx.tam*5);

    for (i=0;i<tx.tam;i++) // Iteracao sobre os pacotes de dados
    {
        valorFB.valorFloat=tx.pacotes[i].valor;

        frame[len++]=valorFB.vet[0]; //Pega o valor em float dividido em 4 bytes
        frame[len++]=valorFB.vet[1];
        frame[len++]=valorFB.vet[2];
        frame[len++]=valorFB.vet[3];

        frame[len++]=(tx.pacotes[i].modelo<<4)|(tx.pacotes[i].status); //Pega o status e a marca


    }
    uint16_t crc = CRC16(&frame[1], (len - 1)); //Calcula o CRC de todos os bytes, menos do SOF


    frame[len++] = (uint8_t)(crc >> 8);
    frame[len++] = (uint8_t)(crc);

    frame[len++] = EOF;

    Protocol_Tx(frame, len);



}

//EXERCICIO 3
typedef union {
    double valorDouble;
    uint8_t vet[sizeof(double)];
} doubleByte;
void convert(double dado, uint8_t * vetor)
{
	doubleByte uniao;
	uniao.valorDouble=dado;
	uint8_t i;
	for (i=0;i<sizeof(double);i++)
	{
		vetor[i]=uniao.vet[i];
	}


}


int main(void)
{


  while (1)
  {

	Protocol_Rx(); // Verifica em loop se recebeu uma mensagem seguindo o protocolo na serial e chama a funcao que vai traduzir
	if (flag_rec_msg) // Verifica se uma mensagem chegou e foi traduzida na struct
    {
        flag_rec_msg=0;
        //Aqui pode ser tratada a struct com os valores ja separados.
    }

    //Para enviar os dados na serial, basta usar a funcao Tratar_Msg_Tx
  }
}
