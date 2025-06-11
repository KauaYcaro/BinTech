//==== PORTAS ====
#include <LiquidCrystal.h>
LiquidCrystal lcd(8,9,4,5,6,7);
#define TRIG 11
#define ECHO 10

//==== VARIÁVEIS DO BOTÃO ====
String botao = "";
bool botaoApertado = false;

//==== MAGIC NUMBERS ====
#define DISTANCIA_SENSOR 30
#define CODIGOSIZE 5
#define DEBOUNCE 200
#define PONTOSPORITEM 100
#define BEEP_CURTO 80
#define BEEP_MEDIO 250
#define BEEP_LONGO 700
#define DELAY_CURTO 2000
#define DELAY_MEDIO 5000
#define DELAY_LONGO 10000

//==== BANCO DE ALUNOS ====
struct Aluno {
    String codigo;
    String nome;
    String turma;
    int pontos;
};
Aluno aluno[] = {
    {"33333", "Kauã Y", "3° TDS A", 0},
    {"11424", "V Daniel", "3° TDS A", 0},
    {"12234", "Kaio A", "3° TDS A", 0},
    {"11122", "Ingridy N", "3° TDS A", 0},
    {"12212", "João G", "3° TDS A", 0},
    {"22143", "Lorenna P", "3° TDS A", 0},
    {"22341", "Eduardo N", "3° TDS A", 0}
};
int quantidadeAlunos = sizeof(aluno)/sizeof(aluno[0]);

//==== VARIÁVEIS GLOBAIS ====
enum State{IDLE, LENDO, ATIVO};
String codigoInput = "";
int indexAlunoAtual = -1;
int pontosSessao = 0;
State estado = IDLE;

//==== FUNÇÕES DO SISTEMA ====

void lerBotao() {
  int leitura = analogRead(A0);
  String leituraAtual = "";

  if (leitura < 50) leituraAtual = "3";
  else if (leitura < 195) leituraAtual = "1";
  else if (leitura < 380) leituraAtual = "4";
  else if (leitura < 555) leituraAtual = "2";
  else if (leitura < 790) leituraAtual = "SIM";

  if (leituraAtual != "" && !botaoApertado) {
    botao = leituraAtual;
    botaoApertado = true;
    delay(DEBOUNCE);
  } else if (leituraAtual == "") {
    botaoApertado = false; // Botão foi solto
    botao = "";
  } else {
    botao = ""; // Evita repetir o mesmo botão
  }
}

void resetar() {
    codigoInput = "";
    indexAlunoAtual = -1;
    pontosSessao = 0;
    estado = IDLE;
}

void beep(int tempo) {
    Serial.print("Ia fazer um buzzer aq");
    delay(tempo);
}

void triplebeep() {
    for(int i=0; i<3; i++) {
        beep(BEEP_CURTO);
        delay(100);
    }
}

//==== FUNÇÕES IDLE ====
void exibirMensagemInicial() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("BinTech");
    lcd.setCursor(0,1);
    lcd.print("Inserir código");
}

//==== FUNÇÕES LENDO ====
void processarBotao() {
    lerBotao();
    if (botao == "") return;

    if (estado == IDLE || estado == LENDO) {
        if (botao == "1" || botao == "2" || botao == "3" || botao == "4") {
            codigoInput += botao;
            estado = LENDO;
            beep(BEEP_CURTO);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(codigoInput);
        } else if (botao == "SIM") {
            // Nenhuma ação definida no estado IDLE/LENDO
        }

        if (codigoInput.length() >= CODIGOSIZE) {
            if (codigoInput == "11111") {
                for (int i=0; i < quantidadeAlunos; i++) {
                    Serial.print(aluno[i].nome);
                    Serial.print(",");
                    Serial.print(aluno[i].turma);
                    Serial.print(",");
                    Serial.print(aluno[i].pontos);
                    Serial.println();
                }
            }
            indexAlunoAtual = encontrarAluno(codigoInput);
        }
    } else if (estado == ATIVO) {
        if (botao == "SIM") {
            finalizarSessao();
        }
    }
}

int encontrarAluno(String codigoInput) {
    for(int i=0; i<quantidadeAlunos; i++) {
        if (aluno[i].codigo == codigoInput) {
            return i;
        }
    }
    return -2;
}

//==== FUNÇÕES ATIVO ====

void detectarItem() {
    long duracao;
    float distancia;

    digitalWrite(TRIG, LOW);
    delayMicroseconds(0.1);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(1);
    digitalWrite(TRIG, LOW);

    duracao = pulseIn(ECHO, HIGH);
    distancia = duracao*0.034/2;

    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.print("cm.");

    if (distancia < DISTANCIA_SENSOR) {
        Serial.write("LED");
        responderDeteccao();
    }
}

void responderDeteccao() {
    beep(BEEP_MEDIO);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Detectado!");
    lcd.setCursor(0,1);
    lcd.print("Aguarde...");
    delay(DELAY_CURTO);
    pontosSessao += PONTOSPORITEM;
    exibirPontos();
}

void exibirPontos() {
    beep(BEEP_LONGO);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Pontos:");
    lcd.setCursor(0,1);
    lcd.print(pontosSessao);
}

void finalizarSessao() {
    beep(BEEP_LONGO);
    aluno[indexAlunoAtual].pontos += pontosSessao;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Sessão Encerrada");
    delay(DELAY_CURTO);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(aluno[indexAlunoAtual].nome);
    lcd.setCursor(0,1);
    lcd.print(aluno[indexAlunoAtual].pontos);
    lcd.print("pts");
    delay(DELAY_LONGO);
    estado = IDLE;
}

//==== SETUP E LOOP ====
void setup() {
    Serial.begin(9600);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    
    lcd.begin(16,2);
    exibirMensagemInicial();
    resetar();
}

void loop() {
    switch(estado) {
        case IDLE:
            resetar();
            exibirMensagemInicial();
            processarBotao();
            break;
        case LENDO:
            processarBotao();
            if (indexAlunoAtual != -1 && indexAlunoAtual != -2) {
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Olá,");
                lcd.setCursor(0,1);
                lcd.print(aluno[indexAlunoAtual].nome);
                delay(DELAY_MEDIO);
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("ATIVO");
                estado = ATIVO;
                break;
            } else if (indexAlunoAtual == -2) {
                triplebeep();
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("NOT FOUND");
                delay(DELAY_MEDIO);
                beep(BEEP_LONGO);
                estado = IDLE;
                break;
            }
        case ATIVO:
            processarBotao();
            detectarItem();
            delay(DEBOUNCE);
            break;
    }
}
