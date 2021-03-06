#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(9, 7, 8, 12, 10, 11);
#define LED_Alerta 6 // Pin is connected to the LED ALERTA
#define LED_Direito 5 // Pin is connected to the LED DIREITO
#define LED_Esquerdo 13 // Pin is connected to the LED ESQUERDO
// ponteiro de função da máquina de estados.
// Ele aponta sempre para a função da máquina de estados que deve ser executada
void(* PonteiroDeFuncao) ();
String flag;
int acaba = 3;
int desce = 4;
int sobe = 2;
int corrente_CH12 = 40;
int corrente_CH34 = 50;
int corrente_CH56 = 40;
int corrente_CH78 = 50;
int freq = 50;
int mode = 2;
bool stim = true;
bool aux_CH12 = false;
bool aux_CH34 = false;
int pw = 500;
int passo;
int variavel;
String text;
String unidade;
bool print;
byte channels = B00000000;
byte set_channels = B00000000;
// B00000000 = Estado Inicial
// B00000011 = Extensão 
// B00001100 = Flexão
// B00001111 = Extensão + Flexão
// B00110000 = -
// B00110011 = (Extensão & Aux_Ext)
// B00111100 = -
// B00111111 = (Extensão & Aux_Ext) + Flexão
// B11000000 = -
// B11000011 = -
// B11001100 = (Flexão & Aux_Flex)
// B11001111 = Extensao + (Flexão & Aux_Flex)
// B11110000 = -
// B11110011 = -
// B11111100 = -
// B11111111 = (Extensão & Aux_Ext) + (Flexão & Aux_Flex)



void setup()
{
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial.flush(); // Clear receive buffer.
  pinMode(acaba, INPUT_PULLUP);
  pinMode(desce, INPUT_PULLUP);
  pinMode(sobe, INPUT_PULLUP);
  pinMode(LED_Alerta, OUTPUT); // Sets pin as OUTPUT.
  pinMode(LED_Direito, OUTPUT); // Sets pin as OUTPUT.
  pinMode(LED_Esquerdo, OUTPUT); // Sets pin as OUTPUT.
}

void loop()
{
  // put your main code here, to run repeatedly:
  PonteiroDeFuncao = StateConect;
  // aponta para o estado inicial. Nunca esquecer de informar um estado inicial
  // (senão, nesse caso em específico, pode haver um erro fatal / hard fault).
  while (1)
  {
    (* PonteiroDeFuncao) ();
    // chama a função apontada pelo ponteiro de função (logo, chama o estado corrente)
  }
  system("PAUSE");
  //return 0;
}

void StateConect(void)
{
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(" EMA FES-Rowing ");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("   Conectando...");
  // verificando conexão ////////////////////////////////////////////////////////////
  bool conexao = false;
  while (conexao == false)
  {
    if (Serial.available())
    {
      flag = Serial.readString();
      if (flag.equals("a"))
      {
        conexao = true;
        PonteiroDeFuncao = StateSetCh;
      }
    }
  }
}

void StateSetCh(void)
{
  // para a funcao
  // int mode = 2;
  text = "Sel.Movimento(s):";
  lcd.clear();
  lcd.print(text);
  variavel = mode;
  passo = 2;
  unidade = "";
  print = false;
  mode = func_set_parametro(variavel, passo, text, unidade, print);
  if (mode == 4)
  {
    lcd.setCursor(0, 1);
    lcd.print("Extensao+flexao");
    channels = B00001111;
  }
  else
    if (mode == 2)
    {
      lcd.setCursor(0, 1);
      lcd.print("Extensao       ");
      channels = B00000011;
    }
  else
    if (mode == 0)
    {
      lcd.setCursor(0, 1);
      lcd.print("         flexao");
      channels = B00001100;
    }
  else
    mode = 2;
  delay(100);
  if (digitalRead(acaba) == LOW)
  {
    if (mode == 2)
      PonteiroDeFuncao = SetAuxCH34;
    else
      PonteiroDeFuncao = SetAuxCH12;
    delay(250);
  }
}

void SetAuxCH12(void)
{
  text = "Ext.c/ Auxiliar?";
  lcd.clear();
  lcd.print(text);
  passo = 1;
  unidade = "";
  variavel = aux_CH12;
  print = false;
  variavel = func_set_parametro(variavel, passo, text, unidade, print);
  if (variavel == 0)
  {
    aux_CH12 = false;
    lcd.setCursor(0, 1);
    lcd.print("Nao             ");
    set_channels = B00000000;
    delay(100);
  }
  if (variavel == 1)
  {
    aux_CH12 = true;
    lcd.setCursor(0, 1);
    lcd.print("Sim, CH 5/6");
    set_channels = B00110000;
    delay(100);
  }
  if (digitalRead(acaba) == LOW)
  {
    channels = channels | set_channels;
    if (mode == 1)
      PonteiroDeFuncao = StateSetAmp;
    else
      PonteiroDeFuncao = SetAuxCH34;
    delay(250);
  }
}

void SetAuxCH34(void)
{
  text = "Flex.c/ Auxiliar?";
  lcd.clear();
  lcd.print(text);
  passo = 1;
  unidade = "";
  variavel = aux_CH34;
  print = false;
  variavel = func_set_parametro(variavel, passo, text, unidade, print);
  if (variavel == 0)
  {
    aux_CH34 = false;
    lcd.setCursor(0, 1);
    lcd.print("Nao             ");
    set_channels = B00000000;
    delay(100);
  }
  if (variavel == 1)
  {
    aux_CH34 = true;
    lcd.setCursor(0, 1);
    lcd.print("Sim, CH 7/8");
    set_channels = B11000000;
    delay(100);
  }
  if (digitalRead(acaba) == LOW)
  {
    channels = channels | set_channels;
    PonteiroDeFuncao = StateSetAmp;
  }
  delay(250);
}

void StateSetAmp(void)
{
  if ((B11000000 & channels) > 0)
  {
    digitalWrite(acaba, HIGH);
    delay(250);
    while (digitalRead(acaba) == HIGH)
    {
      text = "CorrenteExtensao";
      variavel = corrente_CH12;
      passo = 2;
      unidade = "mA (CH [1,2])";
      print = true;
      corrente_CH12 = func_set_parametro(variavel, passo, text, unidade, print);
      delay(100);
    }
  }
  if ((B00110000 & channels) > 0)
  {
    digitalWrite(acaba, HIGH);
    delay(250);
    corrente_CH56 = corrente_CH12;
    while (digitalRead(acaba) == HIGH)
    {
      text = "Corrente Aux Ext";
      variavel = corrente_CH56;
      passo = 2;
      unidade = "mA (CH [5,6])";
      print = true;
      corrente_CH56 = func_set_parametro(variavel, passo, text, unidade, print);
      delay(100);
    }
  }
  if ((B00001100 & channels) > 0)
  {
    digitalWrite(acaba, HIGH);
    delay(250);
    while (digitalRead(acaba) == HIGH)
    {
      text = "Corrente Flexao";
      variavel = corrente_CH34;
      passo = 2;
      unidade = "mA (CH [3,4])";
      print = true;
      corrente_CH34 = func_set_parametro(variavel, passo, text, unidade, print);
      delay(100);
    }
  }
  if ((B11000000 & channels) > 0)
  {
    digitalWrite(acaba, HIGH);
    delay(250);
    corrente_CH78 = corrente_CH34;
    while (digitalRead(acaba) == HIGH)
    {
      text = "Corrente AuxFlex";
      variavel = corrente_CH78;
      passo = 2;
      unidade = "mA (CH [7,8])";
      print = true;
      corrente_CH78 = func_set_parametro(variavel, passo, text, unidade, print);
      delay(100);
    }
  }
  PonteiroDeFuncao = StateSetLP;
}

void StateSetLP(void)
{
  // largura de pulso
  text = "Largura de Pulso:";
  variavel = pw;
  passo = 10;
  unidade = "ms";
  print = true;
  pw = func_set_parametro(variavel, passo, text, unidade, print);
  delay(500);
  if (digitalRead(acaba) == LOW)
  {
    PonteiroDeFuncao = StateSetFreq;
    digitalWrite(acaba, HIGH);
  }
}

void StateSetFreq()
{
  // Frequencia
  text = "Frequencia:";
  variavel = freq;
  passo = 5;
  unidade = "Hz";
  print = true;
  freq = func_set_parametro(variavel, passo, text, unidade, print);
  delay(500);
  if (digitalRead(acaba) == LOW)
  {
    PonteiroDeFuncao = Send;
    digitalWrite(acaba, HIGH);
  }
}

void Send(void)
{
  // enviando dados pela serial (bluetooth) //////////////////////////////////////////////////////////////
  Serial.print("c"); // marcador de corrente_quad
  func_dim_string_to_Stim(corrente_CH12);
  Serial.print("x"); // marcador de corrente_isq
  func_dim_string_to_Stim(corrente_CH34);
  Serial.print("p"); // marcador de largura de pulso
  func_dim_string_to_Stim(pw);
  Serial.print("f"); // marcador de frequecia
  func_dim_string_to_Stim(freq);
  Serial.print("m"); // marcador do modo
  func_dim_string_to_Stim(mode);
  lcd.clear();
  lcd.print("Enviando");
  delay(3000);
  PonteiroDeFuncao = waiting;
}

void waiting(void)
{
  if (Serial.available())
  {
    flag = Serial.readString();
    // lcd.print (flag);
    if (flag.equals("a"))
    {
      lcd.clear();
      lcd.print("Iniciando estimulacao");
      delay(1000);
      PonteiroDeFuncao = Runing;
    }
  }
}

void Runing(void)
{
  int state_0 = 0;
  int state_1 = 1;
  digitalWrite(LED_Alerta, LOW);
  while (stim)
  {
    if (digitalRead(sobe) == LOW)
    {
      digitalWrite(sobe, HIGH);
      state_0 = 1;
      lcd.clear();
      lcd.print("Extensao");
      digitalWrite(LED_Direito, HIGH);
    }
    else
      if (digitalRead(desce) == LOW)
      {
        digitalWrite(desce, HIGH);
        state_0 = 2;
        lcd.clear();
        lcd.print("Flexao");
        digitalWrite(LED_Esquerdo, HIGH);
      }
    else
      if (digitalRead(acaba) == LOW)
      {
        digitalWrite(acaba, HIGH);
        state_0 = 3;
      }
    else
    {
      state_0 = 0;
    }
    if (state_1 != state_0)
    {
      Serial.print(state_0);
      state_1 = state_0;
      lcd.clear();
      lcd.print("    Pressione   ");
      lcd.setCursor(0, 1);
      lcd.print("<-Flex     Ext->");
      digitalWrite(LED_Esquerdo, LOW);
      digitalWrite(LED_Direito, LOW);
    }
    if (state_0 == 3)
    {
      Serial.print(state_0);
      stim = false;
      lcd.clear();
      lcd.print("Fim");
    }
    delay(30);
  }
  delay(500);
  digitalWrite(LED_Alerta, HIGH);
  delay(500);
}

// //////////////////////////////////////////////////////////////////////////////////////
int func_set_parametro(int variavel, int passo, String text, String unidade, bool print)
{
  int BTN_Down = HIGH;
  int BTN_Up = HIGH;
  int lastReadingDown = HIGH;
  int lastReadingUp = HIGH;
  long lastSwitchTimeDown = 0;
  long lastSwitchTimeUp = 0;
  long longSwitchTime = 500;
  long shortSwitchTime = 100;
  long switchTimeDown = 500;
  long switchTimeUp = 500;
  BTN_Up = digitalRead(sobe);
  BTN_Down = digitalRead(desce);
  if (print == true)
  {
    lcd.clear();
    lcd.print(text);
    // while (digitalRead(acaba) == HIGH)
    // {
    lcd.setCursor(0, 1);
    lcd.print(variavel);
    lcd.print(" ");
    lcd.print(unidade);
    lcd.print("   ");
    delay(100);
  }
  if (BTN_Up == LOW && ((((millis() - lastSwitchTimeUp) > switchTimeUp) && lastSwitchTimeUp != 0) || lastReadingUp == HIGH))
  {
    // digitalWrite (sobe, HIGH);
    if (((millis() - lastSwitchTimeUp) > switchTimeUp) && lastSwitchTimeUp != 0)
    {
      switchTimeUp = shortSwitchTime;
    }
    lastSwitchTimeUp = millis();
    lastReadingUp = BTN_Up;
    variavel = variavel + passo;
    delay(100);
  }
  if (BTN_Up == HIGH)
  {
    lastReadingUp = BTN_Up;
    switchTimeUp = longSwitchTime;
    lastSwitchTimeUp = 0;
  }
  if (BTN_Down == LOW && ((((millis() - lastSwitchTimeDown) > switchTimeDown) && lastSwitchTimeDown != 0) || lastReadingDown == HIGH))
  {
    // digitalWrite (desce, HIGH);
    if (((millis() - lastSwitchTimeDown) > switchTimeDown) && lastSwitchTimeDown != 0)
    {
      switchTimeDown = shortSwitchTime;
    }
    lastSwitchTimeDown = millis();
    lastReadingDown = BTN_Down;
    variavel = variavel - passo;
    if (variavel < 0)
    {
      variavel = 0;
    }
    delay(100);
  }
  if (BTN_Down == HIGH)
  {
    lastReadingDown = BTN_Down;
    switchTimeDown = longSwitchTime;
    lastSwitchTimeDown = 0;
  }
  // }
  return variavel;
}

/**
[func_dim_string_to_Stim Configura o envio dos parametros sempre com 3 algarismo]
@param  {[int]}  entrada       [Parametro a ser convertido]
@return {[int]}     [Parâmetro convertido em 3 algarismo]
*/
void func_dim_string_to_Stim(int entrada)
{
  if (qtdAlgarismos(entrada) == 3)
  {
    Serial.print(entrada);
  }
  else
    if (qtdAlgarismos(entrada) == 2)
    {
      Serial.print(0);
      Serial.print(entrada);
    }
  else
    if (qtdAlgarismos(entrada) == 1)
    {
      Serial.print(0);
      Serial.print(0);
      Serial.print(entrada);
    }
}

int qtdAlgarismos(int numero)
{
  int cont = 0;
  while (numero != 0)
  {
    // n = n/10
    numero /= 10;
    cont++;
  }
  return cont;
}
