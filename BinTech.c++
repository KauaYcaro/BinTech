#include <LiquidCrystal.h>

//==== PORTAS ====
LiquidCrystal lcd(8,9,4,5,6,7);
#define TRIG 11
#define ECHO 10
#define BUTTONS A0

//==== MAGIC NUMBERS ====
#define DISTANCIA_SENSOR 5
#define CODIGOSIZE 5
#define DEBOUNCE 200
#define PONTOSPORITEM 100
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
String botao = "";
State estado = IDLE;

//=== FILTRO DE SENSOR ====
const int LEITURAS_CONSECUTIVAS = 3;
int contadorDeteccao = 0;

//==== FUNÇÕES DO SISTEMA ====
void exibirPontos() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Pontos:");
    lcd.setCursor(0,1);
    lcd.print(pontosSessao);
}

void responderDeteccao() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Detectado!");
    lcd.setCursor(0,1);
    lcd.print("Aguarde...");
    delay(DELAY_CURTO);
    pontosSessao += PONTOSPORITEM;
    exibirPontos();
}

void detectarItem() {
    long duracao;
    float distancia;

    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    duracao = pulseIn(ECHO, HIGH);
    distancia = duracao * 0.034 / 2;

    Serial.print("Distancia: ");
    Serial.println(distancia);

    if (distancia < DISTANCIA_SENSOR) {
        contadorDeteccao++;
    } else {
        contadorDeteccao = 0;
    }

    if (contadorDeteccao >= LEITURAS_CONSECUTIVAS) {
        responderDeteccao();
        contadorDeteccao = 0;
    }
}

void resetar() {
    codigoInput = "";
    indexAlunoAtual = -1;
    pontosSessao = 0;
    estado = IDLE;
}

//==== BOTÕES DO SHIELD ====
String encontrarBotao() {
    static String ultimoBotao = "";
    static unsigned long ultimoTempo = 0;
    String botaoAtual = "";

    int leitura = analogRead(BUTTONS);
    unsigned long agora = millis();

    if (leitura < 100) botaoAtual = "DIR";
    else if (leitura < 200) botaoAtual = "CIM";
    else if (leitura < 300) botaoAtual = "BAI";
    else if (leitura < 500) botaoAtual = "ESQ";
    else if (leitura < 700) botaoAtual = "SEL";
    else botaoAtual = "";

    if (botaoAtual != "" && botaoAtual != ultimoBotao && (agora - ultimoTempo) > DEBOUNCE) {
        ultimoBotao = botaoAtual;
        ultimoTempo = agora;
        return botaoAtual;
    }

    if (botaoAtual == "") {
        ultimoBotao = "";
    }

    return "";
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
int encontrarAluno(String codigoInput) {
    for(int i=0; i<quantidadeAlunos; i++) {
        if (aluno[i].codigo == codigoInput) return i;
    }
    return -2;
}

void finalizarSessao() {
    aluno[indexAlunoAtual].pontos += pontosSessao;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Sessao Encerrada");
    delay(DELAY_CURTO);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(aluno[indexAlunoAtual].nome);
    lcd.setCursor(0,1);
    lcd.print(aluno[indexAlunoAtual].pontos);
    lcd.print(" pts");
    delay(DELAY_LONGO);
    estado = IDLE;
}

//==== PROCESSAMENTO DE BOTÕES ====
void processarBotao() {
    botao = encontrarBotao();

    if (estado == IDLE || estado == LENDO) {
        if (botao == "CIM") codigoInput += "1";
        else if (botao == "DIR") codigoInput += "2";
        else if (botao == "ESQ") codigoInput += "3";
        else if (botao == "BAI") codigoInput += "4";

        if (botao != "") {
            estado = LENDO;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(codigoInput);
        }

        if (codigoInput.length() >= CODIGOSIZE) {
            indexAlunoAtual = encontrarAluno(codigoInput);
        }
    } else if (estado == ATIVO) {
        if (botao == "SEL") finalizarSessao();
    }
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
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("NOT FOUND");
                delay(DELAY_MEDIO);
                estado = IDLE;
                break;
            }
            break;
        case ATIVO:
            processarBotao();
            detectarItem();
            delay(50); // pequeno delay para não travar
            break;
    }
}
