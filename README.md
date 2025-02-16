<img width=100% src="https://capsule-render.vercel.app/api?type=waving&color=02A6F4&height=120&section=header"/>
<h1 align="center">Embarcatec | Conversores analógico-digitais (ADC)</h1>

<div align="center">  
  <img width=40% src="http://img.shields.io/static/v1?label=STATUS&message=EM%20DESENVOLVIMENTO&color=02A6F4&style=for-the-badge"/>
</div>

## Objetivo do Projeto

Compreender o funcionamento do conversor analógico-digital (ADC) no RP2040.
Utilizar o PWM para controlar a intensidade de dois LEDs RGB com base nos valores do joystick.
Representar a posição do joystick no display SSD1306 por meio de um quadrado móvel.
Aplicar o protocolo de comunicação I2C na integração com o display. 

## 🗒️ Lista de requisitos

• LED RGB, com os pinos conectados às GPIOs (11, 12 e 13).
• Botão do Joystick conectado à GPIO 22.
• Joystick conectado aos GPIOs 26 e 27.
• Botão A conectado à GPIO 5.
• Display SSD1306 conectado via I2C (GPIO 14 e GPIO15).

## 🛠 Tecnologias

1. **Git e Github**;
2. **VScode**;
3. **Linguagem C**;

## 🔧 Funcionalidades Implementadas:

O joystick fornecerá valores analógicos correspondentes aos eixos X e Y, que serão utilizados para:

Controlar a intensidade luminosa dos LEDs RGB, onde:

O LED Azul terá seu brilho ajustado conforme o valor do eixo Y. Quando o joystick estiver solto (posição central - valor 2048), o LED permanecerá apagado.
À medida que o joystick for movido para cima (valores menores) ou para baixo (valores maiores), o LED aumentará seu brilho gradualmente, atingindo a intensidade máxima nos extremos (0 e 4095).
O LED Vermelho seguirá o mesmo princípio, mas de acordo com o eixo X. Quando o joystick estiver solto (posição central - valor 2048), o LED estará apagado. Movendo o joystick para a esquerda
(valores menores) ou para a direita (valores maiores), o LED aumentará de brilho, sendo mais intenso nos extremos (0 e 4095).
Os LEDs serão controlados via PWM para permitir variação suave da intensidade luminosa.
Exibir no display SSD1306 um quadrado de 8x8 pixels, inicialmente centralizado, que se moverá proporcionalmente aos valores capturados pelo joystick.

Adicionalmente, o botão do joystick terá as seguintes funcionalidades:
•Alternar o estado do LED Verde a cada acionamento.
•Modificar a borda do display para indicar quando foi pressionado, alternando entre diferentes estilos de borda a cada novo acionamento.
Finalmente, o botão A terá a seguinte funcionalidade:
Ativar ou desativar os LED PWM a cada acionamento.

## 💻 Desenvolvedor
 
<table>
  <tr>
    <td align="center"><img style="" src="https://avatars.githubusercontent.com/u/72825281?v=4" width="100px;" alt=""/><br /><sub><b> Marcio Fonseca </b></sub></a><br />👨‍💻</a></td>
  </tr>
</table>


## 🎥 Demonstração: 
- Para ver o funcionamento do projeto, acesse o vídeo:

https://youtu.be/z_5jYnsAuOY

=======
# embarcatec3
