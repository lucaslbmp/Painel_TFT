void tacometro_setup(){
    int x,y; //Posiçoes dos numeros da escala
    unsigned short dx = 45, dy=deltayTacometro/div_tacometro; //Comprimento e altura do retangulo
    myGLCD.setColor(255, 255, 255);
    myGLCD.setFont(franklingothic_normal);
    x = 3;
    //y=212+myGLCD.getFontYsize()/2; // O numero fica centralizado com a linha divisoria entre os retangulos
    y = 212;
    for(int i=0;i<=div_tacometro;i++){
      float res = float(max_rpm)/div_tacometro/1000; //Resolucao em [10^3 rpm]
      if(i%tac_mark==0)        
        myGLCD.print(String(i*res,1), x, y-dy, 0);
        //myGLCD.print("--", x, y-dy, 0);
        y -= dy+2;
    }
    myGLCD.print("rpm", 50, 220, 0);//onde foi feito a mudança da letra rpm
    myGLCD.setFont(SmallFont);
    myGLCD.print("x1000", 5, 225, 0);
}

void velocidade_setup(){
  myGLCD.setColor(255, 180, 50);
  myGLCD.setFont(BigFont);
  myGLCD.print("km/h",115, 163, 0);
}

void combustivel_setup(){
  int x=268,y=152; //Canto inferior esquerdo do ultimo retangulo
  //================ Atualizando o indicador de combustivel ==========================
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print("E", x-myGLCD.getFontXsize(), y-myGLCD.getFontYsize()/2, 0);
  myGLCD.print("F", x-myGLCD.getFontXsize(), y-deltayCombustivel-myGLCD.getFontYsize(), 0);
  myGLCD.setFont(SmallFont);
  myGLCD.print("1/2", x-3*myGLCD.getFontXsize()-3, y-deltayCombustivel/2-myGLCD.getFontYsize(), 0);
  //==================================================================================
  //================ Atualizando o indicador de autonomia ============================
  myGLCD.setColor(64,150, 200);
  myGLCD.setFont(BigFont);
  myGLCD.print("km",318-2*myGLCD.getFontXsize(), 188, 0);
  myGLCD.setFont(SmallFont);
  myGLCD.print("Aut",318-3.5*myGLCD.getFontXsize(), 218, 0);
  //==================================================================================
}

void temperatura_setup(){
  myGLCD.setColor(255,110, 0);
  myGLCD.setFont(SmallFont);
  myGLCD.print("o",197, 184, 0);
  myGLCD.print("CVT",198, 218, 0);
  myGLCD.setFont(BigFont);
  myGLCD.print("C",203, 187, 0);
}

void bateria_setup(){
  int dx = int(float(len_barra_bat/div_bateria)*nv_bat);
  int dy = w_barra_bat;
  myGLCD.drawBitmap(xbat, ybat, 64, 37, bateria_0, 1);
  myGLCD.setColor(255,255, 255);
  myGLCD.fillRoundRect(xbat+10,ybat+10,xbat+10+dx,ybat+10+dy);
}
