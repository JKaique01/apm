/*
 * Apm.c
 *
 * Created: 25/11/2020 20:35:58
 * Author : Jardel Kaique
 */ 




#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <time.h>	


#include "nokia5110.h"


int date_ano=2015;
int date_mes=3;
int date_dia=25;

int date_hora=6;
int date_minuto=10;
int date_segundo=0;

int ms = 0;

//0 = root. 1 = horloge. 2 = calendrier.
//3.x = Texte.
float menu = 0.1;

int leitura_adc=0;

ISR(ADC_vect){
	leitura_adc = ADC;
	
	if (menu < 1){
		if (leitura_adc < 1023*(1.0/4)){
			menu = 0.1;
		}
		else if (leitura_adc < 1023*(2.0/4)){
			menu = 0.2;
		}
		else if (leitura_adc < 1023*(3.0/4)){
			menu = 0.3;
		}
		else if (leitura_adc < 1023*(4.0/4)){
			menu = 0.4;
		}
		
	}
	
	else if (menu > 2){
		if (leitura_adc < 1023*(2.0/4)){
			menu = 3.0;
		}
		else if (leitura_adc < 1023*(4.0/4)){
			menu = 3.1;
		}
	}
	
}






// ||Função para inicialização da USART||
void USART_Init(unsigned int ubrr)
{
	UBRR0H = (unsigned char)(ubrr>>8); //Ajusta a taxa de transmissão
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0); //Habilita o transmissor e o receptor
	UCSR0C = (1<<USBS0)|(3<<UCSZ00); //Ajusta o formato do frame: 8 bits de dados e 2 de parada
	
	DDRC = 0xFF; //Define a porta C como saída
}

// ||Função para envio de um frame de 5 a 8bits||
void USART_Transmit(unsigned char data)
{
	while(!( UCSR0A & (1<<UDRE0)));//Espera a limpeza do registr. de transmissão
	UDR0 = data; //Coloca o dado no registrador e o envia
}

// ||Função para recepção de um frame de 5 a 8bits||
unsigned char USART_Receive(void)
{
	while(!(UCSR0A & (1<<RXC0))); //Espera o dado ser recebido
	return UDR0; //Lê o dado recebido e retorna
}

//USART configura hora e data
ISR(USART_RX_vect)
{
	char recebido;
	recebido = UDR0;
	

	switch(recebido){
		case 'h':
		date_hora-=1;
		break;
		case 'H':
		date_hora+=1;
		break;
		case 'm':
		date_minuto-=1;
		break;
		case 'M':
		date_minuto+=1;
		break;
		case 's':
		date_segundo-=1;
		break;
		case 'S':
		date_segundo+=1;
		break;
		case 'd':
		date_dia-=1;
		break;
		case 'D':
		date_dia+=1;
		break;
		case 'e':
		date_mes-=1;
		break;
		case 'E':
		date_mes+=1;
		break;
		case 'a':
		date_ano-=1;
		break;
		case 'A':
		date_ano+=1;
		break;
		default:
		break;
	}
	
	
	
}

//Função principal de exibição.
void atualiza_display(){
	nokia_lcd_clear(); //Limpa o LCD
	
	if (menu < 1){
		if (menu == 0.1){
			nokia_lcd_set_cursor(4, 6);
			nokia_lcd_write_string("Apm", 5);
		}
		else if (menu == 0.2){
			nokia_lcd_set_cursor(3, 10);
			nokia_lcd_write_string("Horloge", 2);
		}
		else if (menu == 0.3){
			nokia_lcd_set_cursor(13, 10);
			nokia_lcd_write_string("Calendrier", 1);
		}
		else if (menu == 0.4){
			nokia_lcd_set_cursor(2, 10);
			nokia_lcd_write_string("Texte", 3);
		}
	}
	
	else if (menu == 1){
		//Exibir relógio
		nokia_lcd_set_cursor(20, 0);
		nokia_lcd_write_string("Horaire: ", 1);
		char horloge[8];
		
		
		sprintf(horloge, "%02i:%02i:%02i", date_hora, date_minuto, date_segundo);
		nokia_lcd_set_cursor(20, 25);
		nokia_lcd_write_string(horloge, 1);
		
		
		
	}
	else if(menu == 2){
		//Exibir calendário
		nokia_lcd_set_cursor(30, 0);
		nokia_lcd_write_string("Date: ", 1);
		char data[12];
		sprintf(data, "%02i/%02i/%04i", date_dia, date_mes, date_ano);
		nokia_lcd_set_cursor(10, 30);
		nokia_lcd_write_string(data, 1);
		
	}
	else if(menu > 2){

		if (menu==3.0){
			nokia_lcd_set_cursor(0, 10);
			nokia_lcd_write_string("Alimentar gatos antes de dormir", 1);
		}
		else if (menu == 3.1){
			nokia_lcd_set_cursor(0, 10);
			nokia_lcd_write_string("Continuar leitura sobre robotica", 1);
		}
		
		
	}
	
	
	nokia_lcd_render();
}



//Botão OK
//Interrupção
ISR(INT1_vect){
	
	if (menu >=1 ){
		menu = ((int)(menu/10))+0.1;
	}
	
	if (menu < 1){
		//Menu 0
		//Apm
		
		if (menu == 0.2){
			menu = 1;
		}
		if (menu == 0.3){
			menu = 2;
		}
		if (menu == 0.4){
			menu = 3;
		}
	}
	
	
	
	
}



void incrementa_date(){
	
	if (date_segundo < 59){
		date_segundo+=1;
	}
	else{
		date_segundo=0;
		if (date_minuto < 59){
			date_minuto+=1;
		}
		else{
			date_minuto=0;
			if (date_hora<23){
				date_hora+=1;
			}
			else{
				date_hora=0;
				if (date_dia < 31){
					date_dia+=1;
				}
				else{
					date_dia=1;
					if (date_mes < 12){
						date_mes+=1;
					}
					else{
						date_mes=1;
						date_ano+=1;
					}
				}
			}
		}
	}
	
}


ISR(TIMER0_COMPA_vect){ //Interrupção do TC0 a cada 1 ms (64*249+1))/(16 MHz)
	ms+=1;
	if (ms==1000){
		ms=0;
		incrementa_date();
	}
	
}



int main(void)
{
	nokia_lcd_init(); //Inicia o LCD

		
	USART_Init(MYUBRR);

	
	//Direção dos pinos das portas C e D
	DDRD = 0x00; //todos os pinos da porta D como entradas.
	PORTD = 0xff; //pull-up em todos os pinos da porta D.
	
	
	//DDRB = 0xff; //Pinos PB1-PB5 como saídas para o display. A biblioteca já está fazendo isso.
	DDRC = 0b00000010;
	PORTC = 0xff;
	
	
	//Configuração do ADC
	ADMUX = 0b01000000;
	ADCSRA = 0b11101111;
	ADCSRB = 0x00;
	DIDR0 = 0b00111110;
	
	
	//Configuração das interrupções
	EICRA = 0b00001010; //Interrupção INT0 e INT1 na borda de descida.
	EIMSK = 0b00000011; //Habilita as interrupções INT0 e INT1.
	
	
	//Parte do Timer
	TCCR0A = 0b00000010; //Habilita modo CTC do TC0, ou seja, comparar com um valor.
	TCCR0B = 0b00000011; //liga TC0 com prescaler = 64.
	OCR0A = 249; //Ajusta o comparador para o TC0 contar de 0 até 249.
	TIMSK0 = 0b00000010; //Habilita a interrupção na igualdade de comparação com OCR0A. A interrupção ocorre a cada 1 ms = (64*(249+1)) / (16 MHz).
	
	
	sei(); //Habilita interrupções globais, ativando o bit I do SREG

	
	while(1)
	{
		
		atualiza_display();
		

		_delay_ms(100);
	}
}


