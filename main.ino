#include <DHT.h>

#define PINO_N 26
#define PINO_P 25
#define PINO_K 33
#define PINO_DHT 13
#define TIPO_DHT DHT22
#define PINO_LDR 14
#define PINO_RELE 15

bool rele_ativo_alto = true;
float umidade_minima = 40.0;
float ph_minimo = 6.0;
float ph_maximo = 7.0;
bool inverter_ldr = false;

DHT dht(PINO_DHT, TIPO_DHT);

float limitar(float valor, float minimo, float maximo) {
  if (valor < minimo) return minimo;
  if (valor > maximo) return maximo;
  return valor;
}

float mapear(float valor, float in_min, float in_max, float out_min, float out_max) {
  return (valor - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void ligar_bomba() { digitalWrite(PINO_RELE, rele_ativo_alto ? HIGH : LOW); }
void desligar_bomba() { digitalWrite(PINO_RELE, rele_ativo_alto ? LOW : HIGH); }

void setup() {
  Serial.begin(115200);
  pinMode(PINO_N, INPUT_PULLDOWN);
  pinMode(PINO_P, INPUT_PULLDOWN);
  pinMode(PINO_K, INPUT_PULLDOWN);
  pinMode(PINO_RELE, OUTPUT);
  desligar_bomba();
  dht.begin();
}

void loop() {
  bool n = digitalRead(PINO_N);
  bool p = digitalRead(PINO_P);
  bool k = digitalRead(PINO_K);
  bool nutrientes_ok = (n || p || k);

  int leitura_ldr = analogRead(PINO_LDR);
  float ph;
  if (!inverter_ldr) {
    ph = mapear((float)leitura_ldr, 0.0, 4095.0, 0.0, 14.0);
  } else {
    ph = mapear((float)leitura_ldr, 0.0, 4095.0, 14.0, 0.0);
  }
  ph = limitar(ph, 0.0, 14.0);

  float umidade = dht.readHumidity();
  bool dht_ok = !isnan(umidade);

  Serial.print("N=");
  Serial.print(n);
  Serial.print(" P=");
  Serial.print(p);
  Serial.print(" K=");
  Serial.print(k);
  Serial.print(" | pH=");
  Serial.print(ph, 1);
  Serial.print(" | Umidade=");
  if (dht_ok) {
  Serial.print(umidade, 1); 
  }
  else {
  Serial.print("NaN");
  }
  Serial.print("% | ");

  bool irrigar = false;
  if (dht_ok) {
    if (umidade < umidade_minima && ph >= ph_minimo && ph <= ph_maximo && nutrientes_ok) {
      irrigar = true;
    }
  }

  if (irrigar) {
    ligar_bomba();
    Serial.println("Bomba ligada");
  } else {
    desligar_bomba();
    Serial.println("Bomba desligada");
  }

  delay(1500);
}

