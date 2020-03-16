
void setupSD()
{
  //pinMode(CSpin, OUTPUT);
  
                                                         // see if the card is present and can be initialized:
  if (!SD.begin(10)) 
  {
    Serial.println("Card failed, or not present");     // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  /*
  Serial.println("Creating Packets.csv...");
  sensorData = SD.open("Packets.txt", FILE_WRITE);
  sensorData.close();
  */
  Serial.println("Creating Hello.csv...");
  sensorData = SD.open("Hello.csv", FILE_WRITE);
  if (!sensorData)
  {
    Serial.println("File Creation Failed!");
  }
  sensorData.close();
}

//
void writeToSD()
{
  // build the data string
  String dataString = Tele; // convert to CSV
  if(SD.exists("Hello.csv")) // check the card is still there
  { 
    // now append new data file
    sensorData = SD.open("Hello.csv", FILE_WRITE);
    if (sensorData)
    {
      sensorData.println(dataString);
    }
    else
    {
      Serial.println("Error Writting to file");
    }
  }
  else
  {
    //Serial.println("File doesn't exist");
  }
  sensorData.close(); // close the file
  /*
  String pktString = String(pkt);
  //if(SD.exists("Packets.csv")) // check the card is still there
  //{ 
    // now append new data file
    sensorData = SD.open("Packets.txt", FILE_WRITE);
    if (sensorData)
    {
      sensorData.println(pktString);
      sensorData.close(); // close the file
    }
  //}
  else
  {
    Serial.println("Error writing to file !");
  }
  */
}
