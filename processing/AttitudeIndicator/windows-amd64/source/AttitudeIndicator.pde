import java.io.*;

float pitch = 0;
float roll = 0;        // хранится в радианах
float depth = 0;

float horizonOffsetX = 60; // сдвиг авиагоризонта вправо (в пикселях)

float depthTickStep = 10.0f; // шаг делений шкалы глубины (в метрах)
float depthMax = 50.0f; // максимальная глубина индикатора


BufferedReader reader;
PGraphics maskLayer;

void setup() {
  size(800, 800);
  smooth();
  surface.setResizable(true);
  surface.setTitle("Авиагоризонт + Глубина");
  
  reader = new BufferedReader(new InputStreamReader(System.in));
  
  createMask();
  
  //println("Авиагоризонт + индикатор глубины запущен");
  //println("Формат: pitch:45.5 roll:-30.0 depth:120.0 (можно также pitch=45 или разделять запятыми/пробелами)");
}

void draw() {
  background(30);
  
  // Чтение stdin (без блокировки)
  try {
    while (reader.ready()) {
      String line = reader.readLine();
      if (line != null) {
        line = trim(line);
        if (line.length() > 0) {
          //println("Получено: " + line);
          parseCommand(line);
        }
      }
    }
  } catch (IOException e) {
    // не фатально — только логируем
    e.printStackTrace();
  }
  
  // Ограничения значений
  pitch = constrain(pitch, -90, 90);
  
  float diam = min(width, height) * 0.75f;
  float pixelsPerDegree = (diam * 0.8f) / 180.0f; // 180 градусов -> span
  
  // Подвижный слой горизонта (PGraphics)
  PGraphics bg = createGraphics(width, height);
  bg.beginDraw();
  bg.clear();
  bg.translate(width/2.0f + horizonOffsetX, height/2.0f);
  bg.rotate(-roll); // вращаем весь слой по крену
  bg.translate(0, pitch * pixelsPerDegree); // смещение по тангажу
  
  // небо
  bg.noStroke();
  bg.fill(0, 120, 255);
  bg.rect(-width*2, -height*2, width*4, height*2);
  // земля
  bg.fill(139, 90, 43);
  bg.rect(-width*2, 0, width*4, height*2);
  
  // центральная линия горизонта
  bg.stroke(255);
  bg.strokeWeight(6);
  bg.line(-diam, 0, diam, 0);
  
  // отметки углов
  bg.stroke(255);
  bg.strokeWeight(4);
  bg.textSize(20);
  bg.textAlign(CENTER, CENTER);
  bg.fill(255);
  
  for (int deg = -90; deg <= 90; deg += 10) {
    if (deg == 0) continue;
    float y = deg * pixelsPerDegree;
    float len = (abs(deg) % 30 == 0) ? diam * 0.4f : diam * 0.2f;
    bg.line(-len, y, len, y);
    
    if (abs(deg) % 20 == 0 && abs(deg) >= 20 && abs(deg) <= 80) {
      bg.text(str(abs(deg)), -len - 40, y);
      bg.text(str(abs(deg)), len + 40, y);
    }
  }
  bg.endDraw();
  
  // Получаем изображение и применяем маску корректно
  PImage masked = bg.get();            // безопасно извлекает картинку из PGraphics
  PImage maskImg = maskLayer.get();    // получаем PImage из PGraphics
  
  if (maskImg.width != masked.width || maskImg.height != masked.height) {
    maskImg.resize(masked.width, masked.height);
  }
  
  // mask() изменяет исходный объект, поэтому делаем копию если нужно
  masked.mask(maskImg);
  image(masked, 0, 0);
  
  // Фиксированные элементы: самолётик, указатель, рамка
  pushMatrix();
  translate(width/2.0f + horizonOffsetX, height/2.0f);
  
  stroke(255, 220, 0);
  strokeWeight(5);
  fill(255, 220, 0);
  line(-diam*0.2f, 0, -diam*0.05f, 0);
  line(diam*0.2f, 0, diam*0.05f, 0);
  ellipse(0, 0, 24, 24);
  
  pushMatrix();
  rotate(roll); // стрелка самолёта корректируется по крену
  fill(255, 220, 0);
  noStroke();
  triangle(0, -diam/2.0f + 20, -20, -diam/2.0f + 50, 20, -diam/2.0f + 50);
  popMatrix();
  
  stroke(255);
  strokeWeight(3);
  for (int a = -60; a <= 60; a += 10) {
    if (a == 0) continue;
    float ang = radians(a);
    float r1 = diam/2.0f - 10;
    float len = (abs(a) == 30 || abs(a) == 45 || abs(a) == 60) ? 30 : 15;
    float r2 = r1 - len;
    line(cos(ang)*r1, sin(ang)*r1, cos(ang)*r2, sin(ang)*r2);
  }
  
  fill(255, 220, 0);
  noStroke();
  triangle(0, -diam/2.0f + 10, -10, -diam/2.0f + 30, 10, -diam/2.0f + 30);
  
  noFill();
  stroke(220);
  strokeWeight(12);
  ellipse(0, 0, diam + 20, diam + 20);
  
  popMatrix(); // вернулись в экранные координаты
  
  // Индикатор глубины слева (если окно слишком узкое — рисуем упрощённо)
  float depthBarX = 40;
  float depthBarWidth = 30;
  float depthBarHeight = height * 0.65f;
  
  float depthClamped = constrain(depth, 0, depthMax);

float depthY = map(depthClamped, 0, depthMax,
                   height/2.0f - depthBarHeight/2.0f,
                   height/2.0f + depthBarHeight/2.0f);
  
  if (width < 450) {
    // слишком узкое окно — просто текстовые значения
    fill(255);
    textSize(16);
    textAlign(LEFT, TOP);
    text("Roll:  " + nf((degrees(roll) + 360) % 360, 0, 1) + "°", 10, 10);
    text("Pitch: " + nf(pitch, 0, 1) + "°", 10, 30);
    text("Depth: " + nf(depth, 0, 1) + " m", 10, 50);
    return;
  }
  
  // боковая шкала глубины
  fill(40, 80);
  noStroke();
  rect(depthBarX, height/2.0f - depthBarHeight/2.0f, depthBarWidth, depthBarHeight, 6);
  
  // градиент (простая построчная заливка)
  for (int i = 0; i < (int)depthBarHeight; i++) {
    float norm = map(i, 0, depthBarHeight, 0, 1);
    // не задаём конкретные цвета HACK: используем вычисление, но Processing всё равно принимает RGB
    fill(0, 80 + 175*norm, 255);
    rect(depthBarX, height/2.0f - depthBarHeight/2.0f + i, depthBarWidth, 1);
  }
  
  textSize(12);
  fill(255);
  textAlign(LEFT, CENTER);
  for (float m = 0; m <= depthMax; m += depthTickStep) {
  float y = map(m, 0, depthMax,
                height/2.0f - depthBarHeight/2.0f,
                height/2.0f + depthBarHeight/2.0f);
  text(nf(m, 0, 0), depthBarX + depthBarWidth + 5, y);
}
  
  fill(255, 255, 0);
  noStroke();
  triangle(depthBarX + 2, depthY - 12,
           depthBarX + 22, depthY,
           depthBarX + 2, depthY + 12);
  
  fill(255);
  textSize(14);
  textAlign(LEFT, CENTER);
  text(nf(depth, 0, 1) + " м", depthBarX + depthBarWidth + 30, depthY);
  
  // Текстовая информация (верхний левый угол)
  textAlign(LEFT, TOP);
  textSize(20);
  fill(255);
  String rollText = nf((degrees(roll) + 360) % 360, 0, 1) + "°";
  text("Roll:  " + rollText, 20, 20);
  text("Pitch: " + nf(pitch, 0, 1) + "°", 20, 50);
}

// Пересоздаём маску при смене размеров
void createMask() {
  maskLayer = createGraphics(width, height);
  maskLayer.beginDraw();
  maskLayer.clear();
  maskLayer.background(0);
  maskLayer.fill(255);

  float diam = min(width, height) * 0.75f;
  maskLayer.ellipse(
    width/2.0f + horizonOffsetX,
    height/2.0f,
    diam,
    diam
  );

  maskLayer.endDraw();
}

void windowResized() {
  // при ресайзе пересоздаём маску
  createMask();
}

// Парсер команд: принимает форматы pitch:45, roll=-30, depth=120
// допускает разделители пробел или запятая
void parseCommand(String cmd) {
  // разбиваем по пробелам или запятым
  String[] parts = splitTokens(cmd, " ,\t");
  for (String part : parts) {
    part = trim(part);
    if (part.length() == 0) continue;
    try {
      if (matchesField(part, "pitch")) {
        String v = valueOfField(part);
        pitch = float(v);
      } else if (matchesField(part, "roll")) {
        String v = valueOfField(part);
        // входные roll ожидаются в градусах — переводим в радианы
        roll = radians(float(v));
      } else if (matchesField(part, "depth")) {
        String v = valueOfField(part);
        depth = float(v);
      } else {
        // также поддержка вида "pitch:45.5;roll:-10"
        // если не распознано — игнорируем
      }
    } catch (NumberFormatException e) {
      println("Не удалось распарсить число в части: " + part);
    }
  }
}

boolean matchesField(String part, String name) {
  String lower = part.toLowerCase();
  return lower.startsWith(name + ":") || lower.startsWith(name + "=");
}

String valueOfField(String part) {
  int idx = part.indexOf(':');
  if (idx < 0) idx = part.indexOf('=');
  if (idx >= 0 && idx < part.length()-1) {
    return part.substring(idx+1);
  }
  return "";
}
