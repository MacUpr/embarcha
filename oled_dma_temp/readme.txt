Leitura de Temperatura via ADC + DMA com Display SSD1306 no Raspberry Pi Pico W

Este repositório apresenta um exemplo completo de como utilizar o Raspberry Pi Pico W para:
	1.	Capturar 100 leituras do sensor interno de temperatura via ADC, usando DMA para liberar a CPU;
	2.	Calcular a temperatura média em graus Celsius;
	3.	Exibir o resultado em tempo real em um display OLED SSD1306, utilizando I²C;
	4.	Atualizar a cada segundo, de forma eficiente e sem bloqueios perceptíveis.

⸻

🚀 Funcionalidades principais
	•	ADC + DMA
	•	Transferência direta de 100 amostras para RAM
	•	Processamento de média sem sobrecarregar o MCU
	•	Conversão de temperatura
	•	Fórmula oficial do RP2040 para converter tensão em °C
	•	Interface SSD1306
	•	Renderização de texto escalável (2×)
	•	Centralização automática da string
	•	Loop contínuo
	•	Atualização da tela a cada 1 s
	•	Baixo consumo de CPU entre leituras

⸻

🔧 Hardware necessário
	•	Raspberry Pi Pico W
	•	Display OLED SSD1306 (128 × 64 pixels)
	•	Resistores de pull-up para SDA e SCL (se não integrados)
	•	Conexões I²C:
	•	GPIO14 → SDA
	•	GPIO15 → SCL
