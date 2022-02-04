#include "stm32f0xx.h"



#define VCC 3300 //Vcc em mV
#define RESOLUCAO 4095 // Resolucao do ADC de 12 bits

#define BOTAO1 0
#define BOTAO2 1650
#define BOTAO3 2200
#define BOTAO4 2475
#define BOTAO5 2640
#define BOTAO6 2750

#define BOTAO1_2 (BOTAO1+BOTAO2)/2
#define BOTAO2_3 (BOTAO2+BOTAO3)/2
#define BOTAO3_4 (BOTAO3+BOTAO4)/2
#define BOTAO4_5 (BOTAO4+BOTAO5)/2
#define BOTAO5_6 (BOTAO5+BOTAO6)/2
#define BOTAO6_SB (BOTAO6+VCC)/2


void Escreve_Serial(uint8_t * str); //Funcao nao implementada que escreve string na serial


void configADC()
{
  // liga o clock do periperico
    RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // seleciona modo analógico
    GPIOA->MODER |= GPIO_MODER_MODER0;
    // high speed
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR0;
    // limpa seleção de pull-up/down
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0);

    // o ADC só pode ser configurado se não estiver habilitado (ADEN = 0)
    ADC1->CR &= ~(ADC_CR_ADEN);
   
    // seta modo contínuo
    ADC1->CFGR1 |= ADC_CFGR1_CONT;
    // seleciona canal
    ADC1->CHSELR = ADC_CHSELR_CHSEL0;

    // inicia calibracao
    ADC1->CR |= ADC_CR_ADCAL;
    while ((ADC1->CR & ADC_CR_ADCAL) != 0);

    // habilita ADC
    ADC1->CR |= ADC_CR_ADEN;
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0);
       

}
uint16_t lerTensao()
{
  
  ADC1->CR |= ADC_CR_ADSTART; // Inicia Conversao
  while (0 == (ADC1->ISR & ADC_ISR_EOC)); // Verifica se finalizou
  uint16_t valor= ADC1->DR; // Le o valor
  valor = valor*VCC; 
  valor = valor/RESOLUCAO;
  return valor; //Converte para mV

}


uint8_t botaoApertado()
{
  uint16_t tensao=lerTensao();
  if (tensao<BOTAO1_2)
  {
    return 1;
  }
  else if (tensao<BOTAO2_3)
  {
    return 2;
  }
  else if (tensao<BOTAO3_4)
  {
    return 3;
  }
  else if (tensao<BOTAO4_5)
  {
    return 4;
  }
  else if (tensao<BOTAO5_6)
  {
    return 5;
  }
  else if (tensao<BOTAO6_SB)
  {
    return 6;
  }
  else 
  {
    return 7;
  }

}

void ExibeBotao()
{
  static uint8_t valor_ant=0;
  static uint8_t valor_novo;
  valor_novo=botaoApertado();
  if (valor_novo!=valor_ant)
  {
    if (valor_novo>=1 && valor_novo<=6)
    {
      Escreve_Serial("Botao ");
      Escreve_Serial(0x30+valor_novo);
      Escreve_Serial(" Pressionado!");
    }
    else if(valor_novo==7)
    {
      Escreve_Serial("Nenhum botao pressionado");
    }
  }


}
int main(void)
{

  configADC();

  while (1)
  {
	  ExibeBotao();
  }
}
