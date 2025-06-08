//Tentativa no. 3
//Tempo gasto: 1h36min ;)

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

#include <LiquidCrystal.h>
LiquidCrystal lcd(8,9,4,5,6,7);
#define TRIG 11;
#define ECHO 10;
#define LED 12;

enum State{"IDLE", "LENDO", "ATIVO"};

struct Aluno {
    String codigo;
    String nome;
    String turma;
    int pontos;
};
Aluno aluno[] {
    {"12341", "Kauã Y", "3° TDS A", 0},
    {"11424", "V Daniel", "3° TDS A", 0},
    {"12234", "Kaio A", "3° TDS A", 0},
    {"11122", "Ingridy N", "3° TDS A", 0},
    {"12212", "João G", "3° TDS A", 0},
    {"22143", "Lorenna P", "3° TDS A", 0},
    {"22341", "Eduardo N", "3° TDS A", 0}
};

//==== FUNÇÕES GLOBAIS ====

void default() { //Volta as variáveis pro default
    String codigoInput = "";
    int indexAlunoAtual = -1; //Nenhum
    int pontosSessao = 0;
    State estado = "IDLE";
}

void beep(int tempo) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(tempo);
    digitalWrite(BUZZER_PIN, LOW);
}

//==== FUNÇÕES IDLE ====
void exibirMensagemInicial() {
    lcd.clear;
    lcd.setCursor(0,0);
    lcd.print("BinTech");
    lcd.setCursor(0,1);
    lcd.print("Inserir código");
}

void idleRouter() { //Roteia para outro modo 
    if (digitalRead(BT1) == LOW) {
        codigoInput += "1";
        State estado = "LENDO";
    } else if (digitalRead(BT2) == LOW) {
        codigoInput += "2";
        State estado = "LENDO";
    } else if (digitalRead(BT3) == LOW) {
        codigoInput += "3";
        State estado = "LENDO";
    } else if (digitalRead(BT4) == LOW) {
        codigoInput += "4";
        State estado = "LENDO"
    }
}

//==== FUNÇÕES LENDO ====

void lerCodigo() {
    while (codigoInput.length() < 5) {
        if (digitalRead(BT1) == LOW) {
            codigoInput += "1";
        } else if (digitalRead(BT2) == LOW) {
            codigoInput += "2";
        } else if (digitalRead(BT3) == LOW) {
            codigoInput += "3";
        } else if (digitakRead(BT4) == LOW) {
            codigoInput += "4";
        }
    }
}

int encontrarAluno(String codigoInput) {
    for(int i=0; i<7; i++) { //7 alunos é hardcoded
        if (aluno[i].codigo == codigoInput) {
            return i;
        }
    }
    return -1; //Não encontrado
}

//==== FUNÇÕES ATIVO ====

void responderDeteccao {
    beep(500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Detectado!")
    lcd.setCursor(0,1);
    lcd.print("Aguarde...");
    pontosSessao += 100;
    exibirPontos();
}

void exibirPontos() {
    beep(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Pontos:");
    lcd.setCursor(0,1);
    lcd.print(pontosSessao);
}

//==== SETUP E LOOP ==== (manter no final)
void setup() {
    Serial.begin(9600);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    pinMode(LED, OUTPUT);
    
    lcd.begin(16,2);
    exibirMensagemInicial();
    default();
}

void loop() {
    switch(estado) {
        case "IDLE":
            exibirMensagemInicial();
            idleRouter();
            break;
        case "LENDO:
            lerCodigo();
            int indexAlunoAtual = encontrarAluno(codigoInput); //encontrou o aluno
            if (indexAlunoAtual != -1) {
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Olá,");
                lcd.setCursor(0,1);
                lcd.print(aluno[indexAlunoAtual].nome);
                delay(4000);
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("ATIVO");
                State estado = "ATIVO";
                break;
            } else {
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("NOT FOUND");
                delay(5000);
                State estado = "IDLE";
                break;
            }
        case "ATIVO":
            if (digitalRead(NAO) == HIGH) {
                long duracao;
                float distancia;

                digitalWrite(TRIG, LOW);
                delayMicroseconds(0.1);
                digitalWrite(TRIG, HIGH);
                delayMicroseconds(1);
                digitalWrite(TRIG, LOW);

                duracao = pulseIn(ECHO, HIGH);
                distancia = duracao*0.034/2

                Serial.print("Distancia: ");
                Serial.print(distancia);
                Serial.print("cm.");

                if (distancia < 30) {
                    digitalWrite(LED, HIGH); //legacy
                    responderDeteccao();
                } else {
                    digitalWrite(LED, LOW); //legacy
                }
            } else if (digitalRead(SIM) == LOW) {
                aluno[indexAlunoAtual].pontos += pontosSessao;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Sessão Encerrada");
                delay(2000);
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print(aluno[indexAlunoAtual].nome];
                lcd.setCursor(0,1);
                lcd.print(aluno[indexAlunoAtual].pontos];
                lcd.print("pts");
                delay(20000);
                State estado = "IDLE";
            } else if (digitalRead(NAO) == LOW) {
                //exibe alguma mensagem
                State estado = "IDLE";
            }
            delay(200);
            break;
    }
}
