# FES-Rowing EMA

` Atualização 16/11/2018`
` Arquivo com instruções de utilização`
` Versão Handle FES-Rowing comunicação USB`

---
## Descrição

### Arquivos

>`MainStim.py`
>>_Script_ a ser executado no **Raspberry**

>`stimulator.py`
>>Funções utilizadas no `ManStim.py`	

exemplo: 
```python
import stimulator
import serial
import serial.tools.list_ports
import time
import io
``` 

>`MainArduino_rowing.ino`
>>_Firmware_ Arduino NANO embarcado. `Comunicação SERIAL, informações no Display e comandos por botões pushbutton`

---

# Modo de Utilização 

> Atualmente o dispositivo está em fases de teste, por isso o sistema ainda funciona dependente de periféricos (Monitor, teclado e mouse) no Raspberry.

## Conexão
>* Conectar periféricos[^1], Eletroestimulador e Handle nas portas USB do Raspberry
Obs: não importa a ordem/posição dos dispositivos, a rotina desenvolvida para a abertura de portas de comunicação identifica de forma automática (_plug and play_).
*  Conectar **Raspberry** na fonte **DC [5 V, 2 A]** (Necessita de uma boa fonte para alimentar o consumo energético do _Handle_)
* Entrar no sistema operacional e executar o arquivo `MainStim.py`

## Passos
`O dispositivo Handle dispõe de 3 botões:`

>|Botões|Modo SET|Modo RUN|
|:--|:--|:--|
|Direito [verde]|Aumenta valor do parâmetro|Extensão da Perna|
|Esquerdo [verde]|Diminui o valor do parâmetro|Flexão da perna|
|Vermelho|Passa para o próximo parâmetro|Botão de Emergência|



>>1. Aguardar a conexão de portas e indicação no display para configuração dos parâmetros.
1. Inserir parâmetros:
	1. inserir a quantidades de canais (dois ou quatro).
	1. inserir valor(es) de amplitude de corrente.
	1. inserir valor da Largura de pulso.
	1. inserir valor da frequência.

>>1. Aguardar o envio de parâmetros para o Eletroestimulador.
1. Execução do treino.

