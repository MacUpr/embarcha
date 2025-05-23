# Simulador Portátil de Alarme com Raspberry Pi Pico W

Este projeto é uma ferramenta portátil, baseada no microcontrolador Raspberry Pi Pico W, destinada a simulações de emergência e treinamentos de evacuação de brigadas. 

## Visão Geral

O sistema opera como um Access Point Wi-Fi autônomo, permitindo que um instrutor, utilizando um dispositivo móvel (smartphone, tablet), conecte-se à rede do Pico W e acione remotamente o modo "ALARME". Uma vez ativado, o dispositivo emite sinais visuais (LED piscante) e sonoros (buzzer), além de exibir a mensagem "EVACUAR" em um display OLED. [cite: 14] Quando desativado, exibe "Sistema em repouso". [cite: 16] O projeto visa consolidar conhecimentos sobre redes sem fio e controle de hardware em sistemas embarcados. 

## Funcionalidades Principais

* **Modo Access Point:** O Raspberry Pi Pico W cria sua própria rede Wi-Fi, não necessitando de roteador externo. 
* **Servidor HTTP Embarcado:** Uma interface web simples hospedada no Pico W permite o controle. 
* **Controle Remoto:** Botões "Ligar" e "Desligar" na interface web para ativar/desativar o alarme. 
* **Sinalização Visual:** Um LED pisca em modo "Alarme". [cite: 14, 16, 21]
* **Sinalização Sonora:** Um buzzer é ativado durante o alarme. [cite: 14, 16, 22]
* **Display Informativo:** Um display OLED SSD1306 exibe mensagens como "EVACUAR" ou "Sistema em repouso". 

## Materiais e Conceitos Envolvidos

* Raspberry Pi Pico W
* LED
* Buzzer
* Display OLED SSD1306 (comunicação I2C [cite: 23, 24])
* Programação em C/C++ com Pico SDK
* Redes Wi-Fi (modo Access Point) 
* Servidor HTTP (LwIP)
* Controle de GPIO 
* Temporização para alertas não-bloqueantes 
