
void setupSD()
{
  pinMode(CSpin, OUTPUT);
  //
  // see if the card is present and can be initialized:
  if (!SD.begin(CSpin)) 
  {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  
  Serial.println("Creating Packets.txt...");
  sensorData = SD.open("Packets.txt", FILE_WRITE);
  sensorData.close();

  Serial.println("Creating Telementry.txt...");
  sensorData = SD.open("Telementry.txt", FILE_WRITE);
  sensorData.close();
}

//
void writeToSD()
{
  // build the data string
  String dataString = Tele; // convert to CSV
  if(SD.exists("Telementry.csv")) // check the card is still there
  { 
    // now append new data file
    sensorData = SD.open("Telementry.csv", FILE_WRITE);
    if (sensorData)
    {
      sensorData.println(dataString);
      sensorData.close(); // close the file
    }
  }
  else
  {
    Serial.println("Error writing to file !");
  }
  
  String pktString = String(pkt);
  if(SD.exists("Packets.csv")) // check the card is still there
  { 
    // now append new data file
    sensorData = SD.open("Packets.csv", FILE_WRITE);
    if (sensorData)
    {
      sensorData.println(pktString);
      sensorData.close(); // close the file
    }
  }
  else
  {
    Serial.println("Error writing to file !");
  }
}
