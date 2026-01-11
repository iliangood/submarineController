import java.io.*;

float pitch = 0;      // Тангаж в градусах (-90..+90)
float roll = 0;       // Крен в радианах
float pitchSpeed = 0;
float rollSpeed = 0;
float damping = 0.92;

BufferedReader reader;
PGraphics maskLayer;  // Маска создаётся один раз и обновляется при ресайзе

void setup() {
  size(800, 800);
  smooth();
  surface.setResizable(true);
  surface.setTitle("Авиагоризонт");
  
  // Инициализация чтения stdin
  reader = new BufferedReader(new InputStreamReader(System.in));
  
  // Создаём маску сразу (будет обновляться в windowResized)
  createMask();
  
  println("Авиагоризонт запущен. Ожидаю данные по stdin...");
  println("Формат: pitch:45.5 roll:-30.0   (градусы)");
}

void draw() {
  background(30);
  
  // Обновление углов с инерцией (для теста клавишами)
  pitch += pitchSpeed;
  roll += rollSpeed;
  pitchSpeed *= damping;
  rollSpeed *= damping;
  
  pitch = constrain(pitch, -90, 90);
  
  float diam = min(width, height) * 0.75;
  float pixelsPerDegree = (diam * 0.8) / 180.0;
  
  // Чтение команд из stdin
  try {
    while (reader.ready()) {
      String line = reader.readLine();
      if (line != null) {
        line = trim(line);
        if (line.length() > 0) {
          println("Получено из stdin: " + line);
          parseCommand(line);
        }
      }
    }
  } catch (IOException e) {
    e.printStackTrace();
  }
  
  // Подвижный слой (небо + земля + разметка)
  PGraphics bg = createGraphics(width, height);
  bg.beginDraw();
  bg.clear();
  bg.translate(width/2, height/2);
  bg.rotate(-roll);
  bg.translate(0, pitch * pixelsPerDegree);
  
  bg.noStroke();
  bg.fill(0, 120, 255);
  bg.rect(-width*2, -height*2, width*4, height*2);
  
  bg.fill(139, 90, 43);
  bg.rect(-width*2, 0, width*4, height*2);
  
  bg.stroke(255);
  bg.strokeWeight(6);
  bg.line(-diam, 0, diam, 0);
  
  bg.stroke(255);
  bg.strokeWeight(4);
  bg.textSize(20);
  bg.textAlign(CENTER, CENTER);
  bg.fill(255);
  
  for (int deg = -90; deg <= 90; deg += 10) {
    if (deg == 0) continue;
    float y = deg * pixelsPerDegree;
    float len = (abs(deg) % 30 == 0) ? diam * 0.4 : diam * 0.2;
    bg.line(-len, y, len, y);
    
    if (abs(deg) % 20 == 0 && abs(deg) >= 20 && abs(deg) <= 80) {
      bg.text(str(abs(deg)), -len - 40, y);
      bg.text(str(abs(deg)), len + 40, y);
    }
  }
  bg.endDraw();
  
  // Применяем маску (теперь размеры гарантированно совпадают)
  PImage masked = bg.get();
  masked.mask(maskLayer);
  image(masked, 0, 0);
  
  // Фиксированные элементы (самолётик, деления крена, рамка)
  translate(width/2, height/2);
  
  stroke(255, 220, 0);
  strokeWeight(5);
  fill(255, 220, 0);
  line(-diam*0.2, 0, -diam*0.05, 0);
  line(diam*0.2, 0, diam*0.05, 0);
  ellipse(0, 0, 24, 24);
  
  pushMatrix();
  rotate(roll);
  fill(255, 220, 0);
  noStroke();
  triangle(0, -diam/2 + 20, -20, -diam/2 + 50, 20, -diam/2 + 50);
  popMatrix();
  
  stroke(255);
  strokeWeight(3);
  for (int a = -60; a <= 60; a += 10) {
    if (a == 0) continue;
    float ang = radians(a);
    float r1 = diam/2 - 10;
    float len = (abs(a) == 30 || abs(a) == 45 || abs(a) == 60) ? 30 : 15;
    float r2 = r1 - len;
    line(cos(ang)*r1, sin(ang)*r1, cos(ang)*r2, sin(ang)*r2);
  }
  
  fill(255, 220, 0);
  noStroke();
  triangle(0, -diam/2 + 10, -10, -diam/2 + 30, 10, -diam/2 + 30);
  
  noFill();
  stroke(220);
  strokeWeight(12);
  ellipse(0, 0, diam + 20, diam + 20);
  
  resetMatrix();
  textAlign(LEFT, TOP);
  textSize(20);
  fill(255);
  text("Roll:  " + nf(degrees(roll) % 360, 0, 1) + "°", 20, 20);
  text("Pitch: " + nf(pitch, 0, 1) + "°", 20, 50);
}

// Функция создания/обновления маски при ресайзе
void createMask() {
  maskLayer = createGraphics(width, height);
  maskLayer.beginDraw();
  maskLayer.clear();
  maskLayer.background(0);
  maskLayer.fill(255);
  float diam = min(width, height) * 0.75;
  maskLayer.ellipse(width/2, height/2, diam, diam);
  maskLayer.endDraw();
}

// Обработчик изменения размера окна
void windowResized() {
  createMask();  // Пересоздаём маску под новый размер
}

// Парсинг команд из stdin
void parseCommand(String cmd) {
  String[] parts = split(cmd, ' ');
  for (String part : parts) {
    if (part.startsWith("pitch:")) {
      pitch = float(part.substring(6));
    } else if (part.startsWith("roll:")) {
      roll = radians(float(part.substring(5)));
    } else if (part.startsWith("p:")) {
      pitch = float(part.substring(2));
    } else if (part.startsWith("r:")) {
      roll = radians(float(part.substring(2)));
    }
  }
}

// Тест клавишами (опционально)
void keyPressed() {
  if (keyCode == LEFT)  rollSpeed = -0.06;
  if (keyCode == RIGHT) rollSpeed = 0.06;
  if (keyCode == UP)    pitchSpeed = -1.5;
  if (keyCode == DOWN)  pitchSpeed = 1.5;
}

void keyReleased() {
  if (keyCode == LEFT || keyCode == RIGHT) rollSpeed = 0;
  if (keyCode == UP || keyCode == DOWN) pitchSpeed = 0;
}
