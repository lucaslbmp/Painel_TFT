void getDados(){
  while(Serial3.read()>=0);
  Serial3.print('s');
  delay(20);
  if(Serial3.available()){
       auxD = Serial3.readStringUntil('\n');
       t_last_d = millis();
  }
  while(Serial2.read()>=0);
  Serial2.print('s');
  delay(20);
  if(Serial2.available()){
       auxT = Serial2.readStringUntil('\n');
       t_last_t = millis();
  }
  /*if(gps.f_speed_kmph()!= TinyGPS::GPS_INVALID_F_SPEED){
    auxG = "g"+String(gps.f_speed_kmph(),3)+",";
    smartdelay(200);
  }
  else{
    auxG = "g*,";
  }*/
  
  // Checagem de erros
  if(millis() - t_last_d >= 1000)
    auxD = "V*,";
  if(millis() - t_last_t >= 1000)
    auxT = "r*,c*,";
    
  String dados = auxD + auxT + auxG;
  Serial.println(dados); //apagar

  for(int i=0;dados != "";i++){
   if(dados.indexOf(',')==-1)         break;
   while(!isAlpha(dados[0]))          dados.remove(0,1);
   String str_val = dados.substring(0,dados.indexOf(','));
   Serial.println("str_val: "+str_val);
   String str_num = str_val.substring(1);
   atribuiVariavel(str_val[0],str_num.toInt(),str_num.toInt()>0 ^ str_num != "0");
   dados.replace( (str_val+",") , "" );
   Serial.println(dados);
   //dados = dados.substring(dados.indexOf(',')+1);
  }
  delay(30);
}

bool isStringAlphaNum(String dados){
  for(int i=0;i<dados.length();i++){
    if(isAlphaNumeric(dados[i]))        return true;
  }
  return false;
}

void atribuiVariavel(char var,int valor,bool erro){
  Serial.print(var);
  Serial.print(" = ");
  Serial.print(valor);
  Serial.print(" --> erro? ");
  Serial.print(erro);
  Serial.println();
  if(var=='V')                      {vel = valor; vel_err = erro;}
  if(var=='r')                      {rpm = valor; rpm_err = erro;}
  if(var=='c')                      {nv_comb = valor; comb_err = erro;}
  if(var == 'k' && nv_analog_on)    {nv_comb = valor; comb_err = erro;}
  if(var == 'g' && gps_on)          {vel = valor; gps_err = erro;}
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}

void verifica_comunicacao(){
  bool diana_err = vel_err, tarsila_err = rpm_err || comb_err; 
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(255,255*(1-diana_err),255*(1-diana_err));
  myGLCD.print("D",x_icone_uart+2,y_icone_uart+36,0);
  myGLCD.setColor(255,255*(1-tarsila_err),255*(1-tarsila_err));
  myGLCD.print("T",x_icone_uart+18,y_icone_uart+36,0);
  myGLCD.setColor(255,255*(1-gps_err),255*(1-gps_err));
  myGLCD.print("G",x_icone_uart+34,y_icone_uart+36,0);
}

void tacometro(int rpm){
  if(rpm<0)     rpm=0;
  else if(rpm>=max_rpm) rpm = max_rpm;
  // =============== Atualizando o indicador de rotacao ======================
  int r,g,b=0;
  int x=55,y=212; //Canto inferior esquerdo do ultimo retangulo
  unsigned short dx = 45, dy=deltayTacometro/div_tacometro; //Comprimento e altura do retangulo
  unsigned short resolucao = max_rpm/div_tacometro; //Quantos rpm cada retangulo representa
  unsigned char num_rpm = rpm/resolucao; // numero de retangulos que serao pintados
  r = 255/div_tacometro*num_rpm; // vermelho
  g = 255 - 255/div_tacometro*num_rpm; //verde
  myGLCD.setColor(r, g, b);
  for(int i=0;i<num_rpm;i++){ // Desenhando os retangulos que serao pintados
    myGLCD.fillRoundRect(x,y,x+dx,y-dy);
    y -= dy+2; // Coordenada y do novo retangulo
  }
  myGLCD.setColor(32, 32, 32);
  for(int i=num_rpm;i<div_tacometro;i++){ // Desenhando os retangulos que nao serao pintados
    myGLCD.fillRoundRect(x,y,x+dx,y-dy);
    y -= dy+2;  // Coordenada y do novo retangulo
  }
  // ===================================================================
}

void velocidade(int vel){
  if(vel<0)         vel=0;
  else if(vel>99)   vel=99;
  // =============== Atualizando o indicador de velocidade ======================
  myGLCD.setFont(SevenSeg_XXXL_Num);
  if(!vel_err){
     myGLCD.setColor(255, 180, 50);
    myGLCD.print(String(vel/10),110, 60, 0);
    myGLCD.print(String(vel%10),110+myGLCD.getFontXsize(), 60, 0);
  }
  else{
     myGLCD.setColor(150,100,0);
     myGLCD.print("00",110, 60, 0);
  }
}

// ===========================================================================

void combustivel(int nivel){
  if(nivel<0)                 nivel=0;
  else if(nivel>div_comb)     nivel=div_comb;      

  // ============== Atualizando o icone de combustivel ==========================
  if(nivel<=1){ //Se a temperatura subir acima de max_temp, o icone fica piscando
    myGLCD.drawBitmap(270, 10, 42, 40, icone_combustivel[!isFuelRed], 1.2);
    isFuelRed=!isFuelRed;
  }
  else
    myGLCD.drawBitmap(270, 10, 42, 40, icone_combustivel[0], 1.2);
  // ============================================================================
  // =============== Atualizando o indicador de combustivel ======================
  int x=268,y=152; //Canto inferior esquerdo do ultimo retangulo
  unsigned short dx = 45, dy=deltayCombustivel/div_comb; //Comprimento e altura do retangulo
  myGLCD.setColor(255, 255, 0);
  for(int i=0;i<nivel;i++){ // Desenhando os retangulos que serao pintados
    myGLCD.fillRect(x,y,x+dx,y-dy);
    y -= dy+2; // Coordenada y do novo retangulo
  }
  myGLCD.setColor(32, 32, 32);
  for(int i=nivel;i<div_comb;i++){ // Desenhando os retangulos que nao serao pintados
    myGLCD.fillRect(x,y,x+dx,y-dy);
    y -= dy+2;  // Coordenada y do novo retangulo
  }
  // ============================================================================
  
  // =============== Atualizando o indicador de autonomia =======================
  int autonomia = int(max_volume*consumo*nivel/div_comb);
  myGLCD.setColor(64,150, 200);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print(String(autonomia/10),225, 184, 0);
  myGLCD.print(String(autonomia%10),225+myGLCD.getFontXsize(), 184, 0);
  //Serial.print("auton.: "); Serial.println(autonomia);
  // ============================================================================
}

void termometro(float temperatura){
  if(temperatura<0)     temperatura=0;
  else if(temperatura>999)     temperatura=999; //apagar depois
  int temp = int(temperatura);
  
  // =============== Atualizando o icone de temperatura =======================
  if(temperatura > max_temp){ //Se a temperatura subir acima de max_temp, o icone fica piscando
    myGLCD.drawBitmap(x_temp_icone, y_temp_icone, len_temp_icone, w_temp_icone, icone_temperatura[!isTempRed], 1);
    isTempRed=!isTempRed;
  }
  else
    myGLCD.drawBitmap(x_temp_icone, y_temp_icone, len_temp_icone, w_temp_icone, icone_temperatura[0], 1);
   // ============================================================================
    
  // =================== Atualizando a temperatura no TFT =======================
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(255,110, 0);
  myGLCD.print(String(temp/10%10),102+myGLCD.getFontXsize(), 184, 0);
  myGLCD.print(String(temp%10),102+2*myGLCD.getFontXsize(), 184, 0);
  if(temp>=100)          myGLCD.print(String(temp/100),102, 184, 0);
  else                   {myGLCD.setColor(0,0,0); myGLCD.print(String(temp/100),102, 184, 0);}
  // =================================================== =======================
}

void bateria(int nv_bat){
  if(nv_bat<0)        nv_bat=0;
  else if(nv_bat>4)   nv_bat=4;
  int dx = int(float(len_barra_bat/div_bateria)*nv_bat);
  int dy = w_barra_bat;
  //myGLCD.drawBitmap(183, 13, 72, 42, icone_bateria[nv_bat], 1);
  if(nv_bat < nv_bat_ant){
    myGLCD.setColor(0,0, 0);
    myGLCD.fillRoundRect(xbat+10,ybat+10,xbat+10+len_barra_bat,ybat+10+dy);
  }
  myGLCD.setColor(255,255, 255);
  if(dx!=0)     myGLCD.fillRoundRect(xbat+10,ybat+10,xbat+10+dx,ybat+10+dy);
  //Serial.println(dx);
  nv_bat_ant = nv_bat;
}

void zigbee(){
  //if(count_excel == 210 || count_excel == 0){
      //count_excel = 0;
      //mySerial.println("ROW,SET,2");
      //mySerial.println("CLEARDATA");
      //mySerial.print("Time,Counter,millis,rpm,vel,comb,temp");
      //delay(100);
      //mySerial.println("comb,temp,verif,freio");
      //delay(100);
     // }
    //mySerial.print("DATA,TIME,");
    //count_excel++;
    //mySerial.print(count_excel);
    //mySerial.print(',');
    mySerial.print('t'); mySerial.print(millis());mySerial.print(',');
    mySerial.print('r');mySerial.print(rpm);mySerial.print(',');
    mySerial.print('v');mySerial.print(vel);mySerial.print(',');
    mySerial.print('c');mySerial.print(nv_comb); mySerial.print(',');
    mySerial.print('T');mySerial.print(temperatura);
    //mySerial.print(1);
    //mySerial.print('F');mySerial.print(out_freio);mySerial.print(',');
    help_show();
    mySerial.println();
    delay(100);
}

void help_show(){
  if(help == 1){
    if(c_help<500){
        mySerial.println("SOCORRO");
        c_help++;
    }
    else
    {
      c_help = 0;
      help = 0;
    }
  }
}


void teste(){
  if(rpm<0 && !rpm_up || rpm<6000 && rpm_up)                    {rpm += 700; rpm_up = 1;}
  else                                                          {rpm -= 700; rpm_up = 0;}
  if(vel<0 && !vel_up || vel<99 && vel_up)                     {vel += 6; vel_up = 1;}
  else                                                          {vel -= 6; vel_up = 0;}
  if(temperatura<0 && !temp_up || temperatura<129 && temp_up)   {temperatura += 5; temp_up = 1;}
  else                                                         {temperatura -= 5; temp_up = 0;}
  if(nv_comb<0 && !comb_up || nv_comb<div_comb && comb_up)    {nv_comb++; comb_up = 1;}
  else                                                         {nv_comb--; comb_up = 0;}
  if(nv_bat<0 && !bat_up || nv_bat<div_bateria && bat_up)               {nv_bat++; bat_up = 1;}
  else                                                         {nv_bat--; bat_up = 0;}
  Serial.print("rpm: ");Serial.print(rpm);Serial.print("  ");
  Serial.print("vel: "); Serial.print(vel);Serial.print("  ");
  Serial.print("temp: "); Serial.print(temperatura);Serial.print("  ");
  Serial.println("t");
}
