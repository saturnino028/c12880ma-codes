/*
 * Macro Definitions
 */
#define SPEC_TRG         A0
#define SPEC_ST          A1
#define SPEC_CLK         A2
#define SPEC_VIDEO       A3
#define WHITE_LED        A4
#define LASER_404        A5

// Número de canais do espectrômetro
#define SPEC_CHANNELS    288

uint16_t data[SPEC_CHANNELS];

// Espectro de referência fictício (substitua pelos seus dados reais!)
uint16_t data_ref[SPEC_CHANNELS] = {
  137,137,134,132,130,130,130,130,129,127,129,127,126,128,125,124,127,124,125,125,122,125,125,123,126,123,126,125,124,126,125,124,126,124,126,126,125,126,126,126,130,127,130,132,132,137,135,137,139,140,146,149,151,158,163,170,179,185,189,189,185,184,179,178,178,173,174,173,176,179,179,183,188,191,196,197,200,203,205,205,206,202,208,205,204,210,211,211,220,221,222,224,227,234,236,237,243,239,244,245,243,243,245,246,247,247,250,252,254,259,259,266,272,276,280,283,286,293,294,298,308,306,312,318,313,321,324,317,325,318,323,324,320,324,325,326,331,337,336,341,343,342,341,339,344,339,340,338,337,334,332,336,337,335,337,339,340,336,335,332,333,328,329,327,328,330,328,331,332,324,326,324,319,324,318,316,320,317,321,320,321,323,319,313,315,309,311,303,303,308,304,306,311,309,310,310,307,306,302,301,301,297,297,296,290,294,294,297,297,295,294,293,290,291,285,285,284,279,278,279,276,277,276,276,276,276,276,274,272,274,270,267,268,263,261,258,256,252,253,249,253,248,247,246,245,246,244,241,244,239,240,240,238,238,238,234,233,228,227,227,224,223,223,220,218,216,219,217,215,214,212,212,214,211,213,211,207,206,204,202,202,199,200,196,197,194,193,193


  // Preenche até 288 elementos (aqui é exemplo, pode repetir ou copiar valores reais)
};

// --- Função de cálculo de correlação de Pearson ---
float computeCorrelation(uint16_t *x, uint16_t *y, int n) {
  float mean_x = 0, mean_y = 0;
  for (int i = 0; i < n; i++) {
    mean_x += x[i];
    mean_y += y[i];
  }
  mean_x /= n;
  mean_y /= n;

  float num = 0, den_x = 0, den_y = 0;
  for (int i = 0; i < n; i++) {
    float dx = x[i] - mean_x;
    float dy = y[i] - mean_y;
    num += dx * dy;
    den_x += dx * dx;
    den_y += dy * dy;
  }

  float denom = sqrt(den_x * den_y);
  if (denom == 0) return 0;
  // Valor entre -1 e 1
  return num / denom;
}

void setup() {
  pinMode(SPEC_CLK, OUTPUT);
  pinMode(SPEC_ST, OUTPUT);
  pinMode(LASER_404, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);

  digitalWrite(SPEC_CLK, HIGH); // Set SPEC_CLK High
  digitalWrite(SPEC_ST, LOW);   // Set SPEC_ST Low

  Serial.begin(115200);
  Serial.println("Iniciando leitura do espectrômetro...");
}

void readSpectrometer() {
  int delayTime = 1;
  // delay em microssegundos

  // Pulso de start
  digitalWrite(SPEC_CLK, LOW);
  delayMicroseconds(delayTime);
  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(SPEC_CLK, LOW);
  digitalWrite(SPEC_ST, HIGH);
  delayMicroseconds(delayTime);

  // 15 pulsos
  for (int i = 0; i < 15; i++) {
    digitalWrite(SPEC_CLK, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(SPEC_CLK, LOW);
    delayMicroseconds(delayTime);
  }

  digitalWrite(SPEC_ST, LOW);

  // 85 pulsos
  for (int i = 0; i < 85; i++) {
    digitalWrite(SPEC_CLK, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(SPEC_CLK, LOW);
    delayMicroseconds(delayTime);
  }

  // Pulso extra antes da leitura
  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(SPEC_CLK, LOW);
  delayMicroseconds(delayTime);

  // Leitura dos 288 canais
  for (int i = 0; i < SPEC_CHANNELS; i++) {
    data[i] = analogRead(SPEC_VIDEO);
    digitalWrite(SPEC_CLK, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(SPEC_CLK, LOW);
    delayMicroseconds(delayTime);
  }

  digitalWrite(SPEC_ST, HIGH);

  // 7 pulsos finais
  for (int i = 0; i < 7; i++) {
    digitalWrite(SPEC_CLK, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(SPEC_CLK, LOW);
    delayMicroseconds(delayTime);
  }

  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(delayTime);
}

void loop() {
  readSpectrometer();

  // Calcula correlação entre espectro atual e espectro de referência
  float corr = computeCorrelation(data, data_ref, SPEC_CHANNELS);

  Serial.print("Correlacao: ");
  // 4 casas decimais
  Serial.println(corr, 4);

  if (corr > 0.9) {
    Serial.println(">>> Espectro MUITO parecido com a referência!");
  } else if (corr > 0.7) {
    Serial.println(">>> Espectro parcialmente similar.");
  } else {
    Serial.println(">>> Espectro diferente da referência.");
  }

  // Pequena pausa entre leituras
  delay(100);
}
