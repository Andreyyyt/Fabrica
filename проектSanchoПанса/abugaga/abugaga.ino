#define MOTOR1_ENA 2   // ШИМ для мотора 1 (левый перед)
#define MOTOR1_IN1 3   // Направление мотора 1
#define MOTOR2_ENB 4   // ШИМ для мотора 2 (левый зад)
#define MOTOR2_IN3 5   // Направление мотора 2

#define MOTOR3_ENA 6   // ШИМ для мотора 3 (правый перед)
#define MOTOR3_IN1 7   // Направление мотора 3
#define MOTOR4_ENB 8   // ШИМ для мотора 4 (правый зад)
#define MOTOR4_IN3 9   // Направление мотора 4

// Настройки для скорости ~2 км/ч
#define MAX_SPEED 40     // Максимальная скорость (очень медленно)
#define ACCEL_STEP 1     // Шаг ускорения (супер плавно)
#define BASE_DELAY 80    // Базовая задержка для плавности

void setup() {
  // Настройка всех пинов
  pinMode(MOTOR1_ENA, OUTPUT);
  pinMode(MOTOR1_IN1, OUTPUT);
  pinMode(MOTOR2_ENB, OUTPUT);
  pinMode(MOTOR2_IN3, OUTPUT);
  pinMode(MOTOR3_ENA, OUTPUT);
  pinMode(MOTOR3_IN1, OUTPUT);
  pinMode(MOTOR4_ENB, OUTPUT);
  pinMode(MOTOR4_IN3, OUTPUT);

  Serial.begin(115200);
  delay(2000);
  Serial.println("🤖 1-МИНУТНАЯ ДЕМОНСТРАЦИЯ (~2 км/ч)");
  Serial.println("🎯 Все движения супер-плавные!");
}

void loop() {
  oneMinuteDemo();
  
  // Завершение программы
  Serial.println("\n✨ ПРОГРАММА ЗАВЕРШЕНА!");
  while(true) {
    delay(1000);
  }
}

// ========== ПЛАВНЫЕ ФУНКЦИИ ДВИЖЕНИЯ ==========

// Очень плавное движение меканум-колес
void mecanumMove(int x, int y, int r) {
  static int lastX = 0, lastY = 0, lastR = 0;
  
  // Ограничение скорости
  x = constrain(x, -MAX_SPEED, MAX_SPEED);
  y = constrain(y, -MAX_SPEED, MAX_SPEED);
  r = constrain(r, -MAX_SPEED, MAX_SPEED);
  
  // Плавное изменение скорости (линейная интерполяция)
  if (abs(x - lastX) > 1) {
    lastX += (x > lastX) ? 1 : -1;
  }
  if (abs(y - lastY) > 1) {
    lastY += (y > lastY) ? 1 : -1;
  }
  if (abs(r - lastR) > 1) {
    lastR += (r > lastR) ? 1 : -1;
  }
  
  // Расчет скоростей колес
  int speedLF = constrain(lastX + lastY + lastR, -MAX_SPEED, MAX_SPEED);
  int speedLB = constrain(lastX - lastY + lastR, -MAX_SPEED, MAX_SPEED);
  int speedRF = constrain(lastX - lastY - lastR, -MAX_SPEED, MAX_SPEED);
  int speedRB = constrain(lastX + lastY - lastR, -MAX_SPEED, MAX_SPEED);
  
  // Управление моторами
  setMotor(MOTOR1_ENA, MOTOR1_IN1, speedLF);
  setMotor(MOTOR2_ENB, MOTOR2_IN3, speedLB);
  setMotor(MOTOR3_ENA, MOTOR3_IN1, speedRF);
  setMotor(MOTOR4_ENB, MOTOR4_IN3, speedRB);
  
  delay(20); // Для плавности
}

// Плавное управление мотором
void setMotor(int pwmPin, int dirPin, int speed) {
  speed = constrain(speed, -MAX_SPEED, MAX_SPEED);
  
  if (speed > 0) {
    digitalWrite(dirPin, HIGH);
    analogWrite(pwmPin, speed);
  } else if (speed < 0) {
    digitalWrite(dirPin, LOW);
    analogWrite(pwmPin, -speed);
  } else {
    analogWrite(pwmPin, 0);
  }
}

// Плавный разгон до целевой скорости
void smoothAccelerate(int targetX, int targetY, int targetR, int duration) {
  int steps = duration / BASE_DELAY;
  if (steps < 1) steps = 1;
  
  for (int i = 0; i <= steps; i++) {
    float t = (float)i / steps;
    int x = targetX * t;
    int y = targetY * t;
    int r = targetR * t;
    
    mecanumMove(x, y, r);
    delay(BASE_DELAY);
  }
}

// Плавное торможение
void smoothDecelerate(int duration) {
  int steps = duration / BASE_DELAY;
  if (steps < 1) steps = 1;
  
  for (int i = steps; i >= 0; i--) {
    float t = (float)i / steps;
    mecanumMove(0, 0, 0); // Функция сама плавно снизит скорость
    delay(BASE_DELAY);
  }
  
  // Полная остановка
  analogWrite(MOTOR1_ENA, 0);
  analogWrite(MOTOR2_ENB, 0);
  analogWrite(MOTOR3_ENA, 0);
  analogWrite(MOTOR4_ENB, 0);
}

// ========== 1-МИНУТНАЯ ДЕМОНСТРАЦИЯ ==========
void oneMinuteDemo() {
  unsigned long startTime = millis();
  unsigned long elapsed;
  
  Serial.println("\n⏱️  НАЧАЛО 1-МИНУТНОЙ ДЕМОНСТРАЦИИ");
  delay(1000);
  
  // 1. ПЛАВНО ВПЕРЕД (5 секунд)
  Serial.println("\n1️⃣ ВПЕРЕД (5 сек)");
  smoothAccelerate(MAX_SPEED, 0, 0, 2000); // 2 сек на разгон
  delay(3000); // 3 сек равномерного движения
  smoothDecelerate(2000); // 2 сек на торможение
  delay(500);
  
  elapsed = millis() - startTime;
  Serial.print("   Прошло: ");
  Serial.print(elapsed / 1000);
  Serial.println(" сек");
  
  // 2. ПЛАВНО ВЛЕВО БОКОМ (5 секунд)
  Serial.println("\n2️⃣ ВЛЕВО БОКОМ (5 сек)");
  smoothAccelerate(0, -MAX_SPEED, 0, 2000);
  delay(3000);
  smoothDecelerate(2000);
  delay(500);
  
  elapsed = millis() - startTime;
  Serial.print("   Прошло: ");
  Serial.print(elapsed / 1000);
  Serial.println(" сек");
  
  // 3. ПЛАВНО ВПРАВО БОКОМ (5 секунд)
  Serial.println("\n3️⃣ ВПРАВО БОКОМ (5 сек)");
  smoothAccelerate(0, MAX_SPEED, 0, 2000);
  delay(3000);
  smoothDecelerate(2000);
  delay(500);
  
  elapsed = millis() - startTime;
  Serial.print("   Прошло: ");
  Serial.print(elapsed / 1000);
  Serial.println(" сек");
  
  // 4. ДИАГОНАЛЬ ВПЕРЕД-ВПРАВО (5 секунд)
  Serial.println("\n4️⃣ ДИАГОНАЛЬ: ВПЕРЕД-ВПРАВО (5 сек)");
  smoothAccelerate(MAX_SPEED/2, MAX_SPEED/2, 0, 2000);
  delay(3000);
  smoothDecelerate(2000);
  delay(500);
  
  elapsed = millis() - startTime;
  Serial.print("   Прошло: ");
  Serial.print(elapsed / 1000);
  Serial.println(" сек");
  
  // 5. ДИАГОНАЛЬ ВПЕРЕД-ВЛЕВО (5 секунд)
  Serial.println("\n5️⃣ ДИАГОНАЛЬ: ВПЕРЕД-ВЛЕВО (5 сек)");
  smoothAccelerate(MAX_SPEED/2, -MAX_SPEED/2, 0, 2000);
  delay(3000);
  smoothDecelerate(2000);
  delay(500);
  
  elapsed = millis() - startTime;
  Serial.print("   Прошло: ");
  Serial.print(elapsed / 1000);
  Serial.println(" сек");
  
  // 6. ПЛАВНЫЙ РАЗВОРОТ 180° (7 секунд)
  Serial.println("\n6️⃣ РАЗВОРОТ 180° (7 сек)");
  
  // Плавный разгон поворота
  for (int i = 0; i <= MAX_SPEED; i += 1) {
    mecanumMove(0, 0, i);
    delay(60);
  }
  
  // Основной поворот (3 секунды)
  delay(3000);
  
  // Плавное торможение поворота
  for (int i = MAX_SPEED; i >= 0; i -= 1) {
    mecanumMove(0, 0, i);
    delay(60);
  }
  
  smoothDecelerate(1000);
  delay(500);
  
  elapsed = millis() - startTime;
  Serial.print("   Прошло: ");
  Serial.print(elapsed / 1000);
  Serial.println(" сек");
  
  // 7. ПЛАВНО НАЗАД (5 секунд)
  Serial.println("\n7️⃣ НАЗАД (5 сек)");
  smoothAccelerate(-MAX_SPEED, 0, 0, 2000);
  delay(3000);
  smoothDecelerate(2000);
  delay(500);
  
  elapsed = millis() - startTime;
  Serial.print("   Прошло: ");
  Serial.print(elapsed / 1000);
  Serial.println(" сек");
  
  // 8. ДИАГОНАЛЬ НАЗАД-ВПРАВО (5 секунд)
  Serial.println("\n8️⃣ ДИАГОНАЛЬ: НАЗАД-ВПРАВО (5 сек)");
  smoothAccelerate(-MAX_SPEED/2, MAX_SPEED/2, 0, 2000);
  delay(3000);
  smoothDecelerate(2000);
  delay(500);
  
  elapsed = millis() - startTime;
  Serial.print("   Прошло: ");
  Serial.print(elapsed / 1000);
  Serial.println(" сек");
  
  // 9. ДИАГОНАЛЬ НАЗАД-ВЛЕВО (5 секунд)
  Serial.println("\n9️⃣ ДИАГОНАЛЬ: НАЗАД-ВЛЕВО (5 сек)");
  smoothAccelerate(-MAX_SPEED/2, -MAX_SPEED/2, 0, 2000);
  delay(3000);
  smoothDecelerate(2000);
  delay(500);
  
  elapsed = millis() - startTime;
  Serial.print("   Прошло: ");
  Serial.print(elapsed / 1000);
  Serial.println(" сек");
  
  // 10. ПЛАВНЫЙ РАЗВОРОТ В ИСХОДНОЕ ПОЛОЖЕНИЕ (7 секунд)
  Serial.println("\n🔟 ВОЗВРАТ В ИСХОДНУЮ ПОЗИЦИЮ (7 сек)");
  
  // Плавный разгон поворота
  for (int i = 0; i <= MAX_SPEED; i += 1) {
    mecanumMove(0, 0, -i); // Поворот в другую сторону
    delay(60);
  }
  
  // Основной поворот (3 секунды)
  delay(3000);
  
  // Плавное торможение поворота
  for (int i = MAX_SPEED; i >= 0; i -= 1) {
    mecanumMove(0, 0, -i);
    delay(60);
  }
  
  smoothDecelerate(1000);
  
  // ФИНАЛЬНАЯ ПАУЗА
  delay(1000);
  
  elapsed = millis() - startTime;
  Serial.println("\n✅ ДЕМОНСТРАЦИЯ ЗАВЕРШЕНА!");
  Serial.print("⏱️  Общее время: ");
  Serial.print(elapsed / 1000);
  Serial.println(" секунд");
}

// ========== ТАЙМИНГ ДЕМОНСТРАЦИИ ==========
/*
РАСЧЕТ ВРЕМЕНИ:
1. Вперед: 5 сек
2. Влево: 5 сек
3. Вправо: 5 сек
4. Диаг. вперед-вправо: 5 сек
5. Диаг. вперед-влево: 5 сек
6. Разворот 180°: 7 сек
7. Назад: 5 сек
8. Диаг. назад-вправо: 5 сек
9. Диаг. назад-влево: 5 сек
10. Разворот обратно: 7 сек
Паузы между движениями: ~4 сек

ИТОГО: ~58-60 секунд
*/


