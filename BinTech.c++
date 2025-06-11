//==== MÁQUINA DE ESTADO (SM) ====
//1. Os estados são usados como switch no void loop(), pra identificar em que ponto da operação o usuário está.
//2. Cada estado tem funções específicas, favor indicar.
//3. "IDLE" = Fazendo nada, mostrando mensagem inicial.
//4. "LENDO" = Recebendo o código do usuário.
//5. "ATIVO" = Contando itens e atribuindo pontos.

//==== ANOTAÇÕES ====
//1. Checar se as portas estão certinhas, e determinar as portas dos botões (BT1, BT2, BT3, BT4, SIM, NAO) e do buzzer (BUZZER_PIN).
//Nenhum desses acima está definido no código, mas estão chamados com esse nome (!!!!)
//2. Ver o que fazer com o TRIG, ECHO, LED.
//3. Adicionar debounce em TODAS as operações com botão (!!!!)
//4. Colocar serial.print como debug nos pontos de contenção
//5. Colocar acknowlegements com buzzer e delays uniformemente nos pontos de contenção (fiz na bagunça)
//6. Agradecer Kaio

//==== PORTAS ====
#include <LiquidCrystal.h>
LiquidCrystal lcd(8,9,4,5,6,7);
#define TRIG 11
#define ECHO 10
#define LED 12
#define BUZZER_PIN 13

//==== MAGIC NUMBERS ====
#define DISTANCIA_SENSOR 30
#define CODIGOSIZE 5
#define DEBOUNCE 200
#define PONTOSPORITEM 100
//bipes
#define BEEP_CURTO 80
#define BEEP_MEDIO 250
#define BEEP_LONGO 700
//delays
#define DELAY_CURTO 2000
#define DELAY_MEDIO 5000
#define DELAY_LONGO 20000

//==== BANCO DE ALUNOS ====
struct Aluno {
    String codigo;
    String nome;
    String turma;
    int pontos;
};
Aluno aluno[] = {
    {"12341", "Kauã Y", "3° TDS A", 0},
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
void resetar() { //Volta as variáveis pro default
    codigoInput = "";
    indexAlunoAtual = -1; //Nenhum
    pontosSessao = 0;
    estado = IDLE;
}

void beep(int tempo) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(tempo);
    digitalWrite(BUZZER_PIN, LOW);
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

void processarBotao() { //experimental
    if (estado == IDLE || estado == LENDO) {
        if (digitalRead(BT1) == LOW) {
            codigoInput += "1";
            estado = LENDO;
            beep(BEEP_CURTO);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(codigoInput);
            delay(DEBOUNCE);
        } else if (digitalRead(BT2) == LOW) {
            codigoInput += "2";
            estado = LENDO;
            beep(BEEP_CURTO);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(codigoInput);
            delay(DEBOUNCE);
        } else if (digitalRead(BT3) == LOW) {
            codigoInput += "3";
            estado = LENDO;
            beep(BEEP_CURTO);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(codigoInput);
            delay(DEBOUNCE);
        } else if (digitalRead(BT4) == LOW) {
            codigoInput += "4";
            estado = LENDO;
            beep(BEEP_CURTO);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(codigoInput);
            delay(DEBOUNCE);
        } else if (digitalRead(NAO) == LOW) {
            triplebeep();
            estado = IDLE; //aqui, o comportamento é RESETAR PARA IDLE
        } else if (digitalRead(SIM) == LOW) {
            //sem comportamento definido para SIM no estado IDLE ou LENDO
        }
        if (codigoInput.length >= CODIGOSIZE) {
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
    } else { //se o caso for ATIVO
        if (digitalRead(BT1) == LOW) {
            //sem comportamento definido para numéricos em ATIVO
        } else if (digitalRead(BT2) == LOW) {
            //sem comportamento definido para numéricos em ATIVO
        } else if (digitalRead(BT3) == LOW) {
            //sem comportamento definido para numéricos em ATIVO
        } else if (digitalRead(BT4) == LOW) {
            //sem comportamento definido para numéricos en ATIVO
        } else if (digitalRead(SIM) == LOW) {
            finalizarSessao();
        } else if (digitalRead(NAO) == LOW) {
            triplebeep();
            estado = IDLE; //comportamento = RESETAR PARA IDLE
        }
    }
}

int encontrarAluno(String codigoInput) {
    for(int i=0; i<quantidadeAlunos; i++) {
        if (aluno[i].codigo == codigoInput) {
            return i;
        }
    }
    return -2; //Não encontrado
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
        digitalWrite(LED, HIGH); //legacy
        responderDeteccao();
    } else {
        digitalWrite(LED, LOW); //legacy
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

//==== SETUP E LOOP ==== (manter no final)
void setup() {
    Serial.begin(9600);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    pinMode(LED, OUTPUT);
    
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
            if (indexAlunoAtual != -1 && indexAlunoAtual != -2) { //-1 = resetado, -2 = não encontrado
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
            } else if (indexAlunoAtual == -2) { //aluno não encontrado
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
            if (digitalRead(NAO) == HIGH) {
                detectarItem();
            }
            delay(DEBOUNCE);
            break;
    }
}
