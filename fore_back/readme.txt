# Projeto Atividade 5 - Controle de LEDs e Fila com Raspberry Pi Pico W

Este projeto é uma extensão da "Atividade 5" da Residência em Sistemas Embarcados da e-Embarca Tech, focada na utilização de conceitos de foreground/background e no aprimoramento do controle de uma matriz de LEDs NeoPixel e uma fila de eventos utilizando uma Raspberry Pi Pico W.

## Descrição do Projeto

O sistema utiliza três botões (Botão A, Botão B e o botão do Joystick) para interagir com uma matriz de LEDs NeoPixel 5x5 e LEDs RGB externos. O Núcleo 0 da Pico W é responsável por tratar as interrupções geradas pelos botões, enquanto o Núcleo 1 atualiza o estado da matriz NeoPixel e dos LEDs RGB com base nas ações do usuário.

**Funcionalidades:**

* **Botão A:** Adiciona um novo evento à fila (representado por um número sequencial) e acende o próximo LED disponível na matriz NeoPixel com uma cor RGB aleatória.
* **Botão B:** Remove o evento mais antigo da fila e apaga o último LED aceso na matriz NeoPixel.
* **Botão do Joystick:**
    * Zera o contador de eventos.
    * Apaga todos os LEDs da matriz NeoPixel.
    * Reseta a fila, simulando um estado inicial de "fila vazia".
    * Acende o LED RGB externo azul para indicar o estado de reset/fila vazia.
* **LEDs RGB Externos:**
    * **Vermelho:** Acende quando a matriz NeoPixel está completamente preenchida (todos os 25 LEDs acesos).
    * **Azul:** Acende quando a matriz NeoPixel está completamente apagada (ou após o reset pelo botão do joystick).
    * **Verde:** (Opcional) Pode ser usado para outras indicações.
* **Saída Serial (USB):** Imprime o estado atual da fila (tamanho, contador de eventos e elementos) sempre que um evento é adicionado ou removido, ou quando a fila é resetada.

## Materiais Utilizados

* Raspberry Pi Pico W
* Matriz de LEDs NeoPixel 5x5 (25 LEDs WS2812B)
* Joystick com botão
* Botões de pressão (Push-buttons)
* LEDs RGB (ou LEDs individuais Vermelho, Verde e Azul)
* Resistores apropriados para os LEDs e botões (se necessário)
* Protoboard e jumpers

## Estrutura do Código

O projeto é organizado nos seguintes arquivos principais:

* `Atividade_5.c`: Contém a função `main()`, inicialização do sistema, configuração das interrupções e o loop principal do Núcleo 0.
* `funcao_atividade_.c` / `funcao_atividade_.h`: Define e implementa a lógica de tratamento de eventos dos botões, gerenciamento da fila e controle dos LEDs RGB externos. A função `tratar_eventos_leds()` é executada no Núcleo 1.
* `funcoes_neopixel.c` / `funcoes_neopixel.h`: Contém as funções para inicializar e controlar a matriz de LEDs NeoPixel, incluindo a definição da ordem dos LEDs na matriz.
* `ws2818b.pio` / `ws2818b.pio.h`: Programa PIO para o controle dos LEDs WS2812B. O arquivo `.h` é gerado automaticamente pelo `pioasm`.
* `CMakeLists.txt`: Arquivo de configuração do CMake para compilar o projeto com o Pico SDK.

## Configuração do Ambiente e Compilação

1.  **Pré-requisitos:**
    * Raspberry Pi Pico SDK configurado. (Este projeto utiliza a versão 2.1.1 do SDK e a toolchain 14_2_Rel1)
    * CMake (versão 3.13 ou superior).
    * ARM GCC Compiler (arm-none-eabi-gcc).
    * Visual Studio Code com a extensão Raspberry Pi Pico (recomendado).

2.  **Estrutura de Pastas (Recomendada):**
    ```
    Atividade_5/
    ├── Atividade_5.c
    ├── funcao_atividade_.c
    ├── funcao_atividade_.h
    ├── funcoes_neopixel.c
    ├── funcoes_neopixel.h
    ├── ws2818b.pio
    ├── CMakeLists.txt
    └── build/      (será criada durante a compilação)
    ```

3.  **Compilação:**
    * Abra um terminal na pasta do projeto (`Atividade_5`).
    * Crie a pasta de build (se não existir): `mkdir build`
    * Entre na pasta de build: `cd build`
    * Execute o CMake para configurar o projeto (ajuste o caminho para o SDK se necessário):
        ```bash
        cmake -DPICO_SDK_PATH=/caminho/para/seu/pico-sdk ..
        ```
        (O `CMakeLists.txt` fornecido já tenta localizar o SDK em `~/.pico-sdk` ou `$ENV{USERPROFILE}/.pico-sdk`)
    * Compile o projeto:
        ```bash
        make
        ```
        (Ou, se estiver usando Ninja, que é o padrão configurado no `CMakeCache.txt` fornecido: `ninja`)

4.  **Upload para a Pico W:**
    * Pressione e segure o botão BOOTSEL na Pico W enquanto a conecta ao computador via USB.
    * Solte o botão BOOTSEL. A Pico W aparecerá como um dispositivo de armazenamento em massa.
    * Arraste o arquivo `.uf2` gerado na pasta `build` (ex: `Atividade_5.uf2`) para o dispositivo de armazenamento da Pico.
    * A placa irá reiniciar automaticamente e executar o programa.

## Funcionamento Detalhado

### Núcleo 0 (Principal)

* Inicializa a saída serial (USB).
* Inicializa os pinos dos LEDs RGB externos e dos botões.
* Inicializa a matriz NeoPixel.
* Configura as interrupções por borda de descida para os três botões (A, B, Joystick).
* Lança a função `tratar_eventos_leds()` para ser executada no Núcleo 1.
* Aguarda o Núcleo 1 sinalizar que está pronto.
* Entra em um loop de espera (`__wfi()`), aguardando interrupções.
* Quando uma interrupção de botão ocorre, a função `gpio_callback()` é chamada.
* `gpio_callback()` envia o índice do botão pressionado para a fila FIFO do multicore, que será lida pelo Núcleo 1.

### Núcleo 1 (Tratamento de Eventos)

* Sinaliza para o Núcleo 0 que está pronto.
* Entra em um loop infinito aguardando dados da fila FIFO do multicore (enviados pelo Núcleo 0).
* Ao receber o índice de um botão:
    * Aplica um debounce para evitar múltiplas detecções.
    * Verifica se apenas um botão está pressionado para evitar ações conflitantes.
    * **Se Botão A:**
        * Gera uma cor RGB aleatória.
        * Acende o próximo LED da matriz NeoPixel (`index_neo`) com a cor gerada.
        * Incrementa `index_neo`.
        * Se a fila não estiver cheia, adiciona um novo evento (o valor atual de `contador`) à fila.
        * Incrementa `contador`.
        * Imprime o estado da fila.
    * **Se Botão B:**
        * Decrementa `index_neo`.
        * Apaga o LED correspondente na matriz NeoPixel.
        * Se a fila não estiver vazia, remove um evento da fila.
        * Imprime o estado da fila.
    * **Se Botão do Joystick:**
        * Imprime uma mensagem de reset.
        * Reseta `contador` para 0.
        * Chama `npClear()` e `npWrite()` para apagar todos os LEDs da matriz.
        * Reseta `index_neo` para 0.
        * Reseta os ponteiros (`inicio`, `fim`) e a quantidade de elementos da fila para 0.
        * Imprime o estado da fila (que estará vazia).
    * Atualiza os LEDs RGB externos com base no estado da matriz NeoPixel (cheia ou vazia).
    * Aguarda o botão ser solto antes de processar novos eventos.

### Fila de Eventos

* Uma fila circular simples é implementada usando um array (`fila[]`) e os ponteiros `inicio`, `fim`, e o contador `quantidade`.
* `TAM_FILA` define o tamanho máximo da fila (25, correspondendo ao número de LEDs na matriz).
* `contador` é um contador global de eventos que é inserido na fila e resetado pelo botão do joystick.

## Melhorias Implementadas (Ref. Tarefa_ForegroundBackground_Cap10.pdf) [cite: 57]

* **Utilização do Botão do Joystick:** O botão do joystick agora possui a funcionalidade de resetar o sistema. [cite: 60, 67]
* **Reset do Contador de Eventos:** Ao pressionar o botão do joystick, o contador de eventos (`contador`) é zerado. [cite: 67, 69]
* **Reset da Matriz NeoPixel:** A matriz NeoPixel é completamente apagada. [cite: 67]
* **Reset da Fila:** Os contadores da fila (`inicio`, `fim`, `quantidade`) são resetados para simular uma nova fila vazia. [cite: 68, 69]

Este projeto demonstra uma aplicação prática de foreground/background usando os dois núcleos da Raspberry Pi Pico W, tratamento de interrupções, e controle de periféricos como LEDs RGB e uma matriz NeoPixel.
