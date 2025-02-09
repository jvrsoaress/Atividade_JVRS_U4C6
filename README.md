<img width=100% src="https://capsule-render.vercel.app/api?type=waving&color=02A6F4&height=120&section=header"/>
<h1 align="center">Embarcatech Atividade - U4C6 </h1>

## Objetivo do Projeto


Este projeto tem como objetivo aplicar os conceitos de comunicação serial (UART e I2C) no microcontrolador RP2040, utilizando a placa BitDogLab. Será desenvolvido um sistema que controla LEDs comuns e endereçáveis WS2812, com acionamento por botões. O projeto incluirá o uso de interrupções e debounce para garantir o controle eficiente dos LEDs e o acionamento preciso dos botões, integrando hardware e software.

## 🗒️ Lista de requisitos

- **Uso de interrupções**: Todas as funcionalidades relacionadas aos botões devem ser implementadas utilizando rotinas de interrupção (IRQ); 
- **Debouncing**: É obrigatório implementar o tratamento do bouncing dos botões via software; 
- **Controle de LEDs**: O projeto deve incluir o uso de LEDs comuns e LEDs WS2812, demonstrando o domínio de diferentes tipos de controle;
- **Utilização do Display 128 x 64**: A utilização de fontes maiúsculas e minúsculas demonstrará o domínio do uso de bibliotecas, o entendimento do princípio de funcionamento do display, bem como, a utilização do protocolo I2C.
- **Envio de informação pela UART**: Visa observar a compreensão sobre a comunicação serial via UART. 
- **Organização do código**: O código deve estar bem estruturado e comentado para facilitar o entendimento.

## 🛠 Tecnologias

- **Microcontrolador:** Raspberry Pi Pico W  
- **LEDs:** RGB (Vermelho, Verde, Azul)  
- **Botão A**: Conectado à GPIO 5. 
- **Botão B**: Conectado à GPIO 6. 
- **Display SSD1306**: Conectado via I2C (GPIO 14 e GPIO15).   
- **Linguagem de Programação:** C  
- **Frameworks:** Pico SDK


## 🔧 Funcionalidades Implementadas:

1. **Modificação da Biblioteca font.h** 
- Adicionar caracteres minúsculos à biblioteca font.h. Use sua criatividade para criar os novos 
caracteres. 
2. **Entrada de caracteres via PC**  
- Utilize o Serial Monitor do VS Code para digitar os caracteres. 
- Cada caractere digitado no Serial Monitor deve ser exibido no display SSD1306. 
- Observação: Apenas um caractere será enviado de cada vez, não é necessário suportar o envio de strings completas. 
- Quando um número entre 0 e 9 for digitado, um símbolo correspondente ao número deve 
ser exibido, também, na matriz 5x5 WS2812.  
3. **Interação com o Botão A**  
- Pressionar o botão A deve alternar o estado do LED RGB Verde (ligado/desligado). 
- A operação deve ser registrada de duas formas: 
- Uma mensagem informativa sobre o estado do LED deve ser exibida no display SSD1306. 
- Um texto descritivo sobre a operação deve ser enviado ao Serial Monitor. 
4. **Interação com o Botão B** 
- Pressionar o botão A deve alternar o estado do LED RGB Azul (ligado/desligado). 
- A operação deve ser registrada de duas formas: 
- Uma mensagem informativa sobre o estado do LED deve ser exibida no display SSD1306. 
- Um texto descritivo sobre a operação deve ser enviado ao Serial Monitor. 

## 🚀 Passos para Compilação e Upload  

1. **Clonar o repositório** 

- sh
- git clone seu repositorio
- cd semaforo-pico


2. **Configurar e compilar o projeto**  

`mkdir build && cd build`
`cmake ..`
`make`

3. **Transferir o firmware para a placa**

- Conectar a placa BitDogLab ao computador
- Copiar o arquivo .uf2 gerado para o drive da placa

4. **Testar o projeto**

🛠🔧🛠🔧🛠🔧


## 🎥 Demonstração: 

- Para ver o funcionamento do projeto, acesse o vídeo de demonstração gravado por José Vinicius em: https://youtu.be/FktqQ6QNLEA

## 💻 Desenvolvedor
 
<table>
  <tr>
    <td align="center"><img style="" src="https://avatars.githubusercontent.com/u/191687774?v=4" width="100px;" alt=""/><br /><sub><b> José Vinicius </b></sub></a><br />👨‍💻</a></td>
  </tr>
</table>
